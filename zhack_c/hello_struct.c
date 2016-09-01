#include <stdio.h>
#include <stdlib.h>
int main()
{
    struct foo {
        int num;
    };

    struct foo *pfoo;
    pfoo = (struct foo*)malloc(sizeof(struct foo));
    pfoo->num = 2323;
    printf("%p=%d\n",pfoo, pfoo->num);
    free(pfoo);
    // 仍然可以访问，这是为啥？
    printf("%d\n", pfoo->num);

    // 下面这种会内存报错，这是为啥？
    struct foo bar,*pbar;
    bar.num = 33;
    pbar = &bar;
    // free(pbar);//内存访问错误
    printf("%d\n", pbar->num);
}