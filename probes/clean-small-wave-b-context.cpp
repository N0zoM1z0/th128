#include "../src/CleanSmallWaveB.hpp"

namespace th128 {

volatile int g_CsbSink = 0;

int g_CsbTitleFlag = 0;
CsbLaserMgr *g_CsbLaserMgr = NULL;
CsbAnmMgr *g_CsbAnmMgr = NULL;
CsbEnemyMgr *g_CsbEnemyMgr = NULL;
int g_CsbFreezeGauge = 0;
CsbChain *g_CsbChain = NULL;
unsigned int g_CsbFlags = 0;
CRITICAL_SECTION g_CsbChainCs;
unsigned char g_CsbChainCsDepth = 0;
CsbFogMgr *g_CsbFogMgr = NULL;
CsbItemMgr2 *g_CsbItemMgr2 = NULL;

__declspec(noinline) void CsbThreadInf::Stop() {
    if (this->handle != NULL) {
        WaitForSingleObject(this->handle, 100);
        CloseHandle(this->handle);
        this->handle = NULL;
    }
}

__declspec(noinline) CsbTitleMgr::CsbTitleMgr() {
    Sleep(1);
    g_CsbSink += (int)this;
}

__declspec(noinline) CsbLaserBase::CsbLaserBase() {
    Sleep(1);
    data[0] = 0;
    g_CsbSink += (int)this;
}

__declspec(noinline) CsbLaserMgr::~CsbLaserMgr() {
    Sleep(1);
    g_CsbSink += f5d8;
}

__declspec(noinline) void CsbAnmRes::Release() {
    int x = (int)this;
    Sleep((DWORD)x);
    g_CsbSink += x + (int)this;
}

__declspec(noinline) CsbItemMgr2::~CsbItemMgr2() {
    Sleep(1);
    g_CsbSink += (int)flags;
}

__declspec(noinline) unsigned __stdcall CsbTitleWorker(void *arg) {
    g_CsbSink += (int)arg;
    return 0;
}

__declspec(noinline) int __stdcall CsbInitLaserMgr(CsbLaserMgr *mgr) {
    Sleep(1);
    return (int)mgr & 1;
}

__declspec(noinline) int __stdcall CsbInitItemMgr2(CsbItemMgr2 *mgr) {
    Sleep(1);
    return (int)mgr & 1;
}

__declspec(noinline) void CsbPlaySfx(int id) {
    g_CsbSink += id * 3;
    Sleep(1);
}

__declspec(noinline) void __fastcall CsbChainCut(CsbChain *chain, CsbChainElem *elem) {
    if (chain->head == elem) chain->head = elem->next;
    if (chain->tail == elem) chain->tail = elem->prev;
    if (g_CsbSink == 99998) { g_CsbFlags ^= 1; g_CsbChain = NULL; }
}

__declspec(noinline) void CsbDeleteVmByIdPtr(int *idPtr) {
    if (*idPtr != 0) {
        Sleep((DWORD)*idPtr);
        *idPtr = 0;
        g_CsbSink += (int)idPtr;
    }
}

__declspec(noinline) static CsbFogMgr *CsbGetFog(int k) { return (CsbFogMgr *)k; }

} // namespace th128

static void *volatile g_CsbKeep[8];

extern "C" int __stdcall CleanSmallWaveBProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    g_CsbKeep[0] = (void *)&CsbChainCut;
    g_CsbKeep[1] = (void *)&CsbInitLaserMgr;
    g_CsbKeep[2] = (void *)&CsbInitItemMgr2;
    g_CsbKeep[3] = (void *)&CsbAddFreezeGauge;

    g_CsbSink += (int)CsbCreateTitleMgr();
    g_CsbSink += (int)CsbCreateTitleMgr();
    g_CsbSink += (int)CsbCreateLaserMgr();
    g_CsbSink += (int)CsbCreateLaserMgr();
    g_CsbSink += CsbReleaseEndingAnms();
    g_CsbSink += CsbReleaseEndingAnms();

    char buf[64];
    g_CsbSink += (int)CsbReadShiftJisLine((const char *)(int)a, buf, b);
    g_CsbSink += (int)CsbReadShiftJisLine((const char *)(int)b, buf + 4, a);

    CsbUnlinkEnemy((CsbEnemy *)(int)a);
    CsbUnlinkEnemy((CsbEnemy *)(int)b);

    CsbAddFreezeGauge(a);
    CsbAddFreezeGauge(b);

    delete CsbGetFog(k);
    delete CsbGetFog(k + 1);

    g_CsbSink += (int)CsbCreateItemMgr2();
    g_CsbSink += (int)CsbCreateItemMgr2();

    int rem = a;
    g_CsbSink += (int)CsbReadTextLine((char *)(int)a, buf, &rem);
    g_CsbSink += (int)CsbReadTextLine((char *)(int)b, buf + 4, &rem);

    CsbGuiSubObj *gs = (CsbGuiSubObj *)(int)k;
    g_CsbSink += (int)gs->Destroy();
    g_CsbSink += (int)(gs + 1)->Destroy();

    return g_CsbSink;
}
