#include "ZeroCallMedium11.hpp"

extern "C" int __stdcall ZeroCallMedium11Probe(
    const Zc11IceField *field,
    Zc11EffectMgr *effMgr,
    const Zc11Float3 *pos,
    const Zc11Float3 *pos2,
    float a,
    float b,
    int p1,
    int p2,
    int p3,
    Zc11DebugBuf *dbg,
    const char *key,
    Zc11BulletObj *bul,
    Zc11ImgBuf *img,
    Zc11Surface *surf,
    Zc11SoundTable *sndTbl,
    Zc11StageObj *stg,
    int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Zc11TimeScale = a;
        g_Zc11CamPos = *pos;
        g_Zc11AnmMgr = (Zc11AnmMgr *)stg;
        *(void **)bul = &g_Zc11CamPos;
    }
    if (mode & 2) {
        sum += Zc11PopcountIceBitmap(field);
    }
    if (mode & 4) {
        sum += (int)effMgr->AllocSlot(pos, a, b, p1, p2, p3);
    }
    if (mode & 8) {
        sum += Zc11CheckDebugKey(dbg, key, p1, p2);
    }
    if (mode & 16) {
        Zc11InitBulletTimer(bul, pos, pos2, p1, (unsigned char)p2);
    }
    if (mode & 32) {
        sum += Zc11BlitToSurface(img, surf);
    }
    if (mode & 64) {
        sum += Zc11CopyFromSurface(img, surf);
    }
    if (mode & 128) {
        sum += Zc11FindSoundByName(key, sndTbl);
    }
    if (mode & 256) {
        Zc11CleanupStageAnms(stg);
    }
    return sum;
}
