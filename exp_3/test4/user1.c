#include "init.h"
// gcc user1.c -o user1 -lpthread -lrt

pthread_t r_thread, s_thread;
void *send(void *arg)
{
    char msg[MEM_MIN_SIZE];
    while (1)
    {
        char s_str[100] = {0};
        // printf("Process %d input:", *((pid_t *)arg));
        fflush(stdout);
 
        fgets(s_str, 100, stdin);
        s_str[strlen(s_str) - 1] = '\0';
 
        sem_wait(sem_sender);  // 等待发送信号量
        
        sem_wait(sem_mutex);   // 获取互斥锁
        sprintf(msg, "%d:%s", *((pid_t *)arg), s_str);
        strcpy((char *)shm_p, msg);
        sem_post(sem_mutex);   // 释放互斥锁
 
        sem_post(sem_receiver2);  // 通知接收方
 
        if (!strcmp(s_str, "over"))
        {
            pthread_cancel(r_thread);
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
        sem_wait(sem_receiver1);  // 等待接收信号量
        
        sem_wait(sem_mutex);      // 获取互斥锁
        strcpy(r_str, (char *)shm_p);
        sem_post(sem_mutex);      // 释放互斥锁
 
        p = strchr(r_str, ':');
        *(p++) = '\0';
        printf("Received message from process %s: %s\n", r_str, p);

        if (strcmp(p, "over") == 0)
        {
            pthread_cancel(s_thread);
            shmdt(shm_p);
            shmctl(shm_id, IPC_RMID, NULL);
            
            // 清理所有信号量
            sem_unlink("sender");
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
    // pthread_create() see https://book.itheima.net/course/223/1277519158031949826/1277528575729213444
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