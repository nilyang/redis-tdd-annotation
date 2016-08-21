#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<assert.h>

#include"hello_sds.h"

sds sdsnewlen(const void *init, size_t initlen)
{
    struct sdshdr *sh;

    if (init) {
        //zmalloc 不初始化内存
        sh = (struct sdshdr *)zmalloc(sizeof(struct sdshdr) + initlen + 1);
    } else {
        //zcalloc 将分配的内存全部初始化为0
        sh = (struct sdshdr *)zcalloc(sizeof(struct sdshdr) + initlen + 1);
    }

    // printf("initlen=%zu\n",initlen);

    //内存分配失败NULL
    if (sh==NULL) return NULL;

    // 设置初始化长度
    sh->len = initlen;
    
    // 新sds不预留任何空间
    sh->free = 0;
    
    //如果有指定初始化内容，将他们复制到sdshdr 的buf中
    if(initlen && init){
        memcpy(sh->buf, init, initlen);
    }

    // 以 ASCII 0  结尾
    sh->buf[initlen] = '\0';

    // 返回buf部分，而不是整个sdshdr结构
    return (char *)sh->buf;
}

sds sdsnew(const char *init)
{
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    // printf("init=%s , initlen=%zu\n",init, initlen);
    return sdsnewlen(init, initlen);
}

sds sdsempty(void)
{
    return sdsnewlen("",0);
}


// size_t sdslen(const sds s){
//     struct sdshdr *sh = (void *)(s - sizeof(struct sdshdr));
//     return sh->len;
// }

// size_t sdsavail(const sds s)
// {
//     struct sdshdr *sh = (void *)(s - sizeof(struct sdshdr));
//     return sh->free;
// }

sds sdsdup(const sds s)
{
    return sdsnewlen(s,strlen(s));
}

void sdsfree(sds s)
{
    if(s == NULL) return;
    zfree(s - sizeof(struct sdshdr));
}

//貌似废弃了
sds sdsgrowzero(sds s, size_t len)
{
    return (char *)"";
}

//将长度为 len 的字符串 t 追加到 sds 字符串末尾
//T=O(N)
sds sdscatlen_ext(sds s, const void *t, size_t len)
{
    //1.计算旧字符串长度
    //2.新字符串长度
    //3.memcpy(old,new,cpylen)
    struct sdshdr *sh;

    size_t oldlen = sdslen(s);
    
    if(t==NULL)return NULL;
    
    //sdshdr元素值调整
    sh = (struct sdshdr *)(s - (sizeof(struct sdshdr)));
    sh->len = oldlen + len;
    sh->free = sh->len;

    //字符串拼接
    strcat(s, t);

    return sh->buf;
}

/**
 * 拼接sds字符串
 * 注意：完成调用之后，被修改的s将失效，新的字符串应该使用该函数的返回值
 */
sds sdscatlen(sds s, const void *t, size_t len)
{
    struct sdshdr* sh;
    // 原有字符串长度
    size_t curlen = sdslen(s);

    // 扩展原buf字段
    s = sdsMakeRoomFor(s, len);
    // 内存分配成功与否判定
    if(s == NULL) return NULL;
    
    // 字符串相加
    sh = (struct sdshdr *)(s - (sizeof(struct sdshdr)));
    //T=O(N)

    // 手工拼接
    // int i;
    // const char* tt = t;
    // for(i=0;i < len; i++){
    //     sh->buf[curlen+i] = tt[i];
    // }

    //memcpy 安全拼接
    memcpy(s+curlen, t, len);

    //更新字段
    sh->len = curlen + len;
    sh->free = sh->free - len;
    //结尾添加结束符'\0'
    sh->buf[sh->len] = '\0';

    return sh->buf;
}

/*
 * 拼接字符串 t 到 sds 末尾 
 * 注意：完成调用之后，被修改的s将失效，新的字符串应该使用该函数的返回值
 *
 */
sds sdscat(sds s, const char * t)
{
    return sdscatlen(s,t,strlen(t));
}
// 在不释放
void sdsclear(sds s)
{
    struct sdshdr *sh = (struct sdshdr *)(s - (sizeof(struct sdshdr)));

    sh->free += sh->len;
    sh->len = 0;

    //将结束字符放到最前面（相当于惰性地删除）
    sh->buf[0] = '\0';
}

/*
 * 将另一个 sds 追加到一个 sds 的末尾
 * 注意：完成调用之后，被修改的s将失效，新的字符串应该使用该函数的返回值
 * 
 * 返回值
 *   sds ： 追加成功返回新 sds， 失败返回 NULL
 *
 * 复杂度
 *  T = O(N)
 */
