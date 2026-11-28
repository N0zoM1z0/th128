#include "../src/WaveFive.hpp"

namespace th128 {

volatile int g_WfSink = 0;

int g_WfTextFmt = 0;
int g_WfTextStride = 0;
unsigned char *g_WfTextBits = NULL;
WfGui *g_WfGui = NULL;
WfD3dDevice9 *g_WfDevice = NULL;
unsigned int g_WfFlags = 0;
unsigned char g_WfSoundFlag = 0;
int g_WfSoundInited = 0;
char g_WfBgmPath[256];
CRITICAL_SECTION g_WfSoundCs;
unsigned char g_WfSoundCsDepth = 0;
WfCritSupervisor g_WfCritSup;
WfSoundManager g_WfSoundMgr;
WfBgmEntry *g_WfPreloadEntries[16];
void *g_WfPreloadBufs[16];
void *g_WfPreloadCurs[16];
int g_WfPreloadSizes[16];
char g_WfPreloadNames[16][256];

__declspec(noinline) void WfSoundStream::Stop(int flag) {
    if (this->buf != NULL) {
        g_WfSink += (int)this->buf + flag;
    }
}

__declspec(noinline) int WfSoundManager::FindBgmIndex(const char *path) {
    g_WfSink += (int)this->bgmTable + (int)path;
    return g_WfSink & 7;
}

__declspec(noinline) void WfCritSupervisor::UnlockCritSec(int idx) {
    g_WfSink += (int)this + idx;
}

__declspec(noinline) void WfSoundDebugLog(const char *fmt, ...) {
    g_WfSink += (int)fmt;
}

__declspec(noinline) void WfAnmManager::FlushSpriteBatch() {
    this->fvfState = 0;
    g_WfSink += (int)this;
}

__declspec(noinline) void WfAnmManager::ApplyState(WfAnmVm *vm) {
    this->fvfState = (unsigned char)vm->flags;
    g_WfSink += (int)this + (int)vm;
}

__declspec(noinline) void WfAnmManager::AppendSprite(const void *verts) {
    g_WfSink += (int)this + (int)verts;
}

} // namespace th128

static void *volatile g_WfKeep[8];
static void (th128::WfLaserInfinite::*volatile g_WfSlot01Fn)() = NULL;

extern "C" int __stdcall WaveFiveProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    g_WfSlot01Fn = &WfLaserInfinite::Slot01;
    WfLaserInfinite *li = (WfLaserInfinite *)(int)k;
    (li->*g_WfSlot01Fn)();

    g_WfSink += WfInvertTextImageAlpha(a);
    g_WfSink += WfInvertTextImageAlpha(b);

    int sec = 0, cs = 0;
    WfGuiGetSpellTime(&sec, &cs);
    g_WfSink += sec + cs;

    WfSoundManager *sm = (WfSoundManager *)(int)k;
    sm->StopStreamingBgm();
    (sm + 1)->StopStreamingBgm();
    ((WfSoundStream *)(int)a)->Stop(b);
    ((WfSoundStream *)(int)b)->Stop(a);

    WfAnmManager *am = (WfAnmManager *)(int)k;
    WfAnmVm *vm = (WfAnmVm *)(int)a;
    g_WfSink += am->DrawSprite2D(vm, (const void *)(int)b);
    g_WfSink += (am + 1)->DrawSprite2D(vm + 1, (const void *)(int)(b + 4));

    g_WfSink += WfPreLoadBgm((const char *)(int)a, b);
    g_WfSink += WfPreLoadBgm((const char *)(int)b, a);
    g_WfSink += sm->FindBgmIndex((const char *)(int)a);
    g_WfSink += (sm + 1)->FindBgmIndex((const char *)(int)b);
    ((WfCritSupervisor *)(int)a)->UnlockCritSec(b);
    ((WfCritSupervisor *)(int)b)->UnlockCritSec(a);

    g_WfKeep[0] = (void *)&WfInvertTextImageAlpha;
    g_WfKeep[1] = (void *)&WfGuiGetSpellTime;
    g_WfKeep[2] = (void *)&WfSoundDebugLog;
    return g_WfSink;
}
