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
// 求从2开始的m个素数
int test_prime(void)
{
    const int m = 20;
    int prime[m];//c99 standard
    int count  = 1;
    int i = 3;
    for(i=0;i<m;i++)prime[i]=2;
    // printf("%d\n",m);
    i=3;
    while( count < m) {
        if (isPrime(i, prime, count)){
            // printf("count=%d\n",count);
            prime[count++] = i;
        }
        i++;
    }
    for(i=0; i<m;i++){
        printf("%d\t", prime[i]);
        if( (i+1) % 5 == 0 ) printf("\n");
    }
    return 0;
}
#endif