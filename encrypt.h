//
// Created by ztm on 22-10-3.
//

#ifndef BACKUP_TOOL_ENCRYPT_H
#define BACKUP_TOOL_ENCRYPT_H

#include <iostream>
#include <string>
#include <cstring>


using namespace std;


class encrypt {
private:
    unsigned char S[256];

public:
    encrypt(const string &key) {
        unsigned char T[256];
        for (int i = 0; i < 256; i++) {
            S[i] = i;
            T[i] = key[i % key.length()];
        }
        for (int i = 0; i < 256; i++) {
            int j = (j + S[i] + T[i]) % 256;
            auto tmp = S[i];
            S[i] = S[j];
            S[j] = tmp;
        }
    }

    void encrypt_data(unsigned char *data, unsigned long long len) {
        int i = 0, j = 0, t;
        unsigned char S_bak[256];
        memcpy(S_bak, S, 256);
        for (unsigned long long k = 0; k < len; k++) {
            i = (i + 1) % 256;
            j = (j + S_bak[i]) % 256;
            auto tmp = S_bak[i];
            S_bak[i] = S_bak[j];
            S_bak[j] = tmp;
            t = (S_bak[i] + S_bak[j]) % 256;
            data[k] ^= S_bak[t];
        }
    }
};

#endif //BACKUP_TOOL_ENCRYPT_H
