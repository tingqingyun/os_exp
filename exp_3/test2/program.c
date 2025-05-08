#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MUTEX_SEM "/pipe_mutex"
#define SEM_CHILD2 "/child2_sem"
#define SEM_CHILD3 "/child3_sem"
#define SEM_PARENT_READ "/parent_read_sem"
#define SEM_PARENT_READ_INTER "/parent_read_inter_sem"  // 新增中间读取信号量


volatile sig_atomic_t timeout = 0; // 超时标志

// SIGALRM 信号处理函数
void alarm_handler(int sig) 
{
    timeout = 1;
}

// 子进程1：写入数据，带有超时控制
void child1_process(int write_fd, int kb) {
    sem_t *mutex = sem_open(MUTEX_SEM, 0);
    sem_t *sem2 = sem_open(SEM_CHILD2, 0);
    
    int size = kb * 1024;
    char *buf = malloc(size);
    memset(buf, 'A', size);

    signal(SIGALRM, alarm_handler);  // 设置超时信号处理
    alarm(5);  // 设置5秒超时
    
    sem_wait(mutex);
    ssize_t written = write(write_fd, buf, size);
    sem_post(mutex);
    
    alarm(0);  // 取消超时计时器

    if (timeout) {
        int available;
        ioctl(write_fd, FIONREAD, &available);
        printf("子进程一: 阻塞型写入超时，程序已停止。子进程一成功写入 %zd KB,当前管道中有 %d KB数据\n", written / 1024 , available/ 1024);
        // exit(EXIT_FAILURE); 
    } else {
        printf("子进程一成功阻塞型写入：%zd KB\n", written / 1024);
    }

    free(buf);
    
    sem_post(sem2);  
    close(write_fd);
    sem_close(mutex);
    sem_close(sem2);
    exit(EXIT_SUCCESS);
}

// 子进程2：写入数据，带有超时控制
void child2_process(int write_fd, int kb) {
    sem_t *mutex = sem_open(MUTEX_SEM, 0);
    // sem_t *sem2 = sem_open(SEM_CHILD2, 0);
    sem_t *sem3 = sem_open(SEM_CHILD3, 0);
    sem_t *sem_parent_inter = sem_open(SEM_PARENT_READ_INTER, 0);  // 新增信号量

    // sem_wait(sem2);  // 等待子进程一完成
    
    int size = kb * 1024;
    char *buf = malloc(size);
    memset(buf, 'B', size);

    signal(SIGALRM, alarm_handler);  // 设置超时信号处理
    alarm(5);  // 设置5秒超时

    sem_wait(mutex);
    ssize_t written = write(write_fd, buf, size);
    sem_post(mutex);
    
    alarm(0);  // 取消超时计时器

    if (timeout) {
        int available;
        ioctl(write_fd, FIONREAD, &available);
        printf("子进程二: 阻塞型写入超时，已停止程序。子进程二成功写入 %zd KB, 当前管道中有 %d KB数据\n", written / 1024,available / 1024);
        exit(EXIT_FAILURE); 
    } else {
        printf("子进程二成功阻塞型写入：%zd KB\n", written / 1024);
    }

    free(buf);
    
    sem_post(sem_parent_inter);  // 通知父进程可以进行中间读取
    sem_post(sem3);  // 唤醒子进程三
    close(write_fd);
    sem_close(mutex);
    // sem_close(sem2);
    sem_close(sem3);
    sem_close(sem_parent_inter);
    exit(EXIT_SUCCESS);
}

