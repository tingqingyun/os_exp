#include<stdio.h>
#include<mqueue.h>
#include<string.h>

#define MAX_SIZE 256

int main() {
	mqd_t mq;
	struct mq_attr attr;
	char buffer[MAX_SIZE];
	    mq = mq_open("/p", O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(0);
    }
	    while (1) {
        printf("sender1: input your message\n");
        scanf("%s", buffer);
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        char send_buffer[MAX_SIZE];
        snprintf(send_buffer, MAX_SIZE, "sender1:%s", buffer);
        mq_send(mq, send_buffer, strlen(send_buffer), 1);
    }

    mq_send(mq, "sender1:end1", strlen("sender1:end1"), 1);

    mq_close(mq);
    return 0;
}
