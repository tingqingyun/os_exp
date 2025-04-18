#include<iostream>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>
#include<sys/wait.h>
using namespace std;

//string sys_cmd[4] = {"exit","cmd1","cmd2","cmd3"};

void fork_cmd(int index)
{
	pid_t pid;
	pid = fork();
	int result = 0;
	if(pid<0)
	{
		cout<<"error pid";
	}
	else if(pid==0)
	{
		switch(index)
		{
			case 1:
				result = execl("./cmd1","cmd1",NULL);
				break;
			case 2:
				result = execl("./cmd2","cmd2",NULL);
				break;
			case 3:
				result = execl("./cmd3","cmd3",NULL);
				break;
			default:
				cout<<"the index is wrong\n";
		}
		if(result == -1)
		{
			perror("execl failed\n");
		}
		exit(1);
	}		
	else 
	{
		int status;
		waitpid(pid,&status,0);
	}
}
				
	
	
void run(int index)
{
	switch(index)
	{
		case 0:
			exit(0);
			break;
		case 1:
			fork_cmd(1);
			break;
		case 2:
			fork_cmd(2);
			break;
		case 3: 
			fork_cmd(3);
			break;
		default:
			cout<<"wrong index\n";
	}
}
int main()
{	
	int cmd_index = 0; 
 	while(1)
	{
		cout<<"\n";
		cout<<"     welcome to shell   \n";
		cout<<"------------------------\n";
		cout<<"        0: exit         \n";
		cout<<"        1: cmd1         \n";	
		cout<<"        2: cmd2         \n";
		cout<<"        3: cmd3         \n\n";
		cout<<"please input your commond:";
		scanf("%d",&cmd_index);
		run(cmd_index); 
	}
	return 0;

}
