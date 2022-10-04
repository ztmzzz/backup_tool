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
#include <sys/types.h>
#include <sys/time.h>
#include "utils.h"

set<ino_t> inode_set;

void set_meta(const string &temp_path, const meta &temp_meta);

void restore_file(const string &path, const string &dst) {
    mkdir_recursive(dst);
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
        string temp_path = dst + "/" + temp.relative_path;
        mode_t temp_mode = temp_meta.mode;
        if (S_ISREG(temp_mode)) {
            //普通文件
            char buff[temp_meta.size];
            read(src_fd, buff, temp_meta.size);
            if (temp_meta.nlink > 1) {
                if (inode_set.find(temp_meta.inode) != inode_set.end()) {
                    //文件已经存在
                    string link_path = dst + "/" + buff;
                    link(link_path.c_str(), temp_path.c_str());
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
            set_meta(temp_path, temp_meta);
            close(dst_fd);
        } else if (S_ISDIR(temp_mode)) {
            //目录
            mkdir(temp_path.c_str(), temp_mode);
            set_meta(temp_path, temp_meta);
        } else if (S_ISFIFO(temp_mode)) {
            //管道
            mkfifo(temp_path.c_str(), temp_mode);
            set_meta(temp_path, temp_meta);
        } else if (S_ISLNK(temp_mode)) {
            //软链接
            char buffer[temp_meta.size + 1];
            read(src_fd, buffer, temp_meta.size);
            buffer[temp_meta.size] = '\0';
            symlink(buffer, temp_path.c_str());
            set_meta(temp_path, temp_meta);
        }
    }

}

void set_meta(const string &temp_path, const meta &temp_meta) {
    //由于linux的unmask,需要调用chmod
    chmod(temp_path.c_str(), temp_meta.mode);
    chown(temp_path.c_str(), temp_meta.uid, temp_meta.gid);
    timeval temp_timeval[2];
    temp_timeval[0].tv_sec = temp_meta.atime.tv_sec;
    temp_timeval[0].tv_usec = temp_meta.atime.tv_nsec / 1000;
    temp_timeval[1].tv_sec = temp_meta.mtime.tv_sec;
    temp_timeval[1].tv_usec = temp_meta.mtime.tv_nsec / 1000;
    lutimes(temp_path.c_str(), temp_timeval);
}

//int main() {
//    restore("/home/ztm/2/backup", "/home/ztm/3");
//}