sds sdscatsds(sds s, const sds t)
{
    return sdscatlen(s,t, sdslen(t));
}

/**
 * 将指针t指向的字符串，复制len个字节到s中 
 * 如果sds的长度少于len个字符，那么扩展sds
 *
 * 复杂度 T=O(N)
 * 返回 
 *  sds : 复制成功返回新的sds，否则返回NULL
 *
 * Destructively modify the sds string 's' to hold the specified binary
 * safe string pointed by 't' of length 'len' bytes. 
 */
sds sdscpylen(sds s, const char *t, size_t len)
{
    struct sdshdr *sh = (struct sdshdr*)(s-(sizeof(struct sdshdr)));
    
    // 现有buf总长度
    size_t buflen = sh->free + sh->len;
    if(buflen < len){
        // 扩展buf空间
        s = sdsMakeRoomFor(s, len - sh->len);
        if(s == NULL) return NULL;
        sh = (void*)(s-(sizeof(struct sdshdr)));
        buflen = sh->free + sh->len;
    }

    // copy buf contents
    // T = O(N)
    memcpy(s, t, len);
    
    // 添加终结符
    s[len] = '\0';
    
    // 更新字段
    sh->len = len;
    sh->free = buflen - len;

    return sh->buf;
}

/**
 * 将字符串复制到 sds 当中，覆盖原有字符串。
 * 
 * 如果 sds 的长度少于字符串的长度，则扩展sds
 *
 * 复杂度 T=O(N)
 * 返回值
 *  sds ： 复制成功返回新的 sds ， 否则返回 NULL
 */
sds sdscpy(sds s, const char *t)
{
    return sdscpylen(s, t, strlen(t));
}

//{{{-------------------------------helper functions-------------------------//


/* Helper for sdscatlonglong() doing the actual number -> string
 * conversion. 's' must point to a string with room for at least
 * SDS_LLSTR_SIZE bytes.
 *
 * The function returns the lenght of the null-terminated string
 * representation stored at 's'. */
//无符号长长整数，最大长度 ULLONG_MAX 转字符串最大20+1 (最后1位数字字符串末尾'\0')
#define SDS_LLSTR_SIZE 21
int sdsll2str(char *s, long long value)
{
    //注意:
    //   形参的值修改，并不影响实参值，即便是指针也要相同对待（指针变量在这里只是局部变量，不影响实参指针变量）
    //   如果要修改指针变量，这里需要传入二重指针 sdsll2str(char **pps, long long value) :-)
    
    //1. 长整形转字符串，返回数字串的长度
    //2. 最大长度 
    //3.算法，按数字进制位，挨个转为ascii
    char  *ps;  //字符串数组，保存value的每一位数字
    int  digit; //每次计算的个位数
    size_t len; //记录数字长度
    unsigned long long rest;//每次计算除去个位数的剩余数字

    ps = s;//需要使用 sdsMakeRoomFor 生成最少 SDS_LLSTR_SIZE 大小的空间 
    rest = (value < 0) ? -value : value;
    while (rest)
    {
        //每次求个位
        digit = rest % 10;
        rest = (rest - digit) / 10;
        *ps++ = '0' + digit;//计算ascii 
    }
    if (value < 0) *ps++ = '-';

    len = ps - s;
    //末尾
    *ps = '\0';

    //反向输出 reverse，得到数字的sds串
    char tmp;
    --ps;//移动到'\0'前
    while (s < ps) {
        tmp = *s;
        *s = *ps;
        *ps = tmp;
        ps--;
        s++;
    }

    return len;
}

/* 参考 sdsll2str(): Identical sdsll2str(), but for unsigned long long type. */
int sdsull2str(char *s, unsigned long long v) 
{
    char  *ps;  
    int  digit; 
    size_t len; 

    ps = s;
    while (v)
    {
        //每次求个位
        digit = v % 10;
        v = (v - digit) / 10;
        *ps++ = '0' + digit;//计算ascii 
    }

    len = ps - s;
    //末尾
    *ps = '\0';

    //反向输出 reverse，得到数字的sds串
    char tmp;
    --ps;//移动到'\0'前
    while (s < ps) {
        tmp = *s;
        *s = *ps;
        *ps = tmp;
        ps--;
        s++;
    }

    return len;}


/* Create an sds string from a long long value. It is much faster than:
 *
 * sdscatprintf(sdsempty(),"%lld\n", value);
 */
