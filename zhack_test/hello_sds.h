

#ifndef ZMALLOC_H
#define ZMALLOC_H

#define zmalloc malloc
#define zrealloc realloc
#define zcalloc(x) calloc(x,1)
#define zfree free
#define zstrdup strdup

#endif

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
sds sdscatsds(sds s, const sds t);
sds sdscpylen(sds s, const char *t, size_t len);