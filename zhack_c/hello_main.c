//c 语言练习
#include<stdio.h>
#include<string.h>

//#include"hello.h"
// #include "hello_prime.h"
#include"hello_string.h"

// struct sdshdr{
//     unsigned int len;
//     unsigned int free;
//     char buf[];//zero length array,only allowd in struct's last element.
// };

int main()
{
    // printf("sizeof(struct sdshdr)=%zu\n",sizeof(struct sdshdr));

    // test_prime();
    test_strstr();
    return 0;
}
