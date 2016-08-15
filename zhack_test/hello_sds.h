
#ifndef __SDS_H
#define __SDS_H

#ifndef ZMALLOC_H
#define ZMALLOC_H

#define zmalloc malloc
#define zrealloc realloc
#define zcalloc(x) calloc(x,1)
#define zfree free
#define zstrdup strdup

#endif
/*
 * 最大预分配长度
 */
#define SDS_MAX_PREALLOC (1024*1024)

#include <sys/types.h>
#include <stdarg.h>

typedef char * sds;

struct sdshdr{
    unsigned int len;
    unsigned int free;
    char buf[];
};

static inline size_t sdslen(const sds s){
    struct sdshdr *sh = (void *)(s-(sizeof(struct sdshdr)));
    return sh->len;
}

static inline size_t sdsavail(const sds s){
    struct sdshdr *sh = (void *)(s-(sizeof(struct sdshdr)));
    return sh->free;
}

/**
 * 为了方便测试，通过sds指针获取sdshdr结构的数据字段buf
 * @param const sds s
 * @return sds
 */
sds sdsbuf(const sds s)
{
    return (((struct sdshdr *)(s - (sizeof(struct sdshdr))))->buf);
}

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
sds sdsempty(void);
size_t sdslen(const sds s);
sds sdsdup(const sds s);
void sdsfree(sds s);
size_t sdsavail(const sds s);
sds sdsgrowzero(sds s, size_t len);
sds sdscatlen(sds s, const void *t, size_t len);
sds sdscat(sds s, const char * t);
sds sdscatsds(sds s, const sds t);
sds sdscpylen(sds s, const char *t, size_t len);
sds sdscpy(sds s, const char *t);

/* Low level functions exposed to the user API */
sds sdsMakeRoomFor(sds s, size_t addlen);
void sdsIncrLen(sds s, int incr);
sds sdsRemoveFreeSpace(sds s);
size_t sdsAllocSize(sds s);

#endif