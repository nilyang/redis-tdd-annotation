#include"byte_printer.h"

//机器大小端实验

int main()
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

    return 0;
}