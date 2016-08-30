#include<stdio.h>
//p51 图2-4 《深入了解计算机系统第二版》

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, int len)
{
    int i;
    for (i=0; i<len; i++) {
        printf(" %.2x", start[i]);
    }

    printf("\n");
    for (i=0; i<len; i++) {
        printf(" %p", &start[i]);
    }

    printf("\n");
}

void show_int(int x)
{
    show_bytes((byte_pointer)&x, sizeof(int));
}

void show_float(float x) 
{
    show_bytes((byte_pointer)&x, sizeof(float));
}

void show_pointer(void *x)
{
    show_bytes((byte_pointer)&x, sizeof(void*));
}
