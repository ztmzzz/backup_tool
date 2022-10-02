//
// Created by ztm on 22-10-1.
//

#ifndef BACKUP_TOOL_FILE_INFO_H
#define BACKUP_TOOL_FILE_INFO_H

#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

using namespace std;

struct meta {
    uid_t uid = 0;
    gid_t gid = 0;
    mode_t mode = 0;
    ino_t inode = 0;
    nlink_t nlink = 0;
    timespec atime = {};
    timespec mtime = {};
    off_t size = 0;
};

class file_info {
public:
    size_t size = 0;
    meta file_meta = {};
    string relative_path;

    file_info() = default;

    file_info(struct stat *file_stat, const string &relative_path) {
        this->relative_path = relative_path;
        this->file_meta.uid = file_stat->st_uid;
        this->file_meta.gid = file_stat->st_gid;
        this->file_meta.mode = file_stat->st_mode;
        this->file_meta.inode = file_stat->st_ino;
        this->file_meta.nlink = file_stat->st_nlink;
        this->file_meta.atime = file_stat->st_atim;
        this->file_meta.mtime = file_stat->st_mtim;
        this->file_meta.size = file_stat->st_size;
        this->size = sizeof(file_info) - sizeof(string) + relative_path.length();
    }

    static file_info read_from_binary(char *buffer, size_t size) {
        file_info file{};
        file.size = size;
        memcpy(&file.file_meta, buffer, sizeof(meta));
        file.relative_path = string(buffer + sizeof(meta),
                                    file.size - sizeof(size_t) - sizeof(meta));
        return file;
    }

};


#endif //BACKUP_TOOL_FILE_INFO_H
