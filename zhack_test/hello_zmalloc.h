#ifndef __ZMALLOC_H
#define __ZMALLOC_H

//内存分配函数包装

#define zmalloc malloc
#define zrealloc realloc
#define zcalloc(x) calloc(x,1)
#define zfree free
#define zstrdup strdup

#endif