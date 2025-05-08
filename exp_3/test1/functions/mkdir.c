#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: mkdir <directory> [mode]\n");
        return 1;
    }

    char *dir_name = argv[1];
    mode_t mode = 0755;  // 默认权限

    if (argc > 2) {
        // 转换权限字符串为八进制数
        char *endptr;
        mode = strtol(argv[2], &endptr, 8);
        if (*endptr != '\0' || mode > 0777) {
            printf("Invalid mode: %s\n", argv[2]);
            return 1;
        }
    }

    // 创建目录
    if (mkdir(dir_name, mode) != 0) {
        if (errno == EEXIST) {
            printf("Error: Directory '%s' already exists\n", dir_name);
        } else {
            perror("mkdir failed");
        }
        return 1;
    }
    return 0;
}