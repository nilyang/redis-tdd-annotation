# GDB 调试 Segmentation fault

### GDB 追踪内存溢出问题

发现内存溢出问题

```
$ gcc -D SDS_TEST_MAIN  -Wall -g hello_sds.c -o sds
$ ./sds

1 - sdslen(x) test: PASSED
2 - Create a string and obtain the length: PASSED
3 - Create a string with specified length: PASSED
4 - sdsMakeRoomFor() enlarge sds->buf size: PASSED
5 - sdscat() foo + bar: PASSED
6 - sdscpy() against an originally longer string: PASSED
7 - sdscpy() against an originally shorter string: PASSED

Segmentation fault

```

使用gdb调试

```
$ gdb sds
(gdb) b 739
Breakpoint 1 at 0x40164d: file hello_sds.c, line 739.
(gdb) r 
Starting program: /data/working/github.com/redis-3.0-annotated/zhack_test/sds 

1 - sdslen(x) test: PASSED
2 - Create a string and obtain the length: PASSED
3 - Create a string with specified length: PASSED
4 - sdsMakeRoomFor() enlarge sds->buf size: PASSED
5 - sdscat() foo + bar: PASSED
6 - sdscpy() against an originally longer string: PASSED
7 - sdscpy() against an originally shorter string: PASSED

Breakpoint 1, main () at hello_sds.c:739
739	    int lenx = sdsll2str(x, 2016082217LL);
(gdb) step
sdsll2str (s=0x0, value=2016082217) at hello_sds.c:261
261	    ps = s;//需要使用 sdsMakeRoomFor 生成最少 SDS_LLSTR_SIZE 大小的空间 
(gdb) n
262	    rest = (value < 0) ? -value : value;
(gdb) list
257	    int  digit; //每次计算的个位数
258	    size_t len; //记录数字长度
259	    unsigned long long rest;//每次计算除去个位数的剩余数字
260	
261	    ps = s;//需要使用 sdsMakeRoomFor 生成最少 SDS_LLSTR_SIZE 大小的空间 
262	    rest = (value < 0) ? -value : value;
263	    while (rest)
264	    {
265	        //每次求个位
266	        digit = rest % 10;
(gdb) n
263	    while (rest)
(gdb) n
266	        digit = rest % 10;
(gdb) n
267	        rest = (rest - digit) / 10;
(gdb) n
268	        *ps++ = '0' + digit;//计算ascii 
(gdb) n

Program received signal SIGSEGV, Segmentation fault.
0x0000000000400da2 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
268	        *ps++ = '0' + digit;//计算ascii 
(gdb) 

```

可见，在这里遇到了 `Program received signal SIGSEGV, Segmentation fault.`

并且保留了上下文 `sdsll2str (s=0x0, value=2016082217)`

```
Program received signal SIGSEGV, Segmentation fault.
0x0000000000400da2 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
268	        *ps++ = '0' + digit;//计算ascii 

```

此刻注意到，hello_sds.c 的268行遇到了内存泄漏，这时候应该进一步查看下调用栈：
```
(gdb) backtrace 
#0  0x0000000000400e52 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
#1  0x0000000000402020 in main () at hello_sds.c:955

```

不难发现， 参数`s=0x0` ，这明显是有问题的，说明传入 `sdsll2str`之前就出问题了。
利用frame查看调用帧上下文：
```
(gdb) frame 0
#0  0x0000000000400e52 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
268	        *ps++ = '0' + digit;//计算ascii 
(gdb) frame 1
#1  0x0000000000402020 in main () at hello_sds.c:955
955	    int lenx = sdsll2str(x, llx);
(gdb) print x
$4 = (sds) 0x0
(gdb) list  
950	    //}}}
951	
952	    //{{{long long to string
953	    x = sdsMakeRoomFor(x,SDS_LLSTR_SIZE);
954	    long long llx =  2016082217LL;
955	    int lenx = sdsll2str(x, llx);
956	    test_cond_ext("sdsll2str() long long int to string",
957	        lenx == 10 && memcmp(x,"2016082217\0", 11) == 0);
958	    sdsfree(x);

```

