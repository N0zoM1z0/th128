#include "../src/CleanBatchM.hpp"

namespace th128 {

int g_CbmPower = 0;
int g_CbmPowerFlag = 0;
void* g_CbmGui = 0;
CbmSeMgr g_CbmSeMgr;

static volatile int g_Sink = 0;

__declspec(noinline) void CbmBgVm::Update() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}
__declspec(noinline) void CbmSeMgr::PlaySe(int id) {
    int a = g_Sink;
    g_Sink = a + (1 << (a & 7)) + id * 3;
}
__declspec(noinline) void CbmShowPopup(void* gui, int a, int b, int c) {
    DWORD t1 = GetTickCount();
    DWORD t2 = GetTickCount();
    DWORD t3 = GetTickCount();
    g_Sink += (int)gui + a * 3 + b + c + (int)(t1 * 3 + t2 * 5 + t3);
}

} // namespace th128

extern "C" int __stdcall CleanBatchMProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        return ((CbmStageBg*)arg)->UpdateQuads();
    }
    if (selector == 2) {
        CbmAddPower(arg);
        return g_Sink;
    }
    if (selector == 3) {
        ((CbmLaserMgr*)arg)->SpawnNode(arg & 7);
        return g_Sink;
    }
    if (selector == 4) {
        ((CbmStrip*)arg)->CopyStripVerts();
        return g_Sink;
    }
    if (selector == 5) {
        return (int)CbmSumU32((const unsigned int*)arg, arg & 0xff);
    }
    if (selector == 6) {
        return (int)CbmStringErase((std::string*)arg, arg & 7, (arg >> 3) & 7).size();
    }
    if (selector == 7) {
        g_CbmSeMgr.PlaySe(arg);
        CbmShowPopup((void*)arg, arg, arg + 1, arg + 2);
        return g_Sink;
    }
    return 0;
}
