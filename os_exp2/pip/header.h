#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<semaphore.h>	
#include<fcntl.h>
#include<stdlib.h>
#define BUFF_SIZE 256
int write1(int fd);
int write2(int fd);
int write3(int fd);
