#include <stdio.h>
#include <stdarg.h>
#include <string.h>


// 将valist拼接到符合format格式的给定buf后面
// 超出 format格式的并不能打印出来
// 要想无限制打印，则需要根据参数对buf及时扩容，具体实现参考 sds.c:sdscatfmt()
char *fmtmsg(char msgbuf[], char *format, ... )
{
    va_list arglist;

    va_start( arglist, format );
    strcpy( msgbuf, "Error: " );
    size_t copylen = 80 - 7;//除去开头的7个字符，后面73个字符可用，包括'\0'
    vsnprintf( &msgbuf[7], buflen, format, arglist );
    va_end( arglist );
    return( msgbuf );
}

void PrintFloats (int n, ...)
{
  int i;
  double val;
  printf ("Printing floats:");
  va_list vl;
  va_start(vl,n);
  for (i=0;i<n;i++)
  {
    val=va_arg(vl,double);
    printf (" [%.2f]",val);
  }
  va_end(vl);
  printf ("\n");
}

int main( void )
{
    // 指定大小
    PrintFloats(3,3.14159,2.71828,1.41421);

    // 无固定参函数
    char *msg;
    char msgbuf[80];
    msg = fmtmsg(msgbuf, "%s %d %s %s", "Failed", 100, "times" ,"haha");
    printf( "%s\n", msg );

    return 0;
}
