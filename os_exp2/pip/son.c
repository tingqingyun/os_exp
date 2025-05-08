#include "header.h"
int write1(int fd)
{
	char* buff = NULL;
	size_t buff_size = 0;
	size_t length = 0;
	char ch;
	while((ch = getchar() !='\n'))
	{
		if(length + 1>buff_size)
		{
			buff_size +=128;
			buff = realloc(buff,buff_size);
			if(!buff)
			{
				perror("realloc error");
			}
		}
		buff[length++] = ch;
	}
	strcat(buff,"this is son1\n");
	printf(buff);
	return write(fd,buff,strlen(buff));
}

int write2(int fd)
{
	char* buff = NULL;
	size_t buff_size = 0;
	size_t length = 0;
	char ch;
	while((ch = getchar() !='\n'))
	{
		if(length + 1>buff_size)
		{
			buff_size +=128;
			buff = realloc(buff,buff_size);
			if(!buff)
			{
				perror("realloc error");
			}
		}
		buff[length++] = ch;
	}
	strcat(buff,"this is son2\n");
	printf(buff);
	return write(fd,buff,strlen(buff));
}

int write3(int fd)
{
	char* buff = NULL;
	size_t buff_size = 0;
	size_t length = 0;
	char ch;
	while((ch = getchar() !='\n'))
	{
		if(length + 1>buff_size)
		{
			buff_size +=128;
			buff = realloc(buff,buff_size);
			if(!buff)
			{
				perror("realloc error");
			}
		}
		buff[length++] = ch;
	}

	strcat(buff,"this is son3\n");
	printf(buff);
	return write(fd,buff,strlen(buff));
}