sds sdsfromlonglong(long long value) 
{
    char buf[SDS_LLSTR_SIZE];
    int len = sdsll2str(buf,value);
    return sdsnewlen(buf,len);
}

/* 
 * sds无限制参数格式化打印字符串函数，并将格式化后的字符串拼接到sds之后
 * 注意：被 sdscatprintf() 所调用， 本函数的 va_list  配合调用者的  varandic 以实现可变参数功能
 * 复杂度： T=O(N^2)
 * 使用到 vsnprintf : http://china.qnx.com/developers/docs/6.4.1/neutrino/lib_ref/v/vsnprintf.html
 * Like sdscatprintf() but gets va_list instead of being variadic. 
 */
sds sdscatvprintf(sds s, const char *fmt, va_list ap) 
{
    va_list cpy;
    char staticbuf[1024], *buf = staticbuf, *t;
    size_t buflen = strlen(fmt)*2;

    // 尝试使用静态缓冲来加速，如果不行则反过来使用堆分配空间
    // We try to start using a static buffer for speed.
    // If not possible we revert to heap allocation. 
    if(buflen>sizeof(staticbuf)){
        buf = zmalloc(buflen);
        if(buf == NULL) return NULL;
    }else{
        buflen = sizeof(staticbuf);
    }
    
    // 每次vsnsprintf失败之后，重置buf，并尝试将buf扩大2倍
    // （行为类似 sdsMakeRoomFor ）以适应当前buf大小
    // Try with buffers two times bigger every time we fail to
    // fit the string in the current buffer size.
    while(1){
        buf[buflen-2] = '\0';//TODO 为啥呢?
        va_copy(cpy, ap);//c99,c++11
        //T=O(N)
        vsnprintf(buf, buflen, fmt, cpy);
        va_end(cpy);//注意：huangz1990注解版漏了这句，估计是他注解到时候手抖...

        // 如果失败基本上是cpy参数超出了buf长度
        // （如果buf和起初位置不等，则清空buf）然后重新分配空间，重试参数格式化(这里使用了while循环来实现)
        if(buf[buflen-2]!='\0'){
            if(buf!= staticbuf)zfree(buf);
            buflen *= 2;
            buf = zmalloc(buflen);
            if(buf == NULL) return NULL;
            continue;
        }
        break;
    }

    //Finally concat the obtained string to the SDS string and return it.
    t = sdscat(s, buf);
    if (buf != staticbuf) zfree(buf);

    return t;
}


/* 
 * 将 printf-alike 格式的字符串，拼接到sds字符之后 
 *
 * Append to the sds string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsempty("Sum is: ");
 * s = sdscatprintf(s,"%d+%d = %d",a,b,a+b).
 *
 * Often you need to create a string from scratch with the printf-alike
 * format. When this is the need, just use sdsempty() as the target string:
 *
 * s = sdscatprintf(sdsempty(), "... your format ...", args);
 */
sds sdscatprintf(sds s, const char *fmt, ...) 
{
    va_list ap;
    char *t;
    va_start(ap, fmt);
    t = sdscatvprintf(s, fmt, ap);
    va_end(ap);
    return t;
}


