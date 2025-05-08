#include<stdio.h>
#include<semaphore.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#define MSG_MEM_SIZE 1024
#define SEM_MEM_SIZE 64

int main()
{
	sem_t* mutex;
	sem_t* receiver_response;
	sem_t* sender_write;
	char* data;
	key_t msg_key;
	int msg_smd;
	msg_key = ftok("./sender.c",10);
	if(msg_key<0)
	{
		perror("ftok error\n");
		return 0;
	}
	mutex = sem_open("/mutex",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,1);
	receiver_response = sem_open("/receiver_response",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,0);
	sender_write = sem_open("/sender_write",O_CREAT|O_EXCL,S_IRUSR|S_IWUSR,1);
	if(mutex==SEM_FAILED||receiver_response==SEM_FAILED||sender_write==SEM_FAILED)
	{
		mutex = sem_open("/mutex",O_RDWR);
		receiver_response = sem_open("/receiver_response",O_RDWR);
		sender_write = sem_open("/sender_write",O_RDWR);
		if(mutex==SEM_FAILED||receiver_response==SEM_FAILED||sender_write==SEM_FAILED)
		{
			perror("sem_open error\n");
		}
	}
	msg_smd = shmget(msg_key,MSG_MEM_SIZE,IPC_CREAT|S_IRUSR|S_IWUSR);		if(msg_smd<0)
	{
		perror("shmget error\n");
		return 0;
	}
	data = (char*)shmat(msg_smd,0,0);
	if(data<0)
	{
		perror("shmat error\n");
		return 0;
	}
	while(1)
	{
		sem_wait(receiver_response);
		sem_wait(mutex);
		if(*data=='2')
		{
			printf("sender exit\n");
			strcpy(data+1,"over");
			*data = '2';
			sem_post(mutex);
			sem_post(sender_write);
			 break;
		}
		if(*data == '1')
		{
			printf("from sender: %s\n",data+1);
			strcpy(data+1,"over");
			*data = '0';
		}
		sem_post(mutex);
		sem_post(sender_write);
	}
	sem_unlink("/mutex");
	sem_unlink("/receiver_response");
	sem_unlink("/sender_write");
	shmdt(data);
	shmctl(msg_smd,IPC_RMID,0);
	return 0;
}
