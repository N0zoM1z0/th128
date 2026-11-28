#include "../src/CleanBatchJ.hpp"

namespace th128 {

CbjAnmMgr* g_CbjAnmMgr = 0;
CbjScoreState* g_CbjScoreState = 0;
int g_CbjBombVal = 0;
int g_CbjBombLocked = 0;
int g_CbjLifeVal = 0;
void* g_CbjGui = 0;
int g_CbjJoyIdx = 0;
CbjJoyDevice* g_CbjJoyDevices[16];

static volatile int g_Sink = 0;

__declspec(noinline) void CbjPlaySfx(int id) {
    g_Sink += id * 3;
}
__declspec(noinline) void CbjShowPopup(void* gui, int type, int subtype, int arg) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    g_Sink += (int)gui + type + subtype + arg + (int)(t1 * 3 + t2 * 5);
}
__declspec(noinline) void CbjOnLifeUp(void* gui) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    DWORD t4 = GetTickCount();
    g_Sink += (int)gui + (int)(t1 * 3 + t2 * 5 + t3 * 7 + t4);
}

static void (__stdcall * volatile g_DecodePtr)(int*, int*) = &CbjDecodeScoreVal;
static void (__stdcall * volatile g_BombPtr)(int) = &CbjAddBombPiece;
static void (__stdcall * volatile g_LifePtr)(int) = &CbjAddLifePiece;
static BOOL (__stdcall * volatile g_EnumPtr)(const CbjJoyObjInst*, void*) = &CbjEnumJoyAxisCb;

} // namespace th128

extern "C" int __stdcall CleanBatchJProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        return (int)CbjCountIceBits((CbjIceGrid*)GetTickCount());
    }
    if (selector == 2) {
        ((CbjStageClearObj*)arg)->Clear();
        return g_Sink;
    }
    if (selector == 3) {
        int a, b;
        g_DecodePtr(&a, &b);
        return a + b;
    }
    if (selector == 4) {
        g_BombPtr(arg);
        return g_Sink;
    }
    if (selector == 5) {
        g_LifePtr(arg);
        return g_Sink;
    }
    if (selector == 6) {
        return g_EnumPtr((const CbjJoyObjInst*)arg, (void*)arg);
    }
    if (selector == 7) {
        ((CbjMesh*)arg)->CopyVerts();
        return g_Sink;
    }
    if (selector == 8) {
        return (int)CbjChecksumDwords((const unsigned int*)arg, arg);
    }
    if (selector == 9) {
        CbjPlaySfx(arg);
        CbjShowPopup((void*)arg, arg, arg + 1, arg + 2);
        CbjOnLifeUp((void*)arg);
        return g_Sink;
    }
    return 0;
}