/**
 * 类似 sdscatprintf()  ，不使用 vsnprintf 来实现格式化参数解析
 *
 * This function is similar to sdscatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the sds string as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %s - C String
 * %S - SDS string
 * %i - signed int
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
sds sdscatfmt(sds s, char const *fmt, ...)
{
    struct sdshdr *sh = (struct sdshdr *)(s - (sizeof(struct sdshdr)));
    size_t initlen = sdslen(s);
    const char *f = fmt;
    int index;
    va_list ap;// 定义可变参数列表(define argument list variable)

    va_start(ap, fmt);//初始化可变参数列表，指向最后一个定义的参数(init list; point to last defined argument)
    f = fmt;          // Next format specifier byte to process. 
    index = initlen;  // Position of the next byte to write to dest str.
                 //循环捕获变参fmt的每一个字节，决定格式样式
    while (*f) {
        char next, *str;//下一个fmt字符，及
        size_t valen;   //待格式化参数长度
        long long num;  //带符号整数
        unsigned long long unum;//无符号整数

        // Make sure there is always space for at least 1 char.
        // 确保 s 至少具有1个字节空间可用
        if (sh->free == 0) {
            s = sdsMakeRoomFor(s, 1);
            sh = (struct sdshdr*)(s - sizeof(struct sdshdr));
        }

        switch (*f) {

        //遇到格式化字符开头"%"符号，开始执行格式化
        case '%':
            // next = *(f+1);//指针后移1位，指向格式类型字符
            // f++;          //同上 ，其实可以合并为 next = *(++f)
            next = *(++f);   //合并上面两行代码
                             //格式化
            switch (next) {
            case 's':
            case 'S':
                // %s - C String
                // %S - SDS string
                str = va_arg(ap, char *);//获取下一个参数(即...依次对应的参数)
                valen = (next == 's') ? strlen(str) : sdslen(str);
                if (sh->free<valen) {
                    //free空间不足则重新分配valen个字节
                    s = sdsMakeRoomFor(s, valen);
                    //分配之后，重新计算sdshdr空间大小和指针首地址
                    sh = (struct sdshdr*)(s - sizeof(struct sdshdr));
                }
                memcpy(s + index, str, valen);
                sh->len += valen;
                sh->free -= valen;
                index += valen; //索引移动到fmt下一个字符                
                break;
                /*
                memcpy拼接示意图
                i = sdslen(s)

                1. sdsMakeRoomFor 之前
                +--------+--------+
                |        |  free  |
                +--------+--------+
                s        s+index

                2.sdsMakeRoomFor 之后

                条件： sh->free < valen ,则扩大 s 空间，分配s以不小于 (index+valen)*2的空间
                memcpy操作从 s+index 位置开始，将当前可变参数str拷贝valen个字节到该位置
                注意： 目的是使用剩余空间将str拼接到s上

                +--------+--------+--...--+
                |        |        |  free |
                +--------+--------+--...--+
                s        |        |
                         |        *--->(index+valen) 
                         |
                         *--->index
                             
                sh->len += valen;
                sh->free -= valen;
                index += valen;
                */

            case 'i':
            case 'I':
                //带符号数字转sds处理
                // %i - signed int
                // %I - 64 bit signed integer (long long, int64_t)
                if (next == 'i')
                    num = va_arg(ap, int);
                else
                    num = va_arg(ap, long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    valen = sdsll2str(buf, num);
                    if (sh->free < valen) {
                        s = sdsMakeRoomFor(s, valen);
                        sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
                    }
                    memcpy(s + index, buf, valen);
                    sh->len += valen;
                    sh->free -= valen;
                    index += valen;//标记增长到位置
                }
                break;
            case 'u':
            case 'U':
                //无符号数字转sds处理
                // %u - unsigned int
                // %U - 64 bit unsigned integer (unsigned long long, uint64_t)
                if (next == 'u')
                    unum = va_arg(ap, unsigned int);
                else
                    unum = va_arg(ap, unsigned long long);
                {
                    char buf[SDS_LLSTR_SIZE];
                    valen = sdsull2str(buf, unum);
                    if (sh->free < valen) {
                        s = sdsMakeRoomFor(s, valen);
                        sh = (struct sdshdr *)(s - sizeof(struct sdshdr));
                    }
                    memcpy(s + index, buf, valen);
                    sh->len += valen;
                    sh->free -= valen;
                    index += valen;
                }
                break;
            case '%':
            default:
                /* Handle %% and generally %<unknown>. */
                // %% - Verbatim "%" character.
                s[index++] = next;
                sh->len += 1;
                sh->free -= 1;
                break;
            }
            break;

        //普通字符，直接拼接到s末尾
        default:
            s[index++] = *f; //照应循环开头的if(sh->free==0)的判定，每次只需保证容纳下1个字符
            sh->len += 1;
            sh->free -= 1;
            break;
        }
        f++;
    }

    va_end(ap);//清空可变参数列表

               /* Add null-term */
    s[index] = '\0';//惯例末尾加结束符

    return s;
}


