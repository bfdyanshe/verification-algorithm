/**** checksum.c *****/
/* checksum 的实现 
 *
 * one's complement sum
 * 按照十六位(也就是4位hex)分割整个序列。将分割后的各个4位hex累积相加
 * 如果有超过16位的进位出现，则将进位加到后16位结果的最后一位
 * 
 * 取反，然后放入header
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define length_of_header 36
#define LOH length_of_header
#define length_of_header_with_checksum 40
#define LHC length_of_header_with_checksum
int rseed=0;
int loop_flag=0;

/************* getrandom *********************
 * 作用：给一个字符数组塞入一堆随机数
 * 参数：char *arr 要被塞入随机数的数组首地址
 *      int len 数组长度
 * 无返回值
 *********************************************/
void getrandom(char *arr, int len)
{
    int i = 0;
    if(loop_flag){
        srand((unsigned)time(0)+rseed);
        rseed=rand();
    }else{
        srand((unsigned)time(0));
    }
    while (i < len)
    {
        arr[i] = rand() % 16;
        i++;
    }
}

/************* caculate_checksum ***************
 * 作用： 计算 checksum
 * 参数：char * header 被计算的头
 *       int loh 头的长度
 * 返回值： long int sum  即 checksum
 ***********************************************/
long int caculate_checksum(char * header,int loh){
    int i=0;
    int t=0;
    int a=0;
    long int sum=0;
    int mSum=0;
    for (i = 0; i < loh;)
    {
        for (t = 0; t < 4; t++)
        {
            a = header[i];
            a = a | 0xfff0;
            mSum = mSum << 4;
            mSum = mSum | 0x000f;
            mSum = mSum & a;
            i++;
        }
        sum += mSum;
        if ((sum & 0x10000) >> 16 == 0x1)
        {
            sum += 1;
            sum = sum & 0xffff;
        }
    }
    sum = (~sum) & 0xFFFF;
    return sum;
}

/************* caculate_checksum ***************
 * 作用： 通过 checksum 核验数据
 * 参数：char * header 被计算的头
 *       int lhc length_of_header_with_checksum 头的长度
 * 返回值： long int sum  即核验后的结果，0xffff 则代表数据正常
 ***********************************************/
long int verify_checksum(char * header, int lhc){
    long int sum=0;
    int mSum=0;
    int a=0;
    int i=0;
    int t=0;
    for (i = 0; i < LHC;)
    {
        for (t = 0; t < 4; t++)
        {
            a = header[i];
            a = a | 0xfff0;
            mSum = mSum << 4;
            mSum = mSum | 0x000f;
            mSum = mSum & a;
            i++;
        }
        sum += mSum;
        if ((sum & 0x10000) >> 16 == 0x1)
        {
            sum += 1;
            sum = sum & 0xffff;
        }
    }
    return sum;
}

int sample()
{
    char header[LHC];
    char *h = header;
    int i = -1;
    long int sum = 0;
    getrandom(h, LOH);
    // printf("start... loh=%d  lhc=%d \n",LOH,LHC);
    // printf("header: ");
    // for (i = 0; i < LOH; i++)
    // {
    //     printf("%x", *(h++));
    //     if ((i + 1) % 4 == 0)
    //         printf(" ");
    // }
    // printf("\n");
    sum = caculate_checksum(header,LOH);
    // printf(" checksum: 0x%lx\n ", sum);
    // 把 checksum 放入 header 里面
    for(i=1;i<5;i++){
        header[LHC-i]=(sum&0xf);
        sum=sum>>4;
    }
    sum=verify_checksum(header,LHC);
    // printf("verification: 0x%lx\n ", sum);
    return 0;
}

int main(int argv, char** args){
    int loopTimes=0;
    if(argv==1){
        loopTimes=1;
    }else{
        loop_flag=1;
        loopTimes=atoi(args[1]);
    }
    while(loopTimes>0){
        sample();
        loopTimes--;
    }
    return 0;
}