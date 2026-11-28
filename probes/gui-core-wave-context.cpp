#include "../src/GuiCoreWave.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace th128 {

GcwChain *g_GcwChain = NULL;
GcwAnmManager *g_GcwAnmMgr = NULL;
GcwErrorCtx g_GcwErrorCtx;
unsigned int g_GcwSupFlags = 0;
CRITICAL_SECTION g_GcwSupCs0;
unsigned char g_GcwSupCsDepth0 = 0;
int g_GcwGrazeA = 0;
int g_GcwGrazeB = 0;
GcwEnemyFlagMgr *g_GcwEnemyFlagMgr = NULL;
GcwSpellCard *g_GcwSpellCard = NULL;
GcwPlayer *g_GcwPlayer = NULL;
GcwReplayMgr *g_GcwReplayMgr = NULL;
GcwScoreFile *g_GcwScoreFile = NULL;
GcwPopupMgr *g_GcwPopupMgr = NULL;
int g_GcwScore = 0;
GcwEnemyMgr *g_GcwEnemyMgr = NULL;
unsigned int g_GcwGameFlags = 0;
void *g_GcwMsgBufGlobal = NULL;
static volatile int g_Sink = 0;

static __declspec(noinline) void GcwLockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx + (int)t;
}

static __declspec(noinline) void GcwUnlockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx + (int)t + 7;
}

__declspec(noinline) int GcwChain::RegisterCalc(GcwChainElem *elem, int priority) {
    int result = 0;
    GcwChain *chain = g_GcwChain;
    if (elem->onRegC != NULL) {
        result = elem->onRegC(elem->arg20);
        elem->onRegC = NULL;
    }
    GcwLockCs(0);
    GcwChainNode *cursor = &chain->calcChain.node14;
    elem->priority0 = priority;
    while (cursor->next != NULL) {
        if (cursor->next->owner->priority0 >= priority) break;
        cursor = cursor->next;
    }
    GcwChainNode *node = &elem->node14;
    if (cursor->next != NULL) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    GcwUnlockCs(0);
    return result;
}

__declspec(noinline) int GcwChain::RegisterDraw(GcwChainElem *elem, int priority) {
    int result = 0;
    GcwChain *chain = g_GcwChain;
    if (elem->onRegC != NULL) {
        result = elem->onRegC(elem->arg20);
        elem->onRegC = NULL;
    }
    GcwLockCs(0);
    GcwChainNode *cursor = &chain->drawChain.node14;
    elem->priority0 = priority;
    while (cursor->next != NULL) {
        if (cursor->next->owner->priority0 >= priority) break;
        cursor = cursor->next;
    }
    GcwChainNode *node = &elem->node14;
    if (cursor->next != NULL) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    GcwUnlockCs(0);
    return result;
}

__declspec(noinline) void __fastcall GcwChainCutImpl(GcwChain *chain, GcwChainElem *elem) {
    if (elem == NULL) return;
    GcwChainNode *node = &chain->calcChain.node14;
    while (node != NULL) {
        if (node->owner == elem) goto found;
        node = node->next;
    }
    node = &chain->drawChain.node14;
    while (node != NULL) {
        if (node->owner == elem) goto found;
        node = node->next;
    }
    return;
found:
    if (chain->current == node) chain->current = node->next;
    if (node->prev == NULL) return;
    if (node->next != NULL) node->next->prev = node->prev;
    if (node->prev != NULL) node->prev->next = node->next;
    node->next = NULL;
    node->prev = NULL;
    elem->cb8 = NULL;
    if ((elem->flags4 & 1u) != 0) {
        g_Sink += (int)elem;
    }
}

typedef void (__fastcall *GcwCutImplFn)(GcwChain *, GcwChainElem *);
volatile GcwCutImplFn g_GcwCutImplPtr = &GcwChainCutImpl;

__declspec(noinline) void GcwAnmResource::Release() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) void GcwAnmVm::ResetForReuse() {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t + 1;
}

__declspec(noinline) void GcwAnmVm::SetSprite(GcwAnmResource *res, int spriteId) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)res + spriteId + (int)t;
}

__declspec(noinline) GcwAnmResource *GcwAnmManager::PreloadAnm(int slot, const char *name) {
    GcwAnmResource *res = g_GcwAnmMgr->LoadAnm(slot, name);
    if (res == NULL) return NULL;
    res->loading124 = 1;
    while ((g_GcwSupFlags & 0x180u) == 0) {
        Sleep(1);
        if (res->loading124 == 0) break;
    }
    GcwDebugLog("::preloadAnimEnd : %s\n", name);
    return res;
}