/* 
 * 不区分大小写 trim
 * 
 * 自己实现的复杂度： T=O(N*M) N=sds length,M =cset length 
 * 官方使用了strchr，复杂度T=O(N*M) 差不多，性能没有对比过
 * Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
 * s = sdstrim(s,"A. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
sds sdstrim(sds s, const char *cset) 
{
    //思想，删除左侧，如果左侧删除完毕，则将剩余字符串整体左移
    struct sdshdr *ps = (struct sdshdr*)(s - sizeof(struct sdshdr));
    size_t len = sdslen(s);
    char *pstart = s,         //pstart 指向s初始字符
         *pend = (s + len -1),//pend 指向s末尾字符 不能从'\0'往回走，要提前一位
         *pcset;

    //修建算法的官方实现方式比较巧妙，主要利用了strchr函数
    //下面是我的实现

    //循环剔除左边
    int find=0;
    while(*pstart!='\0'){
        pcset = (char *)cset;
        while(*pcset!='\0'){
            if(strncasecmp(pstart,pcset,1) == 0){
                pstart += 1;//右移1位，继续比较
                find = 1;
                break;
            }
            find = 0;
            pcset +=1;
        }

        if(find == 0){
            break;//trim left 结束
        }
    }

    //循环剔除右边
    find = 0;
    while(pend!=pstart){
        pcset = (char*)cset;
        while(*pcset!='\0'){
            if(strncasecmp(pend,pcset,1) == 0){
                pend -= 1;//右移1位，继续比较
                find = 1;
                break;
            }
            find = 0;
            pcset +=1;
        }

        if(find == 0){
            break;//trim left 结束
        }
    }

    //计算最后的长度
    len = pend > pstart? (pend - pstart + 1):0;
    //左移
    if(pstart != s)memmove(s, pstart, len);
    //末尾终结符
    s[len]='\0';
    //更新字段
    ps->free = ps->free + (ps->len - len);
    ps->len = len;

    return s;
}

/* Turn the string into a smaller (or equal) string containing only the
 * substring specified by the 'start' and 'end' indexes.
 *
 * start and end can be negative, where -1 means the last character of the
 * string, -2 the penultimate character, and so forth.
 *
 * The interval is inclusive, so the start and end characters will be part
 * of the resulting string.
 *
 * The string is modified in-place.
 *
 * Example:
 *
 * s = sdsnew("Hello World");
 * sdsrange(s,1,-1); => "ello World"
 */
void sdsrange(sds s, int start, int end) 
{
    // *ps可以优化到最后初始化
    // struct sdshdr  *ps = (struct sdshdr*)(s-sizeof(struct sdshdr));
    size_t newlen = 0,
           len = sdslen(s);

    if(len==0)return;
    //负数转换为正数
    if(start<0){
        start = len + start;
        if(start<0) start = 0;
    }

    if(end<0){
        end = len + end;
        if(end<0) end = 0;
    }

    newlen = (start > end) ? 0 : (end - start + 1);
    if(newlen != 0){
        // 依次处理 start/end 超出范围的情况
        if(start >= (signed)len){
            newlen = 0;
        }else if(end >= (signed)len){
            end = len - 1;
            newlen = (start > end)?0:(end - start + 1);
        }
    }else{
        start = 0;
    }

    //如果需要移动则操作之
    if(start&&newlen) memmove(s,s+start,newlen);
    
    //设置字符串结束符
    s[newlen] = '\0';
    //更新属性
    struct sdshdr  *ps = (struct sdshdr*)(s-sizeof(struct sdshdr));
    
    ps->free = ps->free + (len - newlen);
    ps->len = newlen; 
}

/* Apply tolower() to every character of the sds string 's'. */
void sdstolower(sds s) 
{

}

/* Apply toupper() to every character of the sds string 's'. */
void sdstoupper(sds s) 
{

}


/* Compare two sds strings s1 and s2 with memcmp().
 *
 * Return value:
 *
 *     1 if s1 > s2.
 *    -1 if s1 < s2.
 *     0 if s1 and s2 are exactly the same binary string.
 *
 * If two strings share exactly the same prefix, but one of the two has
 * additional characters, the longer string is considered to be greater than
 * the smaller one. */
int sdscmp(const sds s1, const sds s2) 
{
return 0;
}

/* Split 's' with separator in 'sep'. An array
 * of sds strings is returned. *count will be set
 * by reference to the number of tokens returned.
 *
 * 使用分隔符 sep 对 s 进行分割，返回一个 sds 字符串的数组。
 * *count 会被设置为返回数组元素的数量。
 *
 * On out of memory, zero length string, zero length
 * separator, NULL is returned.
 *
 * 如果出现内存不足、字符串长度为 0 或分隔符长度为 0
 * 的情况，返回 NULL
 *
 * Note that 'sep' is able to split a string using
 * a multi-character separator. For example
 * sdssplit("foo_-_bar","_-_"); will return two
 * elements "foo" and "bar".
 *
 * 注意分隔符可以的是包含多个字符的字符串
 *
 * This version of the function is binary-safe but
 * requires length arguments. sdssplit() is just the
 * same function but for zero-terminated strings.
 *
 * 这个函数接受 len 参数，因此它是二进制安全的。
 * （文档中提到的 sdssplit() 已废弃）
 *
 * T = O(N^2)
 */
sds *sdssplitlen(const char *s, int len, const char *sep, int seplen, int *count) 
{
return (sds *)"";
}

