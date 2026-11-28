#include "../src/Win32SysWave.hpp"

#include <objbase.h>

extern "C" HRESULT __stdcall DirectInput8Create(HINSTANCE hinst, DWORD dwVersion,
                                                const IID &riidltf, LPVOID *ppvOut,
                                                void *punkOuter);

namespace th128 {

volatile int g_WswSink = 0;

WswSoundView *g_WswBgmView = NULL;
int g_WswSavedScreenSave = 0;
int g_WswSavedLowPower = 0;
int g_WswSavedPowerOff = 0;
LARGE_INTEGER g_WswPerfFreq = { 0 };
LARGE_INTEGER g_WswPerfCounter = { 0 };
WswSupervisor g_WswSupervisor;
unsigned int g_WswFlags = 0;
CRITICAL_SECTION g_WswCritSec;
unsigned char g_WswCritDepth = 0;
WswChain *g_WswChain = NULL;
WswReplayManager *g_WswReplayMgr = NULL;
unsigned char g_WswLaunchedFromShortcut = 0;
int g_WswGlobal4db7e8 = 0;
HINSTANCE g_WswInstance = NULL;
HWND g_WswHwnd = NULL;
WswErrorContext g_WswErrorCtx;
unsigned int g_WswDiDevCaps[11];
int g_WswPadAxisCount = 0;

const GUID g_WswIID_IDirectInput8A = { 0 };
const GUID g_WswGUID_SysKeyboard = { 0 };
const void *g_WswDfDIKeyboard = NULL;
const void *g_WswDfDIJoystick2 = NULL;
const GUID g_WswCLSID_ShellLink = { 0 };
const GUID g_WswIID_IShellLinkA = { 0 };
const GUID g_WswIID_IPersistFile = { 0 };

__declspec(noinline) void WswSoundView::SetVolume(int volume) {
    g_WswSink += (int)this + volume;
    if (g_WswSink == 99999) {
        g_WswBgmView = NULL;
    }
}

__declspec(noinline) void WswChain::Cut(WswChainElem *elem) {
    g_WswSink += (int)this + (int)elem;
    if (g_WswSink == 99998) {
        g_WswFlags ^= 1;
        g_WswChain = NULL;
    }
}

__declspec(noinline) WswStageChunk::~WswStageChunk() {
    g_WswSink += data[0];
}

__declspec(noinline) void WswReplayManager::FreeStageChunks(int index) {
    void *p = this->f20[index];
    if (p != NULL) {
        operator delete(p);
        this->f20[index] = NULL;
    }
    g_WswSink += (int)this->f18 + index;
}

__declspec(noinline) void WswInputSupervisor::ReleaseDirectInput() {
    if (this->dinput != NULL) {
        this->dinput->Release();
        this->dinput = NULL;
    }
    g_WswSink += (int)this;
}

__declspec(noinline) void WswErrorContext::Log(const char *fmt, ...) {
    g_WswSink += (int)this + (int)fmt;
}

__declspec(noinline) void WswLog(const char *fmt, ...) {
    g_WswSink += (int)fmt;
}

__declspec(noinline) int __stdcall WswFileExists(const char *path) {
    return GetFileAttributesA(path) != INVALID_FILE_ATTRIBUTES;
}

__declspec(noinline) void WswEnumJoysticksCb() {
    g_WswSink += 1;
}

__declspec(noinline) void WswEnumObjectsCb() {
    g_WswSink += 2;
}

__declspec(noinline) static WswReplayManager *WswGetReplayMgr(int k) {
    return (WswReplayManager *)k;
}

} // namespace th128

static void *volatile g_WswKeep[8];

extern "C" int __stdcall Win32SysWaveProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    WswBgmFadeUpdate();

    unsigned char *buf = (unsigned char *)(int)k;
    WswEncryptBlocks(buf, k, (unsigned char)a, (unsigned char)b, k + 2, k + 3);
    WswEncryptBlocks(buf + 16, k + 1, (unsigned char)(a + 1), (unsigned char)(b + 1), k + 4, k + 5);

    WswTextImage *img = (WswTextImage *)(int)k;
    HFONT f = (HFONT)(int)a;
    const char *s = (const char *)(int)b;
    g_WswSink += img->DrawOutlinedText(k, k + 1, k + 2, k + 3, f, s);
    g_WswSink += (img + 1)->DrawOutlinedText(k + 1, k + 2, k + 3, k + 4, (HFONT)(int)(a + 1), (const char *)(int)(b + 1));

    WswSupervisor *sup = (WswSupervisor *)(int)k;
    sup->InitSystemPathsAndDwm();
    (sup + 1)->InitSystemPathsAndDwm();

    g_WswSink += WswCheckShortcutLaunch();
    g_WswSink += WswCheckShortcutLaunch();

    WswReplayManager *rm0 = WswGetReplayMgr(k + 2);
    rm0->FreeStageChunks(a);
    rm0->FreeStageChunks(b);
    delete WswGetReplayMgr(k);
    delete WswGetReplayMgr(k + 1);

    WswInputSupervisor *isup = (WswInputSupervisor *)(int)k;
    g_WswSink += isup->InitDirectInput();
    g_WswSink += (isup + 1)->InitDirectInput();

    g_WswKeep[0] = (void *)&WswBgmFadeUpdate;
    g_WswKeep[1] = (void *)&WswLog;
    g_WswKeep[2] = (void *)&WswFileExists;
    return g_WswSink;
}
