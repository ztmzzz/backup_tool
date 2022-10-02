//
// Created by ztmzzz on 22-10-1.
//
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <unistd.h>
#include "file_info.h"
#include <map>

using namespace std;

void write_file_info(const string &path, int fd) {
    auto file_stat = new struct stat;
    if (lstat(path.c_str(), file_stat) == -1) {
        perror("lstat");
        return;
    }
    file_info file(file_stat, path);
    write(fd, &file, sizeof(file_info) - sizeof(string));
    write(fd, file.absolute_path.c_str(), file.absolute_path.length());
}

void write_file(const string &path, int fd) {
    int src_fd = open(path.c_str(), O_RDONLY);
    char buffer[1024];
    ssize_t len;
    while ((len = read(src_fd, buffer, 1024)) > 0) {
        write(fd, buffer, len);
    }
    close(src_fd);
}

map<ino_t, string> inode_map;

void backup(const string &src, int dst_fd) {
    DIR *src_dir = opendir(src.c_str());
    if (src_dir == nullptr) {
        return;
    }
    struct dirent *src_dirent;
    while ((src_dirent = readdir(src_dir)) != nullptr) {
        string new_src = src + "/" + src_dirent->d_name;
        if (src_dirent->d_type == DT_DIR) {
            if (strcmp(src_dirent->d_name, ".") == 0 || strcmp(src_dirent->d_name, "..") == 0) {
                continue;
            }
            write_file_info(new_src, dst_fd);
            backup(new_src, dst_fd);
        } else if (src_dirent->d_type == DT_LNK) {
            auto file_stat = new struct stat;
            if (lstat(new_src.c_str(), file_stat) == -1) {
                perror("lstat");
                return;
            }
            file_info file(file_stat, new_src);
            char buffer[4096];
            ssize_t len = readlink(new_src.c_str(), buffer, 4096);
            file.file_meta.size = len;
            write(dst_fd, &file, sizeof(file_info) - sizeof(string));
            write(dst_fd, file.absolute_path.c_str(), file.absolute_path.length());
            write(dst_fd, buffer, len);
        } else if (src_dirent->d_type == DT_FIFO) {
            write_file_info(new_src, dst_fd);
        } else {
            //普通文件
            auto file_stat = new struct stat;
            if (lstat(new_src.c_str(), file_stat) == -1) {
                perror("lstat");
                return;
            }
            file_info file(file_stat, new_src);
            if (file.file_meta.nlink == 1) {
                write(dst_fd, &file, sizeof(file_info) - sizeof(string));
                write(dst_fd, file.absolute_path.c_str(), file.absolute_path.length());
                write_file(new_src, dst_fd);
                continue;
            }
            if (inode_map.count(file.file_meta.inode) == 0) {
                //没有备份过
                inode_map[file.file_meta.inode] = new_src;
                write(dst_fd, &file, sizeof(file_info) - sizeof(string));
                write(dst_fd, file.absolute_path.c_str(), file.absolute_path.length());
                write_file(new_src, dst_fd);
            } else {
                //已经备份过,文件内容改为路径
                file.file_meta.size = inode_map[file.file_meta.inode].length();
                write(dst_fd, &file, sizeof(file_info) - sizeof(string));
                write(dst_fd, file.absolute_path.c_str(), file.absolute_path.length());
                write(dst_fd, inode_map[file.file_meta.inode].c_str(), inode_map[file.file_meta.inode].length());
            }
        }
    }
    closedir(src_dir);
}


//int main() {
//    int dst_fd = open("/home/ztm/2/backup", O_WRONLY | O_APPEND | O_CREAT | O_TRUNC, 0644);
//    backup("/home/ztm/1", dst_fd);
//    close(dst_fd);
//}