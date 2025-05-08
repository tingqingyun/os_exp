#include<stdio.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<pthread.h>
#include<semaphore.h>
#include<mqueue.h>
#include<string.h>
#include<stdlib.h>
#define MAX_SIZE 256
mqd_t mq;
sem_t sem_sender1,sem_sender2,sem_receive,sem_sender1_end;

void* sender1()
{
	char buffer[MAX_SIZE];
	while(1)
	{
		sem_wait(&sem_sender1);
		printf("sender1: input");
		scanf("%s",buffer);
		if(strcmp(buffer,"exit")==0)
		{
			break;
		}
		char send_buffer[MAX_SIZE];
		snprintf(send_buffer,MAX_SIZE,"sender1:%s",buffer);
		//printf("sended message:%s",send_buffer);
		mq_send(mq,send_buffer,strlen(send_buffer),1);
		sem_post(&sem_receive);
	}
	mq_send(mq,"sender1:end1",strlen("sender1:end1"),1);
	sem_post(&sem_receive);
	sem_wait(&sem_sender1_end);
	mq_receive(mq,buffer,strlen(buffer),NULL);
	printf("%s\n",buffer);
	if(strcmp(buffer,"over1")==0)
	{
		printf("sender1 over\n");
		sem_post(&sem_sender1_end);
	}
}

void* sender2()
{
	char buffer[MAX_SIZE];
	while(1)
	{
		sem_wait(&sem_sender2);
		printf("sender2: input your message\n");
		scanf("%s",buffer);
		if(strcmp(buffer,"exit")==0)
		{
			break;
		}
		char send_buffer[MAX_SIZE];
		snprintf(send_buffer,MAX_SIZE,"sender1:%s\n",buffer);
		mq_send(mq,send_buffer,strlen(send_buffer),1);
		sem_post(&sem_receive);
	}
	mq_send(mq,"sender2:end2",strlen("end2"),1);
	sem_post(&sem_receive);
}

void* receiver()
{
	char buffer[MAX_SIZE];
	while(1)
	{
		sem_wait(&sem_receive);
		mq_receive(mq,buffer,MAX_SIZE,NULL);
		printf("receive receiced message:%s\n",buffer);
		if(strncmp(buffer,"sender1:",8)==0)
		{
			if(strcmp(buffer+8,"end1")==0)
			{
				mq_send(mq,"over1",strlen("over1"),1);
			}
			else
			{
				sem_post(&sem_sender1);
			}
		}
		else if(strncmp(buffer,"sender2:",8)==0)
		{
			if(strcmp(buffer+8,"end2")==0)
			{
				mq_send(mq,"over2",strlen("over2"),1);
				break;
			}
			else
			{
				sem_post(&sem_sender2);
			}
		}
	}
	mq_close(mq);
	mq_unlink("/p");
}

int main()
{
	if(mq_unlink("/p")==-1)
	{
		perror("mq_unlink_first");
	}
	pthread_t sender1_thread,sender2_thread,receive_thread;
	sem_init(&sem_sender1,0,1);
	sem_init(&sem_sender2,0,1);
	sem_init(&sem_receive,0,0);
	sem_init(&sem_sender1_end,0,1);
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize  = MAX_SIZE;
	attr.mq_curmsgs = 0;
	mq = mq_open("/p",O_CREAT | O_RDWR,0644,&attr);
	if(mq == (mqd_t)-1)
	{
		perror("mq_open");
		exit(0);
	}
	pthread_create(&sender1_thread,NULL,sender1,NULL);
	pthread_create(&sender2_thread,NULL,sender2,NULL);
	pthread_create(&receive_thread,NULL,receiver,NULL);
	pthread_join(sender1_thread,NULL);
	pthread_join(sender2_thread,NULL);
	pthread_join(receive_thread,NULL);
	sem_destroy(&sem_sender1);
	sem_destroy(&sem_sender2);
	sem_destroy(&sem_receive);
	sem_destroy(&sem_sender1_end);
	return 0;
}
