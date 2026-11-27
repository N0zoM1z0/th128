#include "ZeroCallMedium10.hpp"

#include <math.h>
#include <stdlib.h>
#include <string.h>

unsigned char *__stdcall Zc10DecryptBlocks(unsigned char key, unsigned char *data, int size, unsigned char step, int blockSize, int limit) {
    int rem = size % blockSize;
    int skip = (rem < blockSize / 4) ? rem : 0;
    int allocSize = (limit <= size) ? limit : size;
    unsigned char *tmp = (unsigned char *)malloc(allocSize);
    if (tmp == 0) {
        return data;
    }
    memcpy(tmp, data, allocSize);
    int remaining = size - ((size & 1) + skip);
    unsigned char *src = tmp;
    unsigned char *dst = data;
    while (remaining > 0 && limit > 0) {
        if (remaining < blockSize) {
            blockSize = remaining;
        }
        unsigned char *out = dst + blockSize - 1;
        int n1 = (blockSize + 1) / 2;
        while (n1 > 0) {
            *out = *src ^ key;
            key += step;
            --n1;
            out -= 2;
            ++src;
        }
        out = dst + blockSize - 2;
        int n2 = blockSize / 2;
        while (n2 > 0) {
            *out = *src ^ key;
            key += step;
            --n2;
            out -= 2;
            ++src;
        }
        limit -= blockSize;
        remaining -= blockSize;
        dst += blockSize;
    }
    free(tmp);
    return data;
}

unsigned char *__stdcall Zc10EncryptBlocks(unsigned char key, unsigned char *data, int size, unsigned char step, int blockSize, int limit) {
    int rem = size % blockSize;
    int skip = (rem < blockSize / 4) ? rem : 0;
    int allocSize = (limit <= size) ? limit : size;
    unsigned char *tmp = (unsigned char *)malloc(allocSize);
    if (tmp == 0) {
        return data;
    }
    memcpy(tmp, data, allocSize);
    int remaining = size - ((size & 1) + skip);
    unsigned char *srcBase = tmp;
    unsigned char *dst = data;
    while (remaining > 0 && limit > 0) {
        if (remaining < blockSize) {
            blockSize = remaining;
        }
        unsigned char *in = srcBase + blockSize - 1;
        int n1 = (blockSize + 1) / 2;
        while (n1 > 0) {
            *dst = *in ^ key;
            key += step;
            --n1;
            in -= 2;
            ++dst;
        }
        in = srcBase + blockSize - 2;
        int n2 = blockSize / 2;
        while (n2 > 0) {
            *dst = *in ^ key;
            key += step;
            --n2;
            in -= 2;
            ++dst;
        }
        limit -= blockSize;
        remaining -= blockSize;
        srcBase += blockSize;
    }
    free(tmp);
    return data;
}

int Zc10BuildRibbonU(int count, Zc10AnmVm *vm, Zc10Vertex28 *verts) {
    if (count < 3) {
        return -1;
    }
    float v0;
    float start;
    float step;
    start = vm->f68 + vm->f98;
    int segs = (count + 1) / 2 - 1;
    v0 = vm->f84 + vm->f6c;
    float span = vm->f98 - vm->f88;
    step = span / segs;
    float cur = start;
    for (int i = 0; i < count; i += 2) {
        verts[i].u = cur;
        verts[i].v = v0;
        verts[i].color = vm->color;
        verts[i].rhw = 1.0f;
        cur -= step;
    }
    v0 = vm->f8c + vm->f6c;
    cur = start;
    for (int i = 1; i < count; i += 2) {
        verts[i].u = cur;
        verts[i].v = v0;
        verts[i].color = vm->color;
        verts[i].rhw = 1.0f;
        cur -= step;
    }
    return 0;
}

int Zc10BuildRibbonV(int count, Zc10AnmVm *vm, Zc10Vertex28 *verts) {
    if (count < 3) {
        return -1;
    }
    float u0;
    float start;
    float step;
    start = vm->f6c + vm->f98;
    int segs = (count + 1) / 2 - 1;
    u0 = vm->f84 + vm->f68;
    float span = vm->f98 - vm->f88;
    step = span / segs;
    float cur = start;
    for (int i = 0; i < count; i += 2) {
        verts[i].v = cur;
        verts[i].u = u0;
        verts[i].color = vm->color;
        verts[i].rhw = 1.0f;
        cur -= step;
    }
    u0 = vm->f8c + vm->f68;
    cur = start;
    for (int i = 1; i < count; i += 2) {
        verts[i].v = cur;
        verts[i].u = u0;
        verts[i].color = vm->color;
        verts[i].rhw = 1.0f;
        cur -= step;
    }
    return 0;
}

int Zc10Laser::CheckHitA(const Zc10Float2 *pos, float radius) {
    float dx = pos->x - posX;
    float dy = pos->y - posY;
    float negAngle = -angle;
    float s = (float)sin(negAngle);
    float c = (float)cos(negAngle);
    float rx = dx * c - dy * s;
    float ry = dx * s + dy * c;
    float minX = rx - radius;
    float minY = ry - radius;
    float maxX = rx + radius;
    float maxY = ry + radius;
    if (minX <= length && minY <= width * 0.5 && maxX >= 0.0f && maxY >= -width * 0.5) {
        return 2;
    }
    return 0;
}

void __stdcall Zc10KillEnemiesInBox(Zc10EnemyMgr *mgr, const Zc10Float2 *pos, const Zc10Float2 *size) {
    for (Zc10EnemyNode *node = mgr->head; node != 0;) {
        Zc10Enemy *e = node->enemy;
        Zc10EnemyNode *next = node->next;
        if ((e->flags & 0x800) != 0) {
            if (e->posX < pos->x - size->x * 0.5 &&
                e->posX > pos->x + size->x * 0.5 &&
                e->posY < pos->y - size->y * 0.5 &&
                e->posY > pos->y + size->y * 0.5) {
                if ((e->f5018 & 1) != 0) {
                    e->f5010 -= 99999;
                    e->hp = e->f5014 + (e->f5010 - e->f5014 * 7) / 7;
                } else {
                    e->hp -= 99999;
                }
            }
        }
        node = next;
    }
}
