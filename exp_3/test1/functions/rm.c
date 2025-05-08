#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

void handle_rm(const char *path) {
    struct stat path_stat;
    if (lstat(path, &path_stat) != 0) {
        perror("rm failed");
        return;
    }

    if (S_ISDIR(path_stat.st_mode)) {
        DIR *dir = opendir(path);
        if (!dir) {
            perror("opendir failed");
            return;
        }

        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);
            // handle_rm(full_path);  // 递归删除子项
        }
        closedir(dir);

        if (rmdir(path) != 0) {
            perror("rmdir failed");
        }
    } else {
        // 删除文件
        if (remove(path) != 0) {
            perror("remove failed");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: rm <path>\n");
        return 1;
    }
    handle_rm(argv[1]);
    return 0;
}