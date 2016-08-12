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
 */
sds sdscatlen(sds s, const void *t, size_t len)
{
    struct sdshdr* sh;
    // 原有字符串长度
    size_t curlen = sdslen(s);

    // 扩展原buf字段
    s = sdsMakeRoomFor(s, len);

    // 字符串相加
    sh = (struct sdshdr *)(s - (sizeof(struct sdshdr)));
    //T=O(N)
    int i;
    const char* tt = t;
    for(i=0;i < len; i++){
        sh->buf[curlen+i] = tt[i];
    }

    sh->buf[sh->len] = '\0';
    return sh->buf;
}

sds sdscat(sds s, const char * t)
{
    return sdscatlen(s,t,strlen(t));
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
sds sdsMakeRoomFor(sds s, size_t addlen)
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

    //测试生成指定长度sds
    sdsfree(x);
    x = sdsnewlen("foo",2);
    test_cond_ext("Create a string with specified length",
        sdslen(x) == 2 && memcmp(x,"fo\0",3) == 0);
    

    //sds字符串拼接
    x = sdscat(x, "bar");

    x = sdsMakeRoomFor(x, 10);

    sdsfree(x);
    // test_cond_ext("Strings concatenation",
        // sdslen(x)==5 && memcmp(x, "fobar\0", 6) == 0);
    

    test_report_ext();
    return 0;
}
#endif