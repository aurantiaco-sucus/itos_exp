#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define PRG_LEN 320 //指令数
#define MAX 1000

void init(int *prg) {
    double random[PRG_LEN];
    for (int i = 0; i < PRG_LEN; ++i) {
        random[i] =
    }
    int i = 0;
    while (true) { //按照实验要求随机生成指令的地址
        prg[i] = rand() % PRG_LEN;
        prg[i] = prg[i++] + 1;
        if (i >= PRG_LEN)break;
        prg[i] = rand() % (prg[i++] + 1);
        prg[i] = prg[i++] + 1;
        if (i >= PRG_LEN)break;
        prg[i] = rand() % (PRG_LEN - prg[i] - 1) + prg[i++] + 1;
        prg[i++] = prg[i++] + 1;
        if (i >= PRG_LEN)break;
    }
}

int instructionset[PRG_LEN]; //指令序列
int page[PRG_LEN]; //指令序列对应的页面

void init(); //随机初始化测试数据
void OPT(int n); //最佳淘汰算法（OPT）
void FIFO(int n); //先进先出的算法（FIFO）
void LRU(int n); //最近最久未使用算法（LRU）

int main()
{
    srand((unsigned long)time(0)); //产生随机数种子
    init(); //随机初始化测试数据
    printf("最佳淘汰算法（OPT）:\n");
    for(int i=4;i<=32;i++)OPT(i);

    printf("先进先出的算法（FIFO）:\n");
    for(int i=4;i<=32;i++)FIFO(i);

    printf("最近最久未使用算法（LRU）:\n");
    for(int i=4;i<=32;i++)LRU(i);
}

void LRU(int n)
{
    int i,j,k,flag,record[n],memory[n],value=0; //memory为内存，record记录进程内页面的信息，记录上次使用到现在的时间
    double right=0;
    for(i=0;i<n;i++){
        record[i]=-1;
        memory[i]=-1;
    }

    for(i = 0; i < PRG_LEN; i++){
        flag = 0;
        for(j=0;j<n;j++) //记录该页面占用的时间
            if(record[j]!=-1)record[j]=record[j]+1 ;

        for(j=0;j<n;j++)
            if(page[i]==memory[j]){
                right+=1;
                record[j]=0;
                flag = 1;
                break;
            }
        if(flag==1)continue;
        for(j=0;j<n;j++){ //缺页中断
            if(record[j] == -1){ //有空余内存，可直接进入
                record[j] = 0;
                memory[j] = page[i];
                flag = 1;
                break;
            }
        }
        if(flag==1)continue;
        for(j=0;j<n;j++) //记录页面占用的时间最多的下标
            if(record[j]>=record[value]) value = j;
        memory[value] = page[i]; //替换
    }
    printf("内存%d页命中率=%lf\n",n, right / PRG_LEN);
    FILE *fp;
    fp=fopen("LRU.txt","a");
    fprintf(fp,"%lf ", right / PRG_LEN);
    fclose(fp);
}

void FIFO(int n)
{
    int i,j,k,flag,record[n],memory[n],value=0; //memory为内存，record记录进程内页面的信息，是否已经使用
    double right=0;
    for(i=0;i<n;i++){
        record[i]=-1;
        memory[i]=-1;
    }
    for(i = 0; i < PRG_LEN; i++){
        flag = 0;
        for(j=0;j<n;j++)
            if(page[i]==memory[j]){
                right+=1;
                flag = 1;
                break;
            }
        if(flag==1)continue;
        for(j=0;j<n;j++) //缺页中断
            if(record[j] == -1){ //有空余内存，可直接进入
                record[j] = MAX;
                memory[j] = page[i];
                flag = 1;
                break;
            }
        if(flag==1)continue;
        memory[value] = page[i]; //value记录最早进来的，因为是按顺序，接下来加一即可
        value = (value+1)%n;

    }
    printf("内存%d页命中率=%lf\n",n, right / PRG_LEN);
    FILE *fp;
    fp=fopen("FIFO.txt","a");
    fprintf(fp,"%lf ", right / PRG_LEN);
    fclose(fp);
}

void OPT(int n)
{
    int i,j,k,flag,record[n],memory[n],value=0; //memory为内存，record记录进程内页面的信息，用于淘汰
    double right=0;
    for(i=0;i<n;i++){
        record[i]=-1;
        memory[i]=-1;
    }
    for(i = 0; i < PRG_LEN; i++){
        flag = 0;
        for(j=0;j<n;j++)
            if(page[i]==memory[j]){
                right+=1;
                flag = 1;
                break;
            }
        if(flag==1)continue;
        for(j=0;j<n;j++) //缺页中断
            if(record[j] == -1){ //有空余内存，可直接进入
                record[j] = MAX;
                memory[j] = page[i];
                flag = 1;
                break;
            }
        if(flag==1)continue;
        for(j=0;j<n;j++){ //遍历已占内存的页面在将来最早使用是什么时候,在遍历的过程中顺便把最远的记录下来淘汰
            record[j] = MAX;
            for(k=i+1; k < PRG_LEN; k++)
                if(memory[j]==page[k]){
                    record[j] = k-i;
                    break;}
            if(record[j]>=record[value])value=j;
        }
        memory[value] = page[i];
    }
    printf("内存%d页命中率=%lf\n",n, right / PRG_LEN);
    FILE *fp;
    fp=fopen("OPT.txt","a");
    fprintf(fp,"%lf ", right / PRG_LEN);
    fclose(fp);
}

void init()
{
    int i= 0;

    while(1){ //按照实验要求随机生成指令的地址
        instructionset[i] = rand() % PRG_LEN;
        instructionset[i] = instructionset[i++]+1;
        if(i >= PRG_LEN)break;
        instructionset[i] = rand()%(instructionset[i++]+1);
        instructionset[i] = instructionset[i++]+1;
        if(i >= PRG_LEN)break;
        instructionset[i] = rand()%(PRG_LEN - instructionset[i] - 1) + instructionset[i++] + 1;
        instructionset[i++] = instructionset[i++]+1;
        if(i >= PRG_LEN)break;
    }
/*
while(1){ //展开实验，不按照要求随机生成指令的地址
instructionset[i++] = rand()%PRG_LEN;
if(i>=PRG_LEN)break;
}

while(1){ //展开实验，模拟较早调入的页往往是经常访问的页
instructionset[i++] = rand()%PRG_LEN;
if(i>=PRG_LEN)break;
if(i>=20){
int num = rand()%10;
if(num<=4)
instructionset[i++] = instructionset[num];
}
if(i>=PRG_LEN)break;
}
*/

    for(i=0; i < PRG_LEN; i++)
        page[i] = instructionset[i]/10;
/*
for(i=0;i<PRG_LEN;i++)
printf("->%d",instructionset[i]);
printf("\n");
*/

    printf("指令序列对应的页面：\n");
    for(i=0; i < PRG_LEN; i++)
        printf("->%d",page[i]);
    printf("\n");
}