// 子进程3：非阻塞读取所有数据后再非阻塞写入逐渐填满管道
void child3_process(int write_fd) {
    sem_t *mutex = sem_open(MUTEX_SEM, 0);
    sem_t *sem2 = sem_open(SEM_CHILD2, 0);
    sem_t *sem3 = sem_open(SEM_CHILD3, 0);
    sem_t *sem_parent = sem_open(SEM_PARENT_READ, 0);  // 新增信号量
    sem_t *sem_parent_inter = sem_open(SEM_PARENT_READ_INTER, 0);  // 新增信号量

    // sem_wait(sem3);  // 等待子进程二完成
    
    // sem_wait(sem2); 

    sem_wait(sem_parent_inter);
    
    
    
    
    // 设置非阻塞模式
    int flags = fcntl(write_fd, F_GETFL);
    fcntl(write_fd, F_SETFL, flags | O_NONBLOCK);

    int count = 0;
    char byte = 'C';
    while (1) {
        sem_wait(mutex);
        ssize_t written = write(write_fd, &byte, 1);
        sem_post(mutex);

        if (written == 1) {
            count++;
        } else if (errno == EAGAIN) {
            break;
        } else {
            perror("write");
            break;
        }
    }

    int total_written_kb = (count / 1024);
    
    printf("子进程三成功非阻塞型写入：%d KB，推测为管道默认大小\n", count / 1024);
        
    sem_post(sem_parent);  // 通知父进程可以读取
    close(write_fd);
    sem_close(mutex);
    sem_close(sem2);
    sem_close(sem3);
    sem_close(sem_parent);
    sem_close(sem_parent_inter);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s child1_kb child2_kb parent_read1_kb parent_read2_kb\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 创建并初始化信号量
    sem_unlink(MUTEX_SEM);
    sem_unlink(SEM_CHILD2);
    sem_unlink(SEM_CHILD3);
    sem_unlink(SEM_PARENT_READ);
    sem_unlink(SEM_PARENT_READ_INTER);  // 新增信号量
    
    sem_t *mutex = sem_open(MUTEX_SEM, O_CREAT | O_EXCL, 0666, 1);
    sem_t *sem2 = sem_open(SEM_CHILD2, O_CREAT | O_EXCL, 0666, 0);
    sem_t *sem3 = sem_open(SEM_CHILD3, O_CREAT | O_EXCL, 0666, 0);
    sem_t *sem_parent = sem_open(SEM_PARENT_READ, O_CREAT | O_EXCL, 0666, 0);
    sem_t *sem_parent_inter = sem_open(SEM_PARENT_READ_INTER, O_CREAT | O_EXCL, 0666, 0);  // 新增信号量

    // 创建管道
    int pipefd[2];
    if (pipe(pipefd)) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // 创建子进程
    pid_t pids[3];
    if ((pids[0] = fork()) == 0) child1_process(pipefd[1], atoi(argv[1]));
    if ((pids[1] = fork()) == 0) child2_process(pipefd[1], atoi(argv[2]));
    if ((pids[2] = fork()) == 0) child3_process(pipefd[1]);

    // 父进程处理
    close(pipefd[1]);  // 关闭写端

    sem_wait(sem3);
    sem_wait(sem2); 


    
    // 中间读取：读取子进程1和2写入的所有数据
    int available;
    ioctl(pipefd[0], FIONREAD, &available);
    printf("准备进行中间读取，管道中有 %d KB数据\n", available / 1024);
    
    char *inter_buf = malloc(available);
    sem_wait(mutex);
    ssize_t inter_read = read(pipefd[0], inter_buf, available);
    sem_post(mutex);
    
    printf("父进程中间读取完成：%zd KB\n", inter_read / 1024);
    free(inter_buf);

    // 等待子进程1和2完成后的中间读取
    sem_post(sem_parent_inter);  // 等待子进程2通知可以进行中间读取


    // 等待子进程3完成写入后的最终读取
    sem_wait(sem_parent);  // 等待子进程3通知可以读取

    // 第一次读取，带超时控制
    signal(SIGALRM, alarm_handler);
    timeout = 0;
    alarm(5);  // 设置5秒超时
    
    char *buf1 = malloc(atoi(argv[3]) * 1024);
    // sleep(6000);
    sem_wait(mutex);
    ssize_t read1 = read(pipefd[0], buf1, atoi(argv[3]) * 1024);
    sem_post(mutex);
    
    alarm(0);  // 取消超时计时器
    
    if (timeout) {
        printf("父进程第一次阻塞型读取超时，已停止程序。成功读取 %zd KB\n", read1 / 1024);
        free(buf1);
        close(pipefd[0]);
        sem_unlink(MUTEX_SEM);
        sem_unlink(SEM_CHILD2);
        sem_close(mutex);
        sem_close(sem2);
        sem_close(sem3);
        sem_close(sem_parent);
        sem_close(sem_parent_inter);
        exit(EXIT_FAILURE);
    } else {
        printf("父进程第一次阻塞型读取：%zd KB\n", read1 / 1024);
    }

    // 第二次读取，带超时控制
    signal(SIGALRM, alarm_handler);
    timeout = 0;
    alarm(5);  // 设置5秒超时
    // sleep(6000);
    char *buf2 = malloc(atoi(argv[4]) * 1024);
    sem_wait(mutex);
    ssize_t read2 = read(pipefd[0], buf2, atoi(argv[4]) * 1024);
    sem_post(mutex);
    
    alarm(0);  // 取消超时计时器
    
    if (timeout) {
        printf("父进程第二次阻塞型读取超时，已停止程序。成功读取 %zd KB\n", read2 / 1024);
        free(buf1);
        free(buf2);
        close(pipefd[0]);
        sem_unlink(MUTEX_SEM);
        sem_unlink(SEM_CHILD2);
        sem_unlink(SEM_CHILD3);
        sem_unlink(SEM_PARENT_READ);
        sem_unlink(SEM_PARENT_READ_INTER);
        sem_close(mutex);
        sem_close(sem2);
        sem_close(sem3);
        sem_close(sem_parent);
        sem_close(sem_parent_inter);
        exit(EXIT_FAILURE);
    } else {
        printf("父进程第二次阻塞型读取：%zd KB\n", read2 / 1024);
    }

    // 清理资源
    free(buf1);
    free(buf2);
    close(pipefd[0]);
    sem_unlink(MUTEX_SEM);
    sem_unlink(SEM_CHILD2);
    sem_unlink(SEM_CHILD3);
    sem_unlink(SEM_PARENT_READ);
    sem_unlink(SEM_PARENT_READ_INTER);
    sem_close(mutex);
    sem_close(sem2);
    sem_close(sem3);
    sem_close(sem_parent);
    sem_close(sem_parent_inter);

    return 0;
}


// 编译命令：
// gcc      program_NEW.c -o program_NEW -lpthread

// 运行 ：./program 参数1 参数2 参数3 参数4
// 参数1：子进程1写入的字节数（单位KB）
// 参数2：子进程2写入的字节数（单位KB）
// 参数3：父进程第一次读取的字节数（单位KB）
// 参数4：父进程第二次读取的字节数（单位KB）
// 编译：gcc program.c  -o program -lpthread
