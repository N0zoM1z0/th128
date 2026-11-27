#include "ZeroCallMedium9.hpp"

#include <string.h>
#include <time.h>
#include <windows.h>

int g_Zc9Difficulty;
int g_Zc9Score;
unsigned char g_Zc9Stage;
unsigned char g_Zc9Route;
Zc9SlowRateObj *g_Zc9SlowRateObj;
void *g_Zc9SoundThread1;
void *g_Zc9SoundThread2;
int g_Zc9SoundStopFlag;
signed char g_Zc9ConfigBgmVol;
signed char g_Zc9ConfigBgmOn;

int __stdcall Zc9InsertScoreEntry(Zc9ScoreTable *table) {
    int i;
    for (i = 0; i < 10; ++i) {
        if (table->entries[g_Zc9Difficulty][i].score <= g_Zc9Score) {
            break;
        }
    }
    if (i >= 10) {
        return -1;
    }
    for (int j = 9; j > i; --j) {
        table->entries[g_Zc9Difficulty][j] = table->entries[g_Zc9Difficulty][j - 1];
    }
    table->entries[g_Zc9Difficulty][i].score = g_Zc9Score;
    table->entries[g_Zc9Difficulty][i].stage = g_Zc9Stage;
    table->entries[g_Zc9Difficulty][i].route = g_Zc9Route;
    _time64(&table->entries[g_Zc9Difficulty][i].timestamp);
    strcpy(table->entries[g_Zc9Difficulty][i].name, "        ");
    table->entries[g_Zc9Difficulty][i].slowRate =
        (float)(100.0 - (float)(g_Zc9SlowRateObj->f24 / g_Zc9SlowRateObj->f2c) * 100.0);
    return i;
}

void __stdcall Zc9IceBitmapClear(const Zc9Float2 *pos, Zc9IceField *field, const unsigned int *mask, int height, int width) {
    int shift = (int)(pos->x + 384.0 - (width << 4)) % 32;
    int startX = (int)((pos->x + 192.0 - width * 16.0) * 0.03125);
    int y = (int)(pos->y - height * 0.5);
    int rowOffset = 0;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int x = startX + col;
            if (x >= 0 && x < 12 && y >= 0 && y < 448) {
                field->bitmap[y][x] &= ~(mask[rowOffset + col] >> shift);
            }
            if (x + 1 >= 0 && x + 1 < 12 && y >= 0 && y < 448 && shift > 0) {
                field->bitmap[y][x + 1] &= ~(mask[rowOffset + col] << (32 - shift));
            }
        }
        ++y;
        rowOffset += width;
    }
}

void __stdcall Zc9IceBitmapOr(const Zc9Float2 *pos, Zc9IceField *field, const unsigned int *mask, int height, int width) {
    int shift = (int)(pos->x + 384.0 - (width << 4)) % 32;
    int startX = (int)((pos->x + 192.0 - width * 16.0) * 0.03125);
    int y = (int)(pos->y - height * 0.5);
    int rowOffset = 0;
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            int x = startX + col;
            if (x >= 0 && x < 12 && y >= 0 && y < 448) {
                field->bitmap[y][x] |= (mask[rowOffset + col] >> shift);
            }
            if (x + 1 >= 0 && x + 1 < 12 && y >= 0 && y < 448 && shift > 0) {
                field->bitmap[y][x + 1] |= (mask[rowOffset + col] << (32 - shift));
            }
        }
        ++y;
        rowOffset += width;
    }
}

int __stdcall Zc9MarkEnemiesInBox(const Zc9Float2 *pos, const Zc9Float2 *size, Zc9EnemyMgr *mgr) {
    int count = 0;
    Zc9Float3 boxMin;
    Zc9Float3 boxMax;
    boxMin.x = pos->x - (float)(size->x * 0.5);
    boxMin.y = pos->y - (float)(size->y * 0.5);
    boxMax.x = pos->x + (float)(size->x * 0.5);
    boxMax.y = pos->y + (float)(size->y * 0.5);
    for (Zc9EnemyNode *node = mgr->head; node != 0;) {
        Zc9Enemy *e = node->enemy;
        Zc9EnemyNode *next = node->next;
        unsigned int flags = e->flags;
        if (((flags & 0xa0) == 0 && (flags & 0x1802400) == 0) || (flags & 0x100) != 0) {
            float eMinX = (float)(e->posX - e->hitW * 0.5);
            float eMinY = (float)(e->posY - e->hitH * 0.5);
            float eMaxX = (float)(e->posX + e->hitW * 0.5);
            float eMaxY = (float)(e->posY + e->hitH * 0.5);
            if (eMaxX >= boxMin.x && eMinX <= boxMax.x && eMaxY >= boxMin.y && eMinY <= boxMax.y) {
                e->flags = flags | 0x4000000;
                ++count;
            }
        }
        node = next;
    }
    return count;
}

