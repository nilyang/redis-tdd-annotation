//c 语言练习
#ifndef __HELLO_PRIME_H
#define __HELLO_PRIME_H

#include <math.h>

int is_prime(int x)
{
    int ret = 1;
    int i;
    if( x == 1) ret = 0;

    for(i=2;i < x;i++){
        if(x%i==0){
            ret = 0;
            break;
        }
    }
    return ret;
}

//偶数都不是素数
//从3开始的奇数开始计算,从3到x-1，每次加2
//无需x-1，到sqrt(x)就够了
int is_prime_new(int x)
{
    int ret = 1;
    int i;
    if (x == 1 || (x % 2 ==0 && x!=2))
    {
        ret = 0;
        return ret;
    }

    for(i=3; i< sqrt(x); i+=2){
        if(x % i == 0){
            ret = 0;
            break;
        }
    }
    
    return ret;
}

int isPrime(int x, int knownPrimes[], int numberOfKnownPrimes)
{
    int ret=1;
    int i;
    for(i=0 ; i < numberOfKnownPrimes; i++){
        if (x % knownPrimes[i] == 0){
            ret = 0;
            break;
        }
    }

    return ret;
}

// 判断是否能被已知的且<x的素数整除
int test_prime(void)
{
//初始化，得到小于查找数字n的素素表
//依次排除所得素数的倍数，直到该倍数刚好小于N
//接着下一轮循环
    const int m = 100;
    int prime[m];//c99 standard
    prime[0] = 2;
    int count  = 1;
    int i = 3;
    while( count < m) {
        if (isPrime(i, prime, count)){
            prime[count++] = i;
        }
        i++;
    }

    for(i=0; i<m;i++){
        printf("%d", prime[i]);
        if( (i+1)%5)printf("\n");
        else printf("\n");
    }
    return 0;
}
#endif