#ifndef __HELLO_STRING_H
#define __HELLO_PRIME_H
#include<string.h>
/**
 * strstr二进制安全测试
 * 发现strstr遇到'\0'几结束，非二进制安全
 */
void test_strstr()
{
    const char *haystack="he\0l3lo";
    printf("%p\n", strstr(haystack, "e"));
    printf("%p\n", strstr(haystack, "l"));
    printf("%p\n", strstr("hello", "l\0o"));
    //证明了 strstr 遇到'\0'几结束，非二进制安全
}

void test_memmem()
{
    const char *haystack="he\0l3lo";
    printf("%p\n", memmem(haystack, 7, "e", 1));
    printf("%p\n", memmem(haystack, 7, "l", 1));
    printf("%p\n", memmem("hello", 5, "l\0o", 3));
}
#endif
