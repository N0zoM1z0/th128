#include "ZeroCallMedium12.hpp"

extern "C" int __stdcall ZeroCallMedium12Probe(
    const char *path,
    const Zc12Float3 *pos,
    int score,
    int color,
    Zc12GuiMeters *gui,
    Zc12BulletMove *bul,
    Zc12LaserObj *las,
    Zc12WaveReader *wr,
    unsigned int size,
    unsigned char *dst,
    unsigned int *outRead,
    Zc12SfxChan *chan,
    int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Zc12DatCount = score;
        g_Zc12DatTable[0].name = path;
        g_Zc12PopupMgr = (Zc12PopupMgr *)gui;
        g_Zc12TimeScale = pos->x;
        g_Zc12SfxVol = score;
        g_Zc12TexFmt = score;
        g_Zc12TexPitch = color;
        g_Zc12TexPixels = dst;
        *(void **)outRead = &g_Zc12SfxVol;
    }
    if (mode & 2) {
        sum += (int)Zc12FindDatEntry(path);
    }
    if (mode & 4) {
        Zc12SpawnScorePopup(pos, score, color);
    }
    if (mode & 8) {
        Zc12SetMeterAnmsA(gui, score, color);
    }
    if (mode & 16) {
        Zc12SetMeterAnmsB(gui, score, color);
    }
    if (mode & 32) {
        sum += Zc12StepBulletMove(bul);
    }
    if (mode & 64) {
        Zc12LaserObj localLas;
        Zc12LaserObj *lp = (mode & 1024) ? &localLas : las;
        lp->RunActions();
    }
    if (mode & 128) {
        sum += Zc12ReadWaveData(wr, size, dst, outRead);
    }
    if (mode & 256) {
        Zc12PlaySfxChannel(chan, score);
    }
    if (mode & 512) {
        sum += Zc12InvertTexAlpha(score);
    }
    return sum;
}
