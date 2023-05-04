#include "common.h"

#define MSGKEY 7775

struct msgform
{
    long mtype;
    char mtext[1000];
} msg;

int msgqid;

void client()
{
    int i;
    msgqid=msgget(MSGKEY,0777);   /*打开75#消息队列*/
    for(i=10;i>=1;i--)
    {
        msg.mtype=i;
        printf("(client)sent\n");
        msgsnd(msgqid,&msg,1024,0);     /*发送消息*/
    }
    exit(0);
}

int main()
{
    client();
}
