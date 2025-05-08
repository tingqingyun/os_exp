//sender1.c
#include "common.h"
// gcc sender1.c -o sender1 -lpthread -lrt
 
int main(){
    int mq;
    struct msg_st buf;
    ssize_t bytes_read;
    //创建或者打开有名信号量
    create_sem();
    //创建消息队列
    mq = msgget((key_t) QUEUE_ID, 0666 | IPC_CREAT);//msgget返回一个消息队列标识符，
    CHECK((key_t) -1 != mq);
 
    do {
        printf("sender1> ");
        fflush(stdout);//冲刷输出缓冲区
        fgets(buf.buffer, MAX_SIZE, stdin);
        buf.message_type = snd_to_rcv1;
        /* send the message */
        P(empty);//队列有空闲
        P(w_mutex);
        CHECK(0 <= msgsnd(mq, (void*)&buf, MAX_SIZE, 0));//向标识符为mq的队列发送消息，成功返回1，失败返回-1。
        V(full);
        V(w_mutex);
     } while (strncmp(buf.buffer, MSG_STOP, strlen(MSG_STOP)));//字符比较，结束条件即发送exit字符串。
 
    /* wait for response */
    P(over11);
    bytes_read = msgrcv(mq, (void *) &buf, MAX_SIZE, rcv_to_snd1, 0);
/*
msgtype消息类型，即rcv_to_snd1.
0:接收消息队列中的第一个消息
>0:接受第一个为msgtype类型的消息
<0:接受第一个类型小于等于msgtype的绝对值的消息
*/
    CHECK(bytes_read >= 0);
    printf("%s", buf.buffer);
    return 0;
}
