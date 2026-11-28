#include "../src/AnmDrawChainWave.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace th128 {

IDirect3DDevice9 *g_AdcwDevice = NULL;
AdcwAnmManager *g_AdcwAnmMgr = NULL;
AdcwChain *g_AdcwChain = NULL;
AdcwErrorCtx g_AdcwErrorCtx;

static volatile int g_Sink = 0;

static __declspec(noinline) void AdcwLockCs(int idx) { g_Sink += idx + (int)GetTickCount(); }
static __declspec(noinline) void AdcwUnlockCs(int idx) { g_Sink += idx * 3 + (int)GetTickCount(); }

__declspec(noinline) const char *__cdecl AdcwErrorCtx::Log(const char *fmt, ...) {
    char local[0x2000];
    va_list args;
    AdcwLockCs(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    AdcwUnlockCs(3);
    return fmt;
}

__declspec(noinline) void AdcwAnmManager::FlushSpriteBatch() {
    if (batchCount4e96ec == 0) return;
    DWORD t = GetTickCount();
    g_Sink += batchCount4e96ec + (int)t;
    batchCount4e96ec = 0;
}

__declspec(noinline) void AdcwAnmManager::ApplyState(AdcwAnmVm *vm) {
    g_Sink += batchCount4e96ec + (int)vm->alpha3f3;
}

__declspec(noinline) void AdcwAnmManager::ApplyStateNoVm() {
    g_Sink += batchCount4e96ec + 1;
}

__declspec(noinline) void *AdcwAnmManager::PreloadAnm(int slot, const char *name) {
    void *res = g_AdcwAnmMgr->slots4e90c0[slot & 7];
    DWORD t = GetTickCount();
    g_Sink += (int)res + (int)name[0] + (int)t;
    return res;
}

__declspec(noinline) int AdcwChain::RegisterCalc(AdcwChainElem *elem, int priority) {
    int result = 0;
    AdcwChain *chain = g_AdcwChain;
    if (elem->onRegC != NULL) {
        result = ((int (__fastcall *)(void *))elem->onRegC)(elem->arg20);
        elem->onRegC = NULL;
    }
    AdcwLockCs(0);
    AdcwChainNode *cursor = &chain->calcChain.node14;
    elem->prio0 = (short)priority;
    while (cursor->next4 != NULL) {
        if (cursor->next4->elem0->prio0 >= priority) {
            break;
        }
        cursor = cursor->next4;
    }
    AdcwChainNode *node = &elem->node14;
    if (cursor->next4 != NULL) {
        node->next4 = cursor->next4;
        cursor->next4->prev8 = node;
    }
    cursor->next4 = node;
    node->prev8 = cursor;
    AdcwUnlockCs(0);
    return result;
}

__declspec(noinline) int AdcwChain::RegisterDraw(AdcwChainElem *elem, int priority) {
    int result = 0;
    AdcwChain *chain = g_AdcwChain;
    if (elem->onRegC != NULL) {
        result = ((int (__fastcall *)(void *))elem->onRegC)(elem->arg20);
        elem->onRegC = NULL;
    }
    AdcwLockCs(0);
    AdcwChainNode *cursor = &chain->drawChain.node14;
    elem->prio0 = (short)priority;
    while (cursor->next4 != NULL) {
        if (cursor->next4->elem0->prio0 >= priority) {
            break;
        }
        cursor = cursor->next4;
    }
    AdcwChainNode *node = &elem->node14;
    if (cursor->next4 != NULL) {
        node->next4 = cursor->next4;
        cursor->next4->prev8 = node;
    }
    cursor->next4 = node;
    node->prev8 = cursor;
    AdcwUnlockCs(0);
    return result;
}

__declspec(noinline) void AdcwThreadInf::Stop() {
    if (handle4 == NULL) return;
    DWORD t = GetTickCount();
    g_Sink += (int)handle4 + (int)t;
    handle4 = NULL;
}

__declspec(noinline) int __fastcall AdcwBulletCalcCb(void *arg) { g_Sink += (int)arg; return 1; }
__declspec(noinline) int __fastcall AdcwBulletDrawCb(void *arg) { g_Sink += (int)arg + 1; return 1; }
__declspec(noinline) int __fastcall AdcwMotherCalcCb(void *arg) { g_Sink += (int)arg + 2; return 1; }
__declspec(noinline) int __fastcall AdcwMotherDrawCb(void *arg) { g_Sink += (int)arg + 3; return 1; }
__declspec(noinline) unsigned int __stdcall AdcwMotherThreadProc(void *arg) { g_Sink += (int)arg + 4; return 0; }

typedef void (__stdcall *ClearTexFn)(IDirect3DTexture9 **);
volatile ClearTexFn g_ClearTexPtr = &AdcwClearTextureBlackOpaque;

typedef int (__stdcall AdcwMother::*RegMotherFn)();
volatile RegMotherFn g_RegMotherPtr = &AdcwMother::RegisterChainsAndThread;

} // namespace th128

extern "C" int __stdcall AnmDrawChainWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) {
        int a = ((AdcwAnmManager *)arg)->DrawMeshStrip((AdcwAnmVm *)(arg + 4), (const void *)(arg + 8), arg + 12);
        int b = g_AdcwAnmMgr->DrawMeshStrip((AdcwAnmVm *)(arg + g_Sink), (const void *)(arg + 16), arg + 20);
        return a + b;
    }
    if (selector == 2) return ((AdcwAnmManager *)arg)->DrawTriangleStrip2D((const void *)(arg + 4), arg + 8);
    if (selector == 3) return ((AdcwAnmManager *)arg)->DrawTriangleFan2D((const void *)(arg + 4), arg + 8);
    if (selector == 4) { ((AdcwAnmManager *)arg)->CopyBackBufferToTexture((const AdcwCopyBackBufReq *)(arg + 4)); return g_Sink; }
    if (selector == 5) { ((AdcwAnmManager *)arg)->CopyTextureToTexture(arg, arg + 1, arg + 2, arg + 3, (const RECT *)(arg + 4), (const RECT *)(arg + 8)); return g_Sink; }
    if (selector == 6) { g_ClearTexPtr((IDirect3DTexture9 **)arg); return g_Sink; }
    if (selector == 7) return ((AdcwBulletMgr *)arg)->RegisterChains();
    if (selector == 8) return (((AdcwMother *)arg)->*g_RegMotherPtr)();
    if (selector == 9) return (int)g_AdcwAnmMgr->PreloadAnm(arg, (const char *)(arg + 4));
    return 0;
}
