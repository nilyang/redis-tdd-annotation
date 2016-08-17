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

此刻注意到，hello_sds.c 的268行遇到了内存泄漏，给改行的上一行打断点观察指针`ps`和`s`的值
```
(gdb) break 267
```


```
Program received signal SIGSEGV, Segmentation fault.
0x0000000000400da2 in sdsll2str (s=0x0, value=2016082217) at hello_sds.c:268
268	        *ps++ = '0' + digit;//计算ascii 

```