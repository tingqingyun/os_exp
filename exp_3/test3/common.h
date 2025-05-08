///common.h
 
#ifndef EXP3_3_COMMON_H
#define EXP3_3_COMMON_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/msg.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#define QUEUE_ID    10086
#define MAX_SIZE    1024
#define MSG_STOP    "exit"
//message trace
#define snd_to_rcv1 1
#define snd_to_rcv2 2
#define rcv_to_snd1 3
#define rcv_to_snd2 4
#define CHECK(x) \
    do { \
        if (!(x)) { \
            fprintf(stderr, "%s:%d: ", __func__, __LINE__); \
            perror(#x); \
            exit(-1); \
        } \
    } while (0) \
 
//stderr shu chu biaozhun cuowu
//perror shu chu cuowu yuan yin
/* global variable */
sem_t *w_mutex, *empty, *full, *over11,*over22;
char *SEM_NAME[5] = {"w_mutex", "empty","full", "over11","over22"};
void create_sem()
{
    w_mutex = sem_open(SEM_NAME[0], O_CREAT, 0666, 1);
    empty = sem_open(SEM_NAME[1], O_CREAT, 0666, 10);
    full = sem_open(SEM_NAME[2], O_CREAT, 0666, 0);
    over11 = sem_open(SEM_NAME[3], O_CREAT, 0666, 0);
    over22 = sem_open(SEM_NAME[4], O_CREAT, 0666, 0);
    return ;
}
void release_sem()
{
    int i;
    sem_close(w_mutex);
    for(i = 0; i < 10; ++i)
        sem_close(empty);
    for(i = 0; i < 4; ++i)
        sem_unlink(SEM_NAME[i]);
    return ;
}
 
#define P(x) sem_wait(x)
#define V(x) sem_post(x)
//消息缓冲区
struct msg_st {
    long int message_type;//消息类型
    char buffer[MAX_SIZE];//消息正文
};
 
//进程随机获得写数据的机会
int get_chance(int range)
{
    srand((unsigned)time(NULL));
    return rand()%range;
}//随机数
#endif //EXP3_3_COMMON_H