/* Free the result returned by sdssplitlen(), or do nothing if 'tokens' is NULL. */
void sdsfreesplitres(sds *tokens, int count) 
{

}

/* Append to the sds string "s" an escaped string representation where
 * all the non-printable characters (tested with isprint()) are turned into
 * escapes in the form "\n\r\a...." or "\x<hex-number>".
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call. */
sds sdscatrepr(sds s, const char *p, size_t len) 
{
return (sds)"";
}

/**
 * Helper function for sdssplitargs() that returns non zero if 'c'
 * is a valid hex digit. 
 */
int is_hex_digit(char c) 
{
return 0;
}


/**
 * Helper function for sdssplitargs() that converts a hex digit into an
 * integer from 0 to 15 
 */
int hex_digit_to_int(char c) 
{
return 0;
}



/* Split a line into arguments, where every argument can be in the
 * following programming-language REPL-alike form:
 *
 * 将一行文本分割成多个参数，每个参数可以有以下的类编程语言 REPL 格式：
 *
 * foo bar "newline are supported\n" and "\xff\x00otherstuff"
 *
 * The number of arguments is stored into *argc, and an array
 * of sds is returned.
 *
 * 参数的个数会保存在 *argc 中，函数返回一个 sds 数组。
 *
 * The caller should free the resulting array of sds strings with
 * sdsfreesplitres().
 *
 * 调用者应该使用 sdsfreesplitres() 来释放函数返回的 sds 数组。
 *
 * Note that sdscatrepr() is able to convert back a string into
 * a quoted string in the same format sdssplitargs() is able to parse.
 *
 * sdscatrepr() 可以将一个字符串转换为一个带引号（quoted）的字符串，
 * 这个带引号的字符串可以被 sdssplitargs() 分析。
 *
 * The function returns the allocated tokens on success, even when the
 * input string is empty, or NULL if the input contains unbalanced
 * quotes or closed quotes followed by non space characters
 * as in: "foo"bar or "foo'
 *
 * 即使输入出现空字符串， NULL ，或者输入带有未对应的括号，
 * 函数都会将已成功处理的字符串先返回。
 *
 * 这个函数主要用于 config.c 中对配置文件进行分析。
 * 例子：
 *  sds *arr = sdssplitargs("timeout 10086\r\nport 123321\r\n");
 * 会得出
 *  arr[0] = "timeout"
 *  arr[1] = "10086"
 *  arr[2] = "port"
 *  arr[3] = "123321"
 *
 * T = O(N^2)
 */
sds *sdssplitargs(const char *line, int *argc) 
{
return (sds *)"";
}


/* Modify the string substituting all the occurrences of the set of
 * characters specified in the 'from' string to the corresponding character
 * in the 'to' array.
 *
 * 将字符串 s 中，
 * 所有在 from 中出现的字符，替换成 to 中的字符
 *
 * For instance: sdsmapchars(mystring, "ho", "01", 2)
 * will have the effect of turning the string "hello" into "0ell1".
 *
 * 比如调用 sdsmapchars(mystring, "ho", "01", 2)
 * 就会将 "hello" 转换为 "0ell1"
 *
 * The function returns the sds string pointer, that is always the same
 * as the input pointer since no resize is needed. 
 * 因为无须对 sds 进行大小调整，
 * 所以返回的 sds 输入的 sds 一样
 *
 * T = O(N^2)
 */
sds sdsmapchars(sds s, const char *from, const char *to, size_t setlen) 
{
return (sds)"";
}


/* Join an array of C strings using the specified separator (also a C string).
 * Returns the result as an sds string. */
sds sdsjoin(char **argv, int argc, char *sep)
{
return (sds)"";
}


//}}}-------------------------------helper functions-------------------------//

/**
 * 对 sds 中 buf 的长度进行扩展，确保在函数执行之后，buf至少会有 addlen + 1 长度的空余空间
 * （额外那 1 个字节是存放 \0 的）
 *  
 * 返回值
 *  sds: 扩展成功返回扩展后的 sds 
 *       扩展失败，返回 NULL
 *  复杂度
 *  T = O(N)
 */
sds sdsMakeRoomFor_My(sds s, size_t addlen)
{
    //1. 原来的数据
    size_t cur_free = sdsavail(s);
    struct sdshdr* sh = (void *)(s - (sizeof(struct sdshdr)));
    
    //2. 扩展空闲空间(+1是为了结束符\0)
    size_t new_free = cur_free + addlen;
    char * pbuf = zrealloc(s, new_free + 1);
    
    // 分配失败
    if(!pbuf) return NULL;
    // 分配成功
    // if(pbuf != s);
    
    sh = (void *)(pbuf - (sizeof(struct sdshdr)));
    sh->free = new_free;

    return sh->buf;
}

