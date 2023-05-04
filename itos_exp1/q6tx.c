#include "common.h"

#define SHMKEY 7755

int shmid, i;
int *addr;

void client()
{
    int i;
    shmid=shmget(SHMKEY,1024,0777);      /*打开共享存储区*/
    addr=shmat(shmid,0,0);           /*获得共享存储区首地址*/
    for (i=9;i>=0;i--)
    {
        while (*addr!=-1);
        printf("(client) sent\n");
        *addr=i;
    }
    exit(0);
}
