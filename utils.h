//
// Created by ztm on 22-10-2.
//

#ifndef BACKUP_TOOL_UTILS_H
#define BACKUP_TOOL_UTILS_H

#endif //BACKUP_TOOL_UTILS_H

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <set>
#include "file_info.h"
#include <utime.h>
#include <sys/types.h>

void mkdir_recursive(const string &path) {
    int len = path.length();
    char p[len];
    strcpy(p, path.c_str());
    for (int i = 0; i < len; i++) {
        if (p[i] == '/') {
            p[i] = '\0';
            if (access(p, F_OK) != 0) {
                mkdir(p, 0775);
            }
            p[i] = '/';
        }
    }
    if (access(p, F_OK) != 0) {
        mkdir(p, 0775);
    }
}