int Zc9SoundResetAndVolume(Zc9SoundMgr *mgr) {
    for (int i = 0; i < 12; ++i) {
        mgr->slots[i] = -1;
    }
    if (g_Zc9SoundThread1 != 0) {
        if (g_Zc9SoundStopFlag == 0) {
            g_Zc9SoundStopFlag = 1;
        }
        while (WaitForSingleObject(g_Zc9SoundThread1, 100) == WAIT_TIMEOUT) {
            Sleep(1);
        }
        while (WaitForSingleObject(g_Zc9SoundThread2, 100) == WAIT_TIMEOUT) {
            Sleep(1);
        }
        CloseHandle(g_Zc9SoundThread1);
        CloseHandle(g_Zc9SoundThread2);
        g_Zc9SoundThread1 = 0;
        g_Zc9SoundThread2 = 0;
    }
    if (mgr->primaryBuf == 0) {
        return -1;
    }
    if (mgr->dsound != 0) {
        mgr->bgmVol = g_Zc9ConfigBgmVol;
        mgr->bgmEnabled = g_Zc9ConfigBgmOn;
        if (mgr->bgmEnabled != 0) {
            float f = (float)(mgr->bgmVol / 100.0);
            f = 1.0f - f;
            f = f * f;
            f = f * f;
            f = 1.0f - f;
            mgr->bgmAtten = -5000 - (int)(f * -5000.0);
        } else {
            mgr->bgmAtten = -10000;
        }
    }
    return 0;
}

void Zc9AnmVmReset(Zc9AnmVm *vm) {
    Zc9Float3 savedPos = vm->pos;
    int savedId = vm->id;
    memset(vm, 0, sizeof(Zc9AnmVm));
    vm->scaleX = 1.0f;
    vm->scaleY = 1.0f;
    vm->scale2X = 1.0f;
    vm->scale2Y = 1.0f;
    vm->uvScaleX = 1.0f;
    vm->uvScaleY = 1.0f;
    vm->pos = savedPos;
    vm->id = savedId;
    vm->f3f0 = -1;
    vm->mat[3][2] = 0.0f;
    vm->mat[3][1] = 0.0f;
    vm->mat[3][0] = 0.0f;
    vm->mat[2][3] = 0.0f;
    vm->mat[2][1] = 0.0f;
    vm->mat[2][0] = 0.0f;
    vm->mat[1][3] = 0.0f;
    vm->mat[1][2] = 0.0f;
    vm->mat[1][0] = 0.0f;
    vm->mat[0][3] = 0.0f;
    vm->mat[0][2] = 0.0f;
    vm->mat[0][1] = 0.0f;
    vm->mat[3][3] = 1.0f;
    vm->mat[2][2] = 1.0f;
    vm->mat[1][1] = 1.0f;
    vm->mat[0][0] = 1.0f;
    vm->flags0 = 7;
    vm->curTime = 0;
    vm->curTimeF = 0.0f;
    vm->prevTime = -999999;
    vm->interp0 = 0;
    vm->interp1 = 0;
    vm->interp2 = 0;
    vm->interp3 = 0;
    vm->interp4 = 0;
    vm->interp5 = 0;
    vm->interp6 = 0;
    vm->interp7 = 0;
    vm->interp8 = 0;
    vm->interp9 = 0;
    vm->flags2 = (vm->flags2 & ~0x80u) | 0x40u;
    vm->node1.next = 0;
    vm->node1.prev = 0;
    vm->node1.owner = vm;
    vm->node2.next = 0;
    vm->node2.prev = 0;
    vm->node2.owner = vm;
}