/**
 * 对 sds 中 buf 的长度进行扩展，确保在函数执行之后，
 * buf 至少会有 addlen + 1 长度的空余空间
 * （额外的 1 字节是为 \0 准备的）
 * 复杂度
 *  T = O(N)
 */
sds sdsMakeRoomFor(sds s, size_t addlen)
{
    //1. 获取 s 已占空间长度
    //2. 如果 空闲空间足够，则直接返回 s，否则下一步扩容
    //3. 计算需要扩容的空间大小
    //4. zrealloc 分配扩容空间，成功则更新sds->free字段，否则返回NULL
    //5. 结束

    //0 初始化 
    size_t free = sdsavail(s);
    if(free >= addlen) return NULL;//bugfix 不应该返回NULL

    size_t len,newlen;
    struct sdshdr *sh=NULL, *newsh=NULL;

    //1
    len = sdslen(s);
    sh = (struct sdshdr*)(s - (sizeof(struct sdshdr)));
    
    //扩容最少需要的长度
    newlen = (len + addlen);

    //根据新长度，为 s 分配新空间所需大小
    //i) 扩容后空间小于SDS_MAX_PREALLOC，则分配两倍于所需长度（newlen）的空间
    //ii) 扩容后空间若大于SDS_MAX_PREALLOC，则分配newlen+SDS_MAX_PREALLOC的空间
    if (newlen < SDS_MAX_PREALLOC)
        newlen *= 2;
    else
        newlen += SDS_MAX_PREALLOC; 

    // 值得注意的是，计算sdshdr结构长度是因为，结构体本身带的动态数组buf不占空间
    newsh = zrealloc(sh, sizeof(struct sdshdr) + newlen + 1);
    // 更新free
    newsh->free = newlen - len;
    return newsh->buf;
}


#ifdef SDS_TEST_MAIN
#include<stdio.h>
#include<limits.h>
#include"testhelp_ext.h"

