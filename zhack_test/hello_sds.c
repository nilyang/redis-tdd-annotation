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
        sh = zmalloc(sizeof(struct sdshdr) + initlen + 1);
    } else {
        //zcalloc 将分配的内存全部初始化为0
        sh = zcalloc(sizeof(struct sdshdr) + initlen + 1);
    }

    printf("initlen=%zu\n",initlen);

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
    printf("init=%s , initlen=%zu\n",init, initlen);
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

sds sdscatlen(sds s, const void *t, size_t len)
{
    return (char *)"";
}

// 在不释放
void sdsclear(sds s)
{
    struct sdshdr *sh = (void *)(s - (sizeof(struct sdshdr)));

    sh->free += sh->len;
    sh->len = 0;

    //将结束字符放到最前面（相当于惰性地删除）
    sh->buf[0] = '\0';
}

sds sdscatsds(sds s, const sds t)
{
    return (char *)"";
}
sds sdscpylen(sds s, const char *t, size_t len)
{
    return (char *)"";
}

#ifdef SDS_TEST_MAIN
#include<stdio.h>
#include<limits.h>
#include"../src/testhelp.h"

int main()
{
    // struct sdshdr * sh;
    // sds y;
    sds x = sdsnew("foo");
    printf("%zu\n",sdslen(x));
    int ret = memcmp(x,"foo\0",4);
    printf("%s\n",sdsbuf(x));
    test_cond("Create a string and obtain the length",
        sdslen(x) == 3 && ret == 0);

    sdsfree(x);
    x = sdsnewlen("foo",2);
    test_cond("Create a string with specified length",
        sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0);
    
    test_report();
    return 0;
}
#endif