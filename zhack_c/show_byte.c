#include"byte_printer.h"
#include<string.h>
#include<stdlib.h>


//机器大小端实验
void test_endin()
{
    int x = 12345;
    show_int(x);
    // --> 0x00003039
    // 打印结果为，最低有效字节值0x39先输出，说明是小端法机器
    // 注：(字节顺序是从小到大，对应字节数值依次从小到大)
    // 39 30 00 00
    // 0x7ffdf13afaec 0x7ffdf13afaed 0x7ffdf13afaee 0x7ffdf13afaef

    int y = 1234;
    // 地址类型转换,将变量y的首地址转为无符号字节型，这样就可以按字节取值了
    unsigned char * start=(unsigned char*)&y;
    
    printf(" %.2x", start[0]);//输出
    printf(" %.2x", start[1]);//输出
    printf(" %.2x", start[2]);//输出
    printf(" %.2x", start[3]);//输出
    // d2 04 00 00

    printf("\n");
    show_int(y);
    // d2 04 00 00
    // 0x7fff9574849c 0x7fff9574849d 0x7fff9574849e 0x7fff9574849f
}

// 按字节拷贝实验
void test_byte_copy()
{
    unsigned char z=97;
    
    // 栈空间
    char *s  = "hello";
    // 堆空间
    char *ps = (char*)malloc(sizeof(char)*5+1);
    printf("%s\n",s);
    
    printf("97-->0x%.2x\n", ((unsigned char*)&z)[0]);
    show_int(z);

    memcpy(ps, s, strlen(s));
    ps = realloc(ps, strlen(s)+5);
    // 将1个字节拷贝到ps字符串之后
    memcpy(ps+5, (char *)&z, 1);
    
    ps[6]='\0';//字符串结束符，必须的，否则没法使用strlen等字符串操作函数

    printf("ps=%s, len=%zu\n", ps, strlen(ps));
    //ps=helloA,len=6

    free(ps);

}

// 常见内存操作错误示例
void test_memery_err()
{
    char *s = "hello";
    printf("%s\n",s);
    // *s = '9'; //Segmentation fault
    //原因： 由于C语言在分配给"hello"常量字符串的空间的时候，是分配到静态只读数据段
    //  即 "hello" 存储在 .rodata 段，此段数据是不可写的，所有试图对该数据段进行修改的操作统统驳回
    // 如果需要对 s 进行修改，必须在非只读数据段进，即需要给s分配动态内存空间(堆)
    s = malloc(sizeof(char)*5);
    s = memcpy(s,"aello",6);//包含结束字符
    printf("%s\n",s);
}

int main()
{
    // test_endin();
    // test_byte_copy();
    test_memery_err();
    return 0;
}
