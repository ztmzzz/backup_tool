//
// Created by ztmzzz on 22-10-1.
//
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>

using namespace std;

void backup(string src, string dst) {
    mkdir(dst.c_str(), 0777);
    DIR *src_dir = opendir(src.c_str());
    struct dirent *src_dirent;
    src_dirent = readdir(src_dir);
    while (src_dirent != nullptr) {
        if (src_dirent->d_type == DT_DIR) {
            if (strcmp(src_dirent->d_name, ".") == 0 || strcmp(src_dirent->d_name, "..") == 0) {
                src_dirent = readdir(src_dir);
                continue;
            }
            string new_src = src + "/" + src_dirent->d_name;
            string new_dst = dst + "/" + src_dirent->d_name;
            mkdir(new_dst.c_str(), 0777);
            backup(new_src, new_dst);
        } else {
            string new_src = src + "/" + src_dirent->d_name;
            string new_dst = dst + "/" + src_dirent->d_name;
            int src_fd = open(new_src.c_str(), O_RDONLY);
            int dst_fd = open(new_dst.c_str(), O_WRONLY | O_CREAT, 0777);
            char buffer[1024];
            ssize_t len;
            while ((len = read(src_fd, buffer, 1024)) > 0) {
                write(dst_fd, buffer, len);
            }
            close(src_fd);
            close(dst_fd);
        }
        src_dirent = readdir(src_dir);
    }
    closedir(src_dir);

}


int main() {
    backup("/home/ztmzzz/1", "/home/ztmzzz/2");
}