int main()
{
    printf("\n");
    // struct sdshdr * sh;
    // sds y;
    sds x = sdsnew("foo");
    // printf("%zu\n",sdslen(x));
    test_cond_ext("sdslen(x) test", sdslen(x) == 3);

    int ret = memcmp(x,"foo\0",4);
    // printf("%s\n",sdsbuf(x));
    test_cond_ext("Create a string and obtain the length",
        sdslen(x) == 3 && ret == 0);

    // 测试生成指定长度sds
    sdsfree(x);
    x = sdsnewlen("foo",2);
    test_cond_ext("Create a string with specified length",
        sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0);
    

    // sds字符串拼接
    sdsfree(x);
    x = sdsnew("foo");
    // 扩展buf字段
    x = sdsMakeRoomFor(x, 10);
    test_cond_ext("sdsMakeRoomFor() enlarge sds->buf size",
        sdslen(x) == 3 && memcmp(x, "foo\0", 4) == 0);

    sdsfree(x);
    x = sdsnew("foo");
    x = sdscat(x, "bar");
    test_cond_ext("sdscat() foo + bar",
        sdslen(x) == 6 && memcmp(x,"foobar\0", 6) == 0);

    sdsfree(x);

    //{{{ sds拷贝
    x = sdsnew("foo");
    x = sdscpy(x,"a");
    test_cond_ext("sdscpy() against an originally longer string",
        sdslen(x) == 1 && memcmp(x,"a\0",2) == 0);
    sdsfree(x);
    
    // sds拷贝大字符串
    x = sdsempty();
    x = sdscpy(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk");
    test_cond_ext("sdscpy() against an originally shorter string",
        sdslen(x) == 33 &&
        memcmp(x,"xyzxxxxxxxxxxyyyyyyyyyykkkkkkkkkk\0",33) == 0);
    sdsfree(x);

    //}}}

    //{{{long long to string
    x = sdsMakeRoomFor(sdsempty(),SDS_LLSTR_SIZE);
    long long llx =  2016082217LL;
    int lenx = sdsll2str(x, llx);
    test_cond_ext("sdsll2str() long long int to string",
        lenx == 10 && memcmp(x,"2016082217\0", 11) == 0);
    sdsfree(x);

    // 负数
    x = sdsMakeRoomFor(sdsempty(),SDS_LLSTR_SIZE);
    llx = -922337203685477580LL;
    lenx = sdsll2str(x,llx);
    test_cond_ext("sdsll2str() negative long long int to string",
        lenx == 19 && memcmp(x,"-922337203685477580\0",20) == 0);
    sdsfree(x); 

    // 无符号正数
    x = sdsMakeRoomFor(sdsempty(),SDS_LLSTR_SIZE);
    unsigned long long ullx = 18446744073709551615ULL;
    lenx = sdsull2str(x,ullx);
    test_cond_ext("sdsull2str() unsigned long long int to string ",
        lenx == 20 && memcmp(x, "18446744073709551615\0", 21) == 0 );
    sdsfree(x); 

    // sdsfromlonglong test
    llx = 9223372036854775807LL;
    x = sdsfromlonglong(llx);
    test_cond_ext("sdsfromlonglong() long long to sds",
        sdslen(x) == 19 && memcmp(x,"9223372036854775807\0", 20) == 0);  
    sdsfree(x); 
    //}}}

    //{{{
    //sdscatprintf
    x = sdscatprintf(sdsempty(),"%d",123);
    test_cond_ext("sdscatprintf() seems working in the base case",
        sdslen(x) == 3 && memcmp(x,"123\0",4) == 0)
    sdsfree(x);
    
    x = sdscatprintf(sdsnew("I'am a very large number string ,yes "),"unsigned long long int = %llu",ullx);
    test_cond_ext("sdscatprintf() sds + (unsigned long long int)",
        sdslen(x) == 82 && memcmp(x,"I'am a very large number string ,yes unsigned long long int = 18446744073709551615\0",83) == 0);
    // printf("%s\n",x);
    sdsfree(x);
    //}}}

    //{{{ sdscatfmt test
    //1. sdscatfmt signed int/long
    x = sdsnew("--");
    x = sdscatfmt(x, "Hello %s World %I,%I--", "Hi!", LLONG_MIN,LLONG_MAX);
    test_cond_ext("sdscatfmt() seems working in the base case",
        sdslen(x) == 60 &&
        memcmp(x,"--Hello Hi! World -9223372036854775808,"
                    "9223372036854775807--",60) == 0)
    sdsfree(x);

    //2.sdscatfmt unsigned int/long long
    x = sdsnew("--");
    x = sdscatfmt(x, "%u,%U--", UINT_MAX, ULLONG_MAX);
    test_cond_ext("sdscatfmt() seems working with unsigned numbers",
        sdslen(x) == 35 &&
        memcmp(x,"--4294967295,18446744073709551615--",35) == 0)
    sdsfree(x);
    //}}}

    //{{{
    x = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
    x = sdstrim(x,"A. :");
    //HelloWorld
    test_cond_ext("sdstrim() correctly trims characters",
        sdslen(x) == 10 && memcmp(x,"HelloWorld\0",11) == 0)

    sdsfree(x);
    //}}}


    //{{{ sdsrange test
    x = sdsnew("ciao");
    sdsrange(x,1,1);
    test_cond_ext("sdsrange(...,1,1)",
        sdslen(x) == 1 && memcmp(x,"i\0",2) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,1,-1);
    test_cond_ext("sdsrange(...,1,-1)",
        sdslen(x) == 3 && memcmp(x,"iao\0",4) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,-2,-1);
    test_cond_ext("sdsrange(...,-2,-1)",
        sdslen(x) == 2 && memcmp(x,"ao\0",3) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,-1,-1);
    test_cond_ext("sdsrange(...,-1,-1)",
        sdslen(x) == 1 && memcmp(x,"o\0",2) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,-1,-2);
    test_cond_ext("sdsrange(...,-1,-2)",
        sdslen(x) == 0 && memcmp(x,"\0",1) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,-1,-100);
    test_cond_ext("sdsrange(...,1,-100)",
        sdslen(x) == 0 && memcmp(x,"\0",1) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,2,1);
    test_cond_ext("sdsrange(...,2,1)",
        sdslen(x) == 0 && memcmp(x,"\0",1) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,1,100);
    test_cond_ext("sdsrange(...,1,100)",
        sdslen(x) == 3 && memcmp(x,"iao\0",4) == 0)
    sdsfree(x);

    x = sdsnew("ciao");
    sdsrange(x,100,100);
    test_cond_ext("sdsrange(...,100,100)",
        sdslen(x) == 0 && memcmp(x,"\0",1) == 0)
    sdsfree(x);
    //}}}
    test_report_ext();
    return 0;
}
#endif