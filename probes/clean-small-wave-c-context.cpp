#include "../src/CleanSmallWaveC.hpp"

namespace th128 {

volatile int g_CscSink = 0;

CscEndingMgr *g_CscEndingMgr = NULL;
CscAsciiMgr *g_CscAsciiMgr = NULL;
unsigned char g_CscSoundFlag = 0;
int g_CscSoundMgr = 0;
CscScoreData *g_CscScoreData = NULL;

__declspec(noinline) void CscDeleteVmByIdPtr(int *idPtr) {
    if (*idPtr != 0) {
        Sleep((DWORD)*idPtr);
        *idPtr = 0;
        g_CscSink += (int)idPtr;
    }
}

__declspec(noinline) void CscRegisterDraw(CscChainElem *elem, int prio) {
    Sleep((DWORD)prio);
    g_CscSink += (int)elem + prio;
}

__declspec(noinline) void CscFogDrawCb() {
    g_CscSink += 1;
}

__declspec(noinline) void __stdcall CscWaveFile::ReopenTrack(void *d, HANDLE h) {
    g_CscSink += (int)this + (int)d;
    Sleep((DWORD)h);
    g_CscSink += (int)h;
}

__declspec(noinline) void *CscPreloadAnm(int slot, const char *path) {
    g_CscSink += slot;
    Sleep((DWORD)path);
    g_CscSink += (int)path;
    return (void *)g_CscSink;
}

__declspec(noinline) void __stdcall CscStopVmById(int id, int flag) {
    Sleep((DWORD)id);
    g_CscSink += flag;
    Sleep((DWORD)flag);
    g_CscSink += id;
}

__declspec(noinline) void __stdcall CscEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *path) {
    DWORD t1 = GetTickCount();
    g_CscSink += (int)mgr + cmd + arg + (int)path + (int)t1;
    DWORD t2 = GetTickCount();
    g_CscSink += (int)t1 * 3 + (int)t2 + (int)path;
}

} // namespace th128

static void *volatile g_CscKeep[8];

static unsigned int (th128::CscScoreBuf::*volatile g_CscChecksumFn)(int) = NULL;

extern "C" int __stdcall CleanSmallWaveCProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    CscGuiSubObj *gs = (CscGuiSubObj *)(int)k;
    gs->~CscGuiSubObj();
    (gs + 1)->~CscGuiSubObj();

    CscLineArray *la = (CscLineArray *)(int)k;
    la->FreeLines();
    (la + 1)->FreeLines();

    g_CscChecksumFn = &CscScoreBuf::CalcChecksum;
    CscScoreBuf *sb = (CscScoreBuf *)(int)a;
    g_CscSink += (int)(sb->*g_CscChecksumFn)(b);

    CscFogMgr *fm = (CscFogMgr *)(int)k;
    g_CscSink += fm->RegisterDrawChain();
    g_CscSink += (fm + 1)->RegisterDrawChain();
    CscRegisterDraw((CscChainElem *)(int)a, b);

    CscStreamingSound *ss = (CscStreamingSound *)(int)k;
    g_CscSink += (int)ss->Resume();
    g_CscSink += (int)(ss + 1)->Resume();

    g_CscSink += CscLoadEndingAnmWorkerA();
    g_CscSink += CscLoadEndingAnmWorkerA();
    g_CscSink += CscLoadEndingAnmWorkerB();
    g_CscSink += CscLoadEndingAnmWorkerB();
    CscStopVmById(a, b);
    CscStopVmById(b, a);

    g_CscSink += CscPlayPreloadedBgm(a, b);
    g_CscSink += CscPlayPreloadedBgm(b, a);
    CscEnqueueBgmCommand((void *)(int)a, a, b, (const char *)(int)k);

    g_CscKeep[0] = (void *)&CscPlayPreloadedBgm;
    return g_CscSink;
}
