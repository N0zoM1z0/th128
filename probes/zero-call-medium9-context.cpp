#include "ZeroCallMedium9.hpp"

extern "C" int __stdcall ZeroCallMedium9Probe(
    Zc9ScoreTable *table,
    const Zc9Float2 *pos,
    const Zc9Float2 *size,
    Zc9IceField *field,
    const unsigned int *mask,
    int height,
    int width,
    Zc9EnemyMgr *mgr,
    Zc9SoundMgr *snd,
    Zc9AnmVm *vm,
    void **outPtrs,
    int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Zc9Difficulty = height;
        g_Zc9Score = width;
        g_Zc9Stage = (unsigned char)mode;
        g_Zc9Route = (unsigned char)(mode >> 8);
        g_Zc9SlowRateObj = (Zc9SlowRateObj *)table;
        g_Zc9SoundThread1 = (void *)field;
        g_Zc9SoundThread2 = (void *)mask;
        g_Zc9SoundStopFlag = height;
        g_Zc9ConfigBgmVol = (signed char)height;
        g_Zc9ConfigBgmOn = (signed char)width;
        outPtrs[0] = &g_Zc9Difficulty;
        outPtrs[1] = &g_Zc9Score;
        outPtrs[2] = &g_Zc9Stage;
        outPtrs[3] = &g_Zc9Route;
        outPtrs[4] = &g_Zc9SlowRateObj;
        outPtrs[5] = &g_Zc9ConfigBgmVol;
        outPtrs[6] = &g_Zc9ConfigBgmOn;
    }
    if (mode & 2) {
        sum += Zc9InsertScoreEntry(table);
    }
    if (mode & 4) {
        Zc9IceBitmapClear(pos, field, mask, height, width);
    }
    if (mode & 8) {
        Zc9IceBitmapOr(pos, field, mask, height, width);
    }
    if (mode & 16) {
        sum += Zc9MarkEnemiesInBox(pos, size, mgr);
    }
    if (mode & 32) {
        sum += Zc9SoundResetAndVolume(snd);
    }
    if (mode & 64) {
        Zc9AnmVmReset(vm);
    }
    return sum;
}
