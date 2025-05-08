#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT_LEN 1024
#define CMD_COLLECTION_LEN 3  // 内部命令数量

char *cmdList[] = {"exit", "ls", "cd"}; 

// 处理 cd 命令
void handle_cd(char *args) {
    if (!args) {
        printf("Usage: cd <directory>\n");
        return;
    }
    if (chdir(args) != 0) {
        perror("cd failed");
    }
}

int getCmdIndex(char *cmd) {
    for (int i = 0; i < CMD_COLLECTION_LEN; i++) {
        if (strcmp(cmd, cmdList[i]) == 0) return i;
    }
    return -1;
}

// 执行外部命令
void execute_external_command(char *cmd, char *args) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) { // 子进程
        // 构造参数数组
        char *argv[] = {cmd, args, NULL};

        // 执行外部命令
        execvp(cmd, argv);

        // 如果 execvp 失败
        printf("Command not found\n");
        // perror("execvp failed");
        exit(EXIT_FAILURE);
        
    } else { // 父进程
        wait(NULL); // 等待子进程完成
    }
}

void execute_command(int cmdIndex, char *cmd, char *args) {
    switch (cmdIndex) {
        case 1: // ls
            execute_external_command("/bin/ls", args);
            break;
        case 2: // cd
            handle_cd(args);
            break;
        default: {
            // 定义足够大的缓冲区
            char file_path[256]; // 根据需要调整大小
            // 使用 snprintf 安全拼接路径和命令
            snprintf(file_path, sizeof(file_path), "/home/os_exp/exp_3/test1/functions/%s", cmd);
            // printf("%s", file_path);
            execute_external_command(file_path, args);
            break;
        }
    }
}

int main() {
    char input[MAX_INPUT_LEN];

    while (1) {
        printf("---Input your command > ");
        if (!fgets(input, sizeof(input), stdin)) break;

        // 解析输入
        char *cmd = strtok(input, " \n");
        char *args = strtok(NULL, "\n");

        if (!cmd) continue;

        int cmdIndex = getCmdIndex(cmd);

        switch (cmdIndex) {
            case 0: // exit
                exit(EXIT_SUCCESS);
            default:
                execute_command(cmdIndex, cmd, args);
                break;
        }
    }
    return 0;
}