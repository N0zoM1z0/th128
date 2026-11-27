#include "SoundAnmHelpers.hpp"

namespace th128 {

SaSoundMgr g_SaSoundMgr;
void *g_SaSoundThread = 0;
void *g_SaSfxThread = 0;
unsigned long g_SaSoundThreadId = 0;
int g_SaSoundState = 0;
void *g_SaSoundArg = 0;
const char g_SaBgmTableName[] = "thbgm.fmt";
void *g_SaBgmEvent = 0;
SaBgmStream *g_SaBgmStream = 0;
SaErrorLog g_SaErrorLog;

volatile int g_SaAnchorSink = 0;

__declspec(noinline) unsigned long __stdcall SaSoundThreadProc(void *param) {
    g_SaAnchorSink ^= (int)param;
    return 0;
}

__declspec(noinline) void *__stdcall SaLoadFile(const char *path, int a, int b) {
    g_SaAnchorSink ^= (int)path ^ a ^ b;
    return (void *)&g_SaAnchorSink;
}

__declspec(noinline) void __stdcall SaHandleStreamNotify(SaBgmStream *stream) {
    g_SaAnchorSink ^= (int)stream;
}

__declspec(noinline) void SaSfxLoadThread() {
    g_SaAnchorSink ^= 0x1111;
}

__declspec(noinline) void SaStreamObj::Cleanup() {
    g_SaAnchorSink ^= (int)this;
}

} // namespace th128

using namespace th128;

extern "C" int __stdcall SoundAnmHelpersProbe(
    void *arg,
    SaSoundMgr *mgr,
    SaD3DTexture **ppTex,
    SaInputPad *pad,
    float f0, float f1, float f2, float f3, float f4, float f5, float f6, float f7
) {
    volatile void *ptrs[] = {
        (void *)&SaNoOp4575C0,
        (void *)&SaInitSoundThread,
        (void *)&SaRequestSoundStop,
        (void *)&SaShutdownSoundThreads,
        (void *)&SaStartSfxThread,
        (void *)&SaLoadBgmTable,
        (void *)&SaBgmThreadProc,
        (void *)&SaSinF,
        (void *)&SaClearD3DTexture,
        (void *)&SaHitTestOrientedRect,
        (void *)&SaHitTestQuad,
        (void *)&SaHitTestOrientedRectMargin,
        (void *)&SaLoadFile,
        (void *)&SaHandleStreamNotify,
    };
    void (SaStreamObj::*volatile pCleanup)() = &SaStreamObj::Cleanup;
    g_SaAnchorSink ^= (int)ptrs[0] ^ (int)&pCleanup;

    int acc = 0;
    SaNoOp4575C0();
    acc ^= ((int (__stdcall *)(void *))ptrs[1])(arg);
    acc ^= SaRequestSoundStop();
    acc ^= SaShutdownSoundThreads();
    SaStartSfxThread();
    acc ^= ((int (__stdcall *)(SaSoundMgr *))ptrs[5])(mgr);
    acc ^= (int)((unsigned long (__stdcall *)(void *))ptrs[6])(arg);
    acc ^= (int)((float (__stdcall *)(float))ptrs[7])(f0);
    ((void (__stdcall *)(SaD3DTexture **))ptrs[8])(ppTex);
    acc ^= ((int (__stdcall *)(float, float, float, float, float, float, float))ptrs[9])(f0, f1, f2, f3, f4, f5, f6);
    acc ^= ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[10])(f0, f1, f2, f3, f4, f5, f6, f7);
    acc ^= ((int (__stdcall *)(float, float, float, float, float, float, float, float))ptrs[11])(f0, f1, f2, f3, f4, f5, f6, f7);
    acc ^= (int)g_SaErrorLog.ptr;

    SaStreamObj *obj = new SaStreamObj();
    delete obj;

    SaInputPad *arr = new SaInputPad[2];
    arr[0].UpdateRepeatTimers();
    pad->UpdateRepeatTimers();
    delete[] arr;

    return acc ^ g_SaAnchorSink;
}
