#include "../src/EhBatchF.hpp"

namespace th128 {

EbfChainMgr* g_EbfChainMgr = 0;
EbfAnmMgr* g_EbfAnmMgr = 0;
EbfPlayer* g_EbfPlayer = 0;
int g_EbfLockFlags = 0;
CRITICAL_SECTION g_EbfChainCs;
unsigned char g_EbfChainLockCount = 0;
unsigned char g_EbfStageFlags = 0;
void* g_EbfSavedBuf = 0;

static volatile int g_Sink = 0;

__declspec(noinline) void __fastcall EbfChainCutInternal(EbfChainMgr* chain, EbfChainElem* elem) {
    void* p = ::operator new(4);
    g_Sink += (int)elem * 3 + (1 << ((int)chain & 7)) + (int)p;
    DWORD t = GetTickCount();
    g_Sink += (int)t + (int)elem;
}
typedef void (__fastcall *EbfCutFn)(EbfChainMgr*, EbfChainElem*);
volatile EbfCutFn g_EbfCutPtr = &EbfChainCutInternal;

__declspec(noinline) EbfAnmVm::~EbfAnmVm() {
    void* p = ::operator new(4);
    g_Sink += (int)this + (int)p;
}
__declspec(noinline) void EbfAnmMgr::KeepPlayerAnm(int id) {
    g_Sink += 1 << ((int)anmSlot & 7);
    for (EbfAnmListNode* n = listA; n != 0; n = n->next) {
        if ((int)n->vm == id) {
            g_Sink += (int)anmSlot;
        }
    }
    DWORD t = GetTickCount();
    g_Sink += (int)anmSlot + (int)t;
}
__declspec(noinline) void EbfReleaseAnm(void* anm) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)t1 + (int)anm;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)anm;
}
__declspec(noinline) void EbfDestroyVm(void* vm) {
    void* p = ::operator new(4);
    g_Sink += (int)p + (int)vm * 3;
    DWORD t1 = GetTickCount();
    g_Sink ^= (int)vm * 5 + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)vm * 7 + (int)t2;
}

} // namespace th128

extern "C" int __stdcall EhBatchFProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        ((EbfPlayer*)arg)->~EbfPlayer();
        return g_Sink;
    }
    if (selector == 2) {
        ((EbfAnmMgr*)arg)->~EbfAnmMgr();
        return g_Sink;
    }
    if (selector == 3) {
        g_EbfCutPtr((EbfChainMgr*)arg, (EbfChainElem*)(arg + 4));
        ((EbfAnmMgr*)arg)->KeepPlayerAnm(arg);
        return g_Sink;
    }
    return 0;
}