__declspec(noinline) GcwAnmResource *GcwAnmManager::LoadAnm(int slot, const char *name) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)this + slot + (int)name + (int)t1 + 5;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)name;
    return (GcwAnmResource *)g_Sink;
}

static __declspec(noinline) GcwAnmVm *GcwGetVmHelper(GcwAnmManager *mgr, GcwAnmId id) {
    DWORD t = GetTickCount();
    g_Sink += (int)mgr + (int)id.id + (int)t;
    return (GcwAnmVm *)g_Sink;
}

__declspec(noinline) void GcwAnmManager::StopVmById(GcwAnmId id, short interrupt) {
    GcwAnmVm *vm = GcwGetVmHelper(g_GcwAnmMgr, id);
    if (vm == NULL) return;
    DWORD t = GetTickCount();
    g_Sink += (int)vm + interrupt + (int)t;
}

__declspec(noinline) const char *__cdecl GcwErrorCtx::Log(const char *fmt, ...) {
    char local[0x2000];
    va_list args;
    GcwLockCs(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    GcwUnlockCs(3);
    return fmt;
}

__declspec(noinline) GcwStageBgMini::~GcwStageBgMini() {
    if (dummy != 0) {
        free((void *)dummy);
        dummy = 0;
    }
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)t;
}

__declspec(noinline) void __stdcall GcwPopupMgr::Show(int kind, int value) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + kind + value + (int)t;
}

GcwEclConcrete::~GcwEclConcrete() {
    vtable0 = (void *)0x4a26dc;
    if (mainBuf8c != NULL) {
        free(mainBuf8c);
        mainBuf8c = NULL;
    }
}

__declspec(noinline) void GcwDeleteVmByIdPtr(GcwAnmId *id) {
    GcwAnmVm *vm = GcwGetVmHelper(g_GcwAnmMgr, *id);
    if (vm != NULL) {
        vm->flags4b0 |= 0x20000000u;
    }
    id->id = 0;
}

int g_GcwGrazeDiv = 100;

__declspec(noinline) int GcwAddGrazeOrPoints(int amount) {
    if (g_GcwGrazeA >= g_GcwGrazeB) return 0;
    int prev = g_GcwGrazeA;
    int next = prev + amount;
    g_GcwGrazeA = next;
    if (next > g_GcwGrazeB) {
        next = g_GcwGrazeB;
        g_GcwGrazeA = next;
    }
    return ((next - amount) / g_GcwGrazeDiv) != (next / g_GcwGrazeDiv);
}

static short g_GcwSoundTable[64];
__declspec(noinline) void GcwPlaySfx(int id) {
    int v = g_GcwSoundTable[id * 10];
    if (v >= 0) g_Sink ^= v + id;
}

__declspec(noinline) void __cdecl GcwDebugLog(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)fmt + (int)t;
}

__declspec(noinline) void __stdcall GcwEnemyMgrResetList(GcwEnemyMgr *mgr) {
    DWORD t = GetTickCount();
    g_Sink += (int)mgr + (int)t;
}

__declspec(noinline) void GcwDestroyMsgObj(void *obj) {
    DWORD t = GetTickCount();
    g_Sink += (int)obj + (int)t;
}

} // namespace th128

extern "C" int __stdcall GuiCoreWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((GcwAsciiInf *)arg)->Init();
    if (selector == 2) { ((GcwEnemyFlagMgr *)arg)->Cleanup(); return g_Sink; }
    if (selector == 3) { GcwSpellCard::Finish(); return g_Sink; }
    if (selector == 4) return ((GcwEclFile *)arg)->LoadDeps((const unsigned int *)(arg + 4));
    if (selector == 5) { ((GcwEnemyMgr *)arg)->Cleanup(); return g_Sink; }
    if (selector == 6) { ((GcwGui *)arg)->Reset(); return g_Sink; }
    if (selector == 7) {
        g_GcwAnmMgr->PreloadAnm(arg, (const char *)arg);
        g_GcwAnmMgr->LoadAnm(arg, (const char *)arg);
        GcwAnmId id; id.id = (unsigned int)arg;
        g_GcwAnmMgr->StopVmById(id, (short)arg);
        g_GcwGrazeDiv = arg;
        g_Sink += GcwAddGrazeOrPoints(arg);
        GcwDeleteVmByIdPtr(&id);
        GcwChainCutImpl((GcwChain *)arg, (GcwChainElem *)(arg + 4));
        return g_GcwChain->RegisterCalc((GcwChainElem *)arg, arg) + g_GcwChain->RegisterDraw((GcwChainElem *)arg, arg);
    }
    return 0;
}
