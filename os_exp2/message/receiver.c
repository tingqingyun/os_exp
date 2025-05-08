#include<stdio.h>
#include<mqueue.h>
#include<string.h>
#include<stdlib.h>
#include<semaphore.h>

#define MAX_SIZE 256

int main() {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE];
    mq = mq_open("/p", O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(0);
    }

    sem_t sem_sender1_end;
    sem_init(&sem_sender1_end, 0, 1);

    int sender1_done = 0, sender2_done = 0;

    while (!sender1_done || !sender2_done) {
        mq_receive(mq, buffer, MAX_SIZE, NULL);
        printf("received message: %s\n", buffer);
        if (strncmp(buffer, "sender1:", 8) == 0) {
            if (strcmp(buffer + 8, "end1") == 0) {
                sender1_done = 1;
            }
        } else if (strncmp(buffer, "sender2:", 8) == 0) {
            if (strcmp(buffer + 8, "end2") == 0) {
                sender2_done = 1;
            }
        }
    }

    mq_close(mq);
    mq_unlink("/p");

    sem_destroy(&sem_sender1_end);
    return 0;
}
