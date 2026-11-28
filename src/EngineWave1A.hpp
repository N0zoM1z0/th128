#pragma once

#include <windows.h>

namespace th128 {

struct Ew1aAnmVm {
    unsigned char data[0x4e8];

    Ew1aAnmVm();
    void ResetForReuse();
};

struct Ew1aAnmManager {
    unsigned char pad00[0xbc];
    Ew1aAnmVm pool[4096];
    unsigned char poolUsed[4096];
    int nextPoolIdx;
};

struct Ew1aChainNode {
    int priority;
    unsigned int flags;
    void *callback;
    void *onRegister;
    void *onDelete;
    Ew1aChainNode *self;
    Ew1aChainNode *next;
    Ew1aChainNode *prev;
    void *arg;

    Ew1aChainNode() {
        flags &= ~1u;
        callback = NULL;
        onRegister = NULL;
        onDelete = NULL;
        priority = 0;
        self = this;
        next = NULL;
        prev = NULL;
    }
};

struct Ew1aChain {
    Ew1aChainNode calcHead;
    Ew1aChainNode drawHead;
    int f48;
    int f4c;

    Ew1aChain();
};

struct Ew1aRng {
    unsigned short GetU16();
    unsigned int GetU32AndSkip2();
};

extern Ew1aAnmManager *g_Ew1aAnmManager;

Ew1aAnmVm *Ew1aAllocateAnmVm();
unsigned char *Ew1aDecryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int blockSize, int limit);
unsigned char *Ew1aEncryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int blockSize, int limit);
const char *Ew1aReadShiftJisLine(const char *src, char *dst, int maxLen);
unsigned char __fastcall Ew1aSumBytes(int len, const unsigned char *buf);

} // namespace th128
