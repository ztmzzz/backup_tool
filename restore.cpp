//
// Created by ztm on 22-10-2.
//
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <set>
#include "file_info.h"

set<ino_t> inode_set;

void restore(string path) {
    int src_fd = open(path.c_str(), O_RDONLY);
    if (src_fd == -1) {
        cout << "open file error" << endl;
        return;
    }

    size_t size;
    while (read(src_fd, &size, sizeof(size_t))) {
        char temp_buffer[size - sizeof(size_t)];
        read(src_fd, temp_buffer, size - sizeof(size_t));
        file_info temp = file_info::read_from_binary(temp_buffer, size);
        meta temp_meta = temp.file_meta;
        string temp_path = temp.absolute_path;
        mode_t temp_mode = temp_meta.mode;
        if (S_ISREG(temp_mode)) {
            //普通文件
            char buff[temp_meta.size + 1];
            read(src_fd, buff, temp_meta.size);
            if (temp_meta.nlink > 1) {
                if (inode_set.find(temp_meta.inode) != inode_set.end()) {
                    //文件已经存在
                    buff[temp_meta.size] = '\0';
                    link(buff, temp_path.c_str());
                    continue;
                } else {
                    inode_set.insert(temp_meta.inode);
                }
            }
            int dst_fd = open(temp_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, temp_mode);
            if (dst_fd == -1) {
                cout << "open file error" << endl;
                return;
            }
            write(dst_fd, buff, temp_meta.size);
            //由于linux的unmask,需要调用chmod
            chmod(temp_path.c_str(), temp_mode);
            close(dst_fd);
        } else if (S_ISDIR(temp_mode)) {
            //目录
            mkdir(temp_path.c_str(), temp_mode);
            chmod(temp_path.c_str(), temp_mode);
        } else if (S_ISFIFO(temp_mode)) {
            //管道
            mkfifo(temp_path.c_str(), temp_mode);
            chmod(temp_path.c_str(), temp_mode);
        } else if (S_ISLNK(temp_mode)) {
            //软链接
            char buffer[temp_meta.size + 1];
            read(src_fd, buffer, temp_meta.size);
            buffer[temp_meta.size] = '\0';
            symlink(buffer, temp_path.c_str());
            chmod(temp_path.c_str(), temp_mode);
        }
    }

}

int main() {
    restore("/home/ztm/2/backup");
}