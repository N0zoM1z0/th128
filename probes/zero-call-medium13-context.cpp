#include "ZeroCallMedium13.hpp"
#include <new>

extern "C" int __stdcall ZeroCallMedium13Probe(
    int *outA,
    int *outB,
    const Zc13RankObj *rankObj,
    float f0, float f1, float f2, float f3,
    Zc13Rng *rng,
    unsigned int cx,
    unsigned int cy,
    Zc13IceGrid *grid,
    const Zc13Float2 *pos,
    Zc13BossStruct *boss,
    Zc13InterpObj *interp,
    int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Zc13Gui = (Zc13GuiCounter *)boss;
        g_Zc13Diff = (int)cx;
        g_Zc13RankThresholds[0][0] = (int)cy;
        g_Zc13CritFlags = cx;
        g_Zc13CritDepth = (unsigned char)cy;
        g_Zc13TimeScale = f0;
        *(void **)outA = &g_Zc13CritFlags;
    }
    if (mode & 2) {
        Zc13DecodeScoreCounter(outA, outB);
    }
    if (mode & 4) {
        sum += Zc13GetRankIndex(rankObj);
    }
    if (mode & 8) {
        sum += Zc13AabbOverlap(f0, f1, f2, f3, f0, f1, f2, f3);
    }
    if (mode & 16) {
        sum += (int)Zc13RngNextU32(rng);
    }
    if (mode & 32) {
        sum += (int)Zc13FindCollidingIce(cx, cy, grid, pos, f0);
    }
    if (mode & 64) {
        sum += (int)new (boss) Zc13BossStruct();
    }
    if (mode & 128) {
        Zc13LzssReplaceNode((int)cx, (int)cy);
    }
    if (mode & 256) {
        Zc13InitInterpTimer(interp, (int)cx, (unsigned char)cy, (unsigned char)mode, (unsigned char)(mode >> 8));
    }
    return sum;
}
