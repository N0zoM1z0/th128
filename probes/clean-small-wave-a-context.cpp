#include "../src/CleanSmallWaveA.hpp"

#include <string.h>

namespace th128 {

volatile int g_CsaSink = 0;

CsaAnmManager *g_CsaAnmMgr = NULL;
unsigned int g_CsaFlags = 0;
CRITICAL_SECTION g_CsaThreadCs;
unsigned char g_CsaThreadCsDepth = 0;
CsaThreadInf g_CsaLoaderThread;
CsaScoreData *g_CsaScoreData = NULL;
int g_CsaSoundMgr = 0;

__declspec(noinline) CsaAnmVm *CsaAnmManager::GetVmById(int id) {
    CsaVmNode *n = this->worldHead;
    while (n != NULL) {
        if (n->vm->id == id) {
            return n->vm;
        }
        n = n->next;
    }
    return NULL;
}

__declspec(noinline) void CsaThreadInf::Stop() {
    if (this->handle != NULL) {
        WaitForSingleObject(this->handle, 100);
        CloseHandle(this->handle);
        this->handle = NULL;
    }
}

__declspec(noinline) void __stdcall CsaEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *path) {
    DWORD t1 = GetTickCount();
    g_CsaSink += (int)mgr + cmd + arg + (int)path + (int)t1;
    DWORD t2 = GetTickCount();
    g_CsaSink += (int)t1 * 3 + (int)t2 + (int)path;
}

} // namespace th128

static bool (th128::CsaMemFile::*volatile g_CsaSeekFn)(int, int) = NULL;

extern "C" int __stdcall CleanSmallWaveAProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;

    CsaAnmVm *vm = (CsaAnmVm *)(int)k;
    int outId = 0;
    CsaRegisterWorldVmAtTail(vm, &outId);
    CsaRegisterWorldVmAtTail(vm + 1, &outId);
    CsaRegisterUiVmAtTail(vm, &outId);
    CsaRegisterUiVmAtTail(vm + 1, &outId);
    CsaRegisterWorldVmAtHead(vm, &outId);
    CsaRegisterWorldVmAtHead(vm + 1, &outId);
    CsaRegisterUiVmAtHead(vm, &outId);
    CsaRegisterUiVmAtHead(vm + 1, &outId);

    typedef unsigned (__stdcall *ThreadFn)(void *);
    g_CsaSink += CsaStartLoaderThread((ThreadFn)(int)a);
    g_CsaSink += CsaStartLoaderThread((ThreadFn)(int)b);
    ((CsaThreadInf *)(int)a)->Stop();

    g_CsaSink += CsaPlayStageBgm((const char *)(int)a, b);
    g_CsaSink += CsaPlayStageBgm((const char *)(int)b, a);
    g_CsaSink += CsaPlayBgmWithLoop(a, (const char *)(int)b);
    g_CsaSink += CsaPlayBgmWithLoop(b, (const char *)(int)a);
    CsaEnqueueBgmCommand((void *)(int)a, a, k, (const char *)(int)b);

    g_CsaSeekFn = &CsaMemFile::Seek;
    CsaMemFile *mf = (CsaMemFile *)(int)k;
    g_CsaSink += (mf->*g_CsaSeekFn)(a, b);

    CsaSetInterruptAll((CsaAnmVm2 *)(int)k, a, (short)b);
    CsaSetInterruptAll((CsaAnmVm2 *)(int)(k + 1), b, (short)a);

    CsaCardObj *co = (CsaCardObj *)(int)k;
    co->ShowVm(a);
    (co + 1)->ShowVm(b);
    co->HideVmBySlot(a, b);
    (co + 1)->HideVmBySlot(b, a);

    return g_CsaSink + outId;
}
