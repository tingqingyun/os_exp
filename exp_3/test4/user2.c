#include "init.h"
// gcc user2.c -o user2 -lpthread -lrt
pthread_t r_thread, s_thread;

void *send(void *arg)
{
    char msg[MEM_MIN_SIZE]; // msg -> fmt string
    // char s_str[100];        // input string; bug
    while (1)
    {
        char s_str[100] = {0}; // fix bug
        // printf("Process %d input:", *((pid_t *)arg));
        fflush(stdout); // fflush(stdout) is to flush the output buffer.

        // scanf("%[^\n]%*c", s_str); // cant read line with space

        fgets(s_str, 100, stdin);        // can read line with space
        s_str[strlen(s_str) - 1] = '\0'; // remove '\n'

        // Wait for the receiver to be ready.
        sem_wait(sem_sender);

        sem_wait(sem_mutex);   // 获取互斥锁
        sprintf(msg, "%d:%s", *((pid_t *)arg), s_str);
        strcpy((char *)shm_p, msg);
        sem_post(sem_mutex);   // 释放互斥锁

        // Tell the receiver(user1) that the message is ready.
        sem_post(sem_receiver1);

        // send "over" to exit.
        if (!strcmp(s_str, "over"))
        {
            pthread_cancel(r_thread); // Cancel the receiver thread.
            shmdt(shm_p);
            break;
        }
    }
}

void *receive(void *arg)
{
    char r_str[100];
    char *p;
    while (1)
    {
        // Wait to receive a message from the sender(user1).
        sem_wait(sem_receiver2);

        sem_wait(sem_mutex);      // 获取互斥锁
        strcpy(r_str, (char *)shm_p);
        sem_post(sem_mutex);      // 释放互斥锁

        // Find the first ':' in the string.
        p = strchr(r_str, ':');
        *(p++) = '\0';
        printf("Received message from process %s: %s\n", r_str, p);

        // if user1 send "over" to exit.
        if (strcmp(p, "over") == 0)
        {
            pthread_cancel(s_thread);       // Cancel the sender thread.
            shmdt(shm_p);                   // Detach the shared memory.
            shmctl(shm_id, IPC_RMID, NULL); // Remove the shared memory.
            sem_unlink("sender");           // Remove the semaphore.
            sem_unlink("receiver1");
            sem_unlink("receiver2");
            sem_unlink("mutex"); 
            break;
        }

        // printf("Process %d input:\n", *((pid_t *)arg));
        fflush(stdout);
        sem_post(sem_sender);
    }
}

int main()
{
    pid_t pid = getpid();
    int res1 = 0, res2 = 0;

    init();

    printf("Process %d Start...\n\n", pid);
    res1 = pthread_create(&s_thread, NULL, send, &pid);
    res2 = pthread_create(&r_thread, NULL, receive, &pid);

    if (res1 || res2)
    {
        printf("create pthread failed\n");
        return 1;
    }

    pthread_join(s_thread, NULL);
    pthread_join(r_thread, NULL);
    printf("Process %d Exit...\n", pid);
}