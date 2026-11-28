#include "EngineWave1A.hpp"

#include <stdlib.h>
#include <string.h>

namespace th128 {

Ew1aAnmVm *Ew1aAllocateAnmVm() {
    Ew1aAnmManager *mgr = g_Ew1aAnmManager;
    int idx = mgr->nextPoolIdx;
    Ew1aAnmVm *vm = &mgr->pool[idx];
    if (mgr->poolUsed[idx] == 0) {
        mgr->poolUsed[idx] = 1;
        mgr->nextPoolIdx = (mgr->nextPoolIdx + 1) % 4096;
        return vm;
    }
    idx = (idx + 1) % 4096;
    mgr->nextPoolIdx = idx;
    vm = &mgr->pool[idx];
    if (mgr->poolUsed[idx] != 0) {
        vm = new Ew1aAnmVm();
        vm->ResetForReuse();
        return vm;
    }
    vm->ResetForReuse();
    mgr->poolUsed[mgr->nextPoolIdx] = 1;
    return vm;
}

Ew1aChain::Ew1aChain() {
    f4c = 0;
}

unsigned char *Ew1aDecryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int blockSize, int limit) {
    int rem = size % blockSize;
    rem = (rem < blockSize / 4) ? rem : 0;
    int copySize = (limit > size) ? size : limit;
    unsigned char *tmp = (unsigned char *)malloc(copySize);
    if (tmp == NULL) {
        return data;
    }
    size -= (size & 1) + rem;
    memcpy(tmp, data, copySize);

    unsigned char *src = tmp;
    unsigned char *dst = data;
    while (size > 0 && limit > 0) {
        if (size < blockSize) {
            blockSize = size;
        }
        unsigned char *blockEnd = dst + blockSize;
        unsigned char *p = blockEnd - 1;
        for (int i = (blockSize + 1) / 2; i > 0; i--) {
            *p = *src ^ key;
            key += step;
            p -= 2;
            src++;
        }
        p = blockEnd - 2;
        for (int i = blockSize / 2; i > 0; i--) {
            *p = *src ^ key;
            key += step;
            p -= 2;
            src++;
        }
        limit -= blockSize;
        size -= blockSize;
        dst = blockEnd;
    }
    free(tmp);
    return data;
}

unsigned char *Ew1aEncryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int blockSize, int limit) {
    int rem = size % blockSize;
    rem = (rem < blockSize / 4) ? rem : 0;
    int copySize = (limit > size) ? size : limit;
    unsigned char *tmp = (unsigned char *)malloc(copySize);
    if (tmp == NULL) {
        return data;
    }
    size -= (size & 1) + rem;
    memcpy(tmp, data, copySize);

    unsigned char *src = tmp;
    unsigned char *dst = data;
    while (size > 0 && limit > 0) {
        if (size < blockSize) {
            blockSize = size;
        }
        unsigned char *blockEnd = src + blockSize;
        unsigned char *p = blockEnd - 1;
        for (int i = (blockSize + 1) / 2; i > 0; i--) {
            *dst = *p ^ key;
            key += step;
            p -= 2;
            dst++;
        }
        p = blockEnd - 2;
        for (int i = blockSize / 2; i > 0; i--) {
            *dst = *p ^ key;
            key += step;
            p -= 2;
            dst++;
        }
        limit -= blockSize;
        size -= blockSize;
        src = blockEnd;
    }
    free(tmp);
    return data;
}

unsigned int Ew1aRng::GetU32AndSkip2() {
    union {
        unsigned int u32[2];
        unsigned short u16[4];
    } v;
    v.u16[0] = GetU16();
    v.u16[1] = GetU16();
    v.u16[2] = GetU16();
    v.u16[3] = GetU16();
    return v.u32[0];
}

const char *Ew1aReadShiftJisLine(const char *src, char *dst, int maxLen) {
    memset(dst, 0, maxLen);
    while (*src != '\n' && *src != '\r' && maxLen != 0) {
        *dst++ = *src;
        unsigned char c = (unsigned char)*src;
        if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xfc)) {
            src++;
            maxLen--;
            *dst++ = *src;
        }
        src++;
        maxLen--;
    }
    while (*src == '\n' || *src == '\r') {
        src++;
    }
    return src;
}

unsigned char __fastcall Ew1aSumBytes(int len, const unsigned char *buf) {
    unsigned char sum = 0;
    while (len != 0) {
        sum += *buf++;
        len--;
    }
    return sum;
}

} // namespace th128