顺藤摸瓜，果然 `x`是由`sdsMakeRoomFor()`产生，打个断点再来观察
```
//打断点
(gdb) break 953
Note: breakpoints 1 and 2 also set at pc 0x401ff0.
Breakpoint 3 at 0x401ff0: file hello_sds.c, line 953.
//重新run一次
(gdb) run
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /data/working/github.com/redis-tdd-annotation/zhack_test/sds 

1 - sdslen(x) test: PASSED
2 - Create a string and obtain the length: PASSED
3 - Create a string with specified length: PASSED
4 - sdsMakeRoomFor() enlarge sds->buf size: PASSED
5 - sdscat() foo + bar: PASSED
6 - sdscpy() against an originally longer string: PASSED
7 - sdscpy() against an originally shorter string: PASSED

Breakpoint 1, main () at hello_sds.c:953
953	    x = sdsMakeRoomFor(x,SDS_LLSTR_SIZE);
//打印上下文
(gdb) print x
$5 = (sds) 0x605458 "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk"
//单步进入
(gdb) step
sdsMakeRoomFor (s=0x605458 "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk", addlen=21) at hello_sds.c:864
864	    size_t free = sdsavail(s);
//此时可用看到，x正确传入，并来到判定语句
(gdb) next
sdsMakeRoomFor (s=0x605458 "xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk", addlen=21) at hello_sds.c:865
865	    if(free >= addlen) return NULL;
//到了这一步，先打印判定条件的值看下
(gdb) print free
$6 = 33
(gdb) print addlen
$7 = 21
//从上面的观察发现，此if语句为真，则返回NULL
//似乎明白了什么，下一步看下
(gdb) next
890	}
(gdb) next
main () at hello_sds.c:954
954	    long long llx =  2016082217LL;
(gdb) next
955	    int lenx = sdsll2str(x, llx);
(gdb) next

Program received signal SIGSEGV, Segmentation fault.
0x0000000000400e52 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
268	        *ps++ = '0' + digit;//计算ascii 
```

果然，是在这里把x指针搞废了:-P，好吧还好由gdb来帮忙。下面来看看代码的逻辑，发现原逻辑本意是 `free>=addlen`的时候返回s，而我在这里返回了NULL
对照一下作者最初的源代码，果然是我打错了，手误手误哎~，下面是源代码对比：

```
//sdsMakeRoomFor
sds sdsMakeRoomFor(sds s,sizeof addlen)
{
...
    //0 初始化 
    size_t free = sdsavail(s);

    if(free >= addlen) return NULL;
...
}

//main
int main()
{
...
    // sds拷贝大字符串
    x = sdsempty();
    x = sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    test_cond_ext("sdscpy() against an originally shorter string",
        sdslen(x) == 33 &&
        memcmp(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0);

    //}}}

    //{{{long long to string
    x = sdsMakeRoomFor(x,SDS_LLSTR_SIZE);
    long long llx =  2016082217LL;
    int lenx = sdsll2str(x, llx);
...
}
```

通过观察得知，x必然每次都为 `0x0`，因为`sdslen(x)=33`,而`SDS_LLSTR_SIZE=21`，所以每次都会返回NULL。
这里是一个逻辑bug，将`NULL`返回修改为返回 `s` 即可。当`free>=addlen`的时候无需扩容，返回源sds符串本身即可。

而为了代码的严谨性，对测试代码也做了修改，每次使用完毕之后，都需要清理空间，每次使用之前都需要初始化：

```
//sdsMakeRoomFor
sds sdsMakeRoomFor(sds s,sizeof addlen)
{
...
    //0 初始化 
    size_t free = sdsavail(s);

    if(free >= addlen) return s;//返回s本身
...
}

//main
int main()
{
...
    // sds拷贝大字符串
    x = sdsempty();
    x = sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    test_cond_ext("sdscpy() against an originally shorter string",
        sdslen(x) == 33 &&
        memcmp(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0);
    sdsfree(x); //释放空间

    //}}}

    //{{{long long to string
    x = sdsMakeRoomFor(sdsempty(),SDS_LLSTR_SIZE);//注意sdsempty()
    long long llx =  2016082217LL;
    int lenx = sdsll2str(x, llx);
    test_cond_ext("sdsll2str() long long int to string",
        lenx == 10 && memcmp(x,"2016082217\0", 11) == 0);
    sdsfree(x);
...
}

```

至此，一次完整的gdb发现bug和修复的过程就此收尾，是不是很过瘾？:P

全文完，谢谢观赏！欢迎各位大神批评指正~

## 附录 gcc+valgrind build的 过程
[gcc_valgrind.png](gcc_valgrind.png)