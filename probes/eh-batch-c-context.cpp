#include "EhBatchC.hpp"

volatile int g_EbcSink = 0;

__declspec(noinline) void __fastcall EbcChainCutInternal(EbcChain *chain, EbcChainElem *elem) {
    g_EbcSink += (int)elem * 3 + (1 << ((int)chain & 7));
    DWORD t = GetTickCount();
    g_EbcSink += (int)t + (int)elem;
}

typedef void (__fastcall *EbcCutFn)(EbcChain *, EbcChainElem *);
volatile EbcCutFn g_EbcCutPtr = &EbcChainCutInternal;

__declspec(noinline) EbcAnmSlot::~EbcAnmSlot() {
    DWORD t1 = GetTickCount();
    g_EbcSink += (int)t1 + (int)this;
    DWORD t2 = GetTickCount();
    g_EbcSink += (int)t1 * 3 + (int)t2 + (int)this;
}

__declspec(noinline) void EbcThreadBase::Reset() {
    DWORD t = GetTickCount();
    g_EbcSink += (int)this + (int)t;
    DWORD t2 = GetTickCount();
    g_EbcSink += (int)this + (int)t2;
}

__declspec(noinline) EbcReplayItem::~EbcReplayItem() {
    g_EbcSink += (int)this + dummy;
    g_EbcSink += (int)GetTickCount();
}

__declspec(noinline) void EbcStopBgm(int arg) {
    g_EbcSink += arg * 3;
    void *p = ::operator new(4);
    g_EbcSink += (int)p;
}

extern "C" int __stdcall EhBatchCProbe(void **args, int mode) {
    int r = 0;
    if (mode & 1) delete (EbcMainMgr *)args[0];
    if (mode & 2) delete (EbcScoreMgr *)args[1];
    if (mode & 4) delete (EbcTitleObj *)args[2];
    if (mode & 8) ((EbcMainMgr *)args[0])->~EbcMainMgr();
    if (mode & 16) ((EbcScoreMgr *)args[1])->~EbcScoreMgr();
    if (mode & 32) ((EbcTitleObj *)args[2])->~EbcTitleObj();
    if (mode & 64) {
        g_EbcCutPtr((EbcChain *)args[3], (EbcChainElem *)args[4]);
        EbcStopBgm(mode);
    }
    return r + g_EbcSink;
}
