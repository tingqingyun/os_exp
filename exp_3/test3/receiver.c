//receiver.c
// gcc receiver.c -o receiver -lpthread -lrt
#include "common.h"
int main()
{
    create_sem();
    struct msg_st buf, over1, over2;
    int mq, must_stop = 2;
    struct msqid_ds t;
    over1.message_type = 3;
    strcpy(over1.buffer, "over1\n");
    over2.message_type = 4;
    strcpy(over2.buffer, "over2\n");

    /* open the mail queue */
    mq = msgget((key_t) QUEUE_ID, 0666 | IPC_CREAT);
    CHECK((key_t) -1 != mq);
 
    do {
        ssize_t bytes_read, bytes_write;
        /* receive the message */
        P(full);
        bytes_read = msgrcv(mq, (void *) &buf, MAX_SIZE, 0, 0);
        V(empty);
        CHECK(bytes_read >= 0);
        if (!strncmp(buf.buffer, MSG_STOP, strlen(MSG_STOP))) {
            if (buf.message_type == 1) {
                printf("send over1 to sender1\n");
                bytes_write = msgsnd(mq, (void *) &over1, MAX_SIZE, 0);
                CHECK(bytes_write >= 0);
                V(over11);
                must_stop--;
            } else if (buf.message_type == 2) {
                printf("send over2 to sender2\n");
                bytes_write = msgsnd(mq, (void *) &over2, MAX_SIZE, 0);
                CHECK(bytes_write >= 0);
                V(over22);
                must_stop--;
            }
        } else {
            printf("Received%ld: %s", buf.message_type, buf.buffer);
        }
    } while (must_stop);
    sleep(1); 
    CHECK(!msgctl(mq, IPC_RMID, &t));
    release_sem();
    return 0;
}