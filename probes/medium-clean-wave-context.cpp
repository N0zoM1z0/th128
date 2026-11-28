#include "../src/MediumCleanWave.hpp"

namespace th128 {

volatile int g_McwSink = 0;

McwD3dDevice *g_McwDevice = NULL;
McwAnmManager *g_McwAnmMgr = NULL;
McwChain *g_McwChain = NULL;
unsigned int g_McwFlags = 0;
CRITICAL_SECTION g_McwChainCs;
unsigned char g_McwChainCsDepth = 0;
CRITICAL_SECTION g_McwRngCs;
unsigned char g_McwRngCsDepth = 0;
unsigned short g_McwRngSeed = 0;
unsigned int g_McwRngCount = 0;
McwBulletManager *g_McwBulletMgr = NULL;
McwSoundTest *g_McwSoundTest = NULL;
McwItemManager *g_McwItemMgr = NULL;

__declspec(noinline) void McwAnmManager::FlushSpriteBatch() {
    if (this->batchCount != 0) {
        g_McwDevice->DrawPrimitiveUP(4, this->batchCount, this, 0x1c);
        this->batchCount = 0;
    }
}

__declspec(noinline) void McwAnmManager::ApplyState(McwAnmVm *vm) {
    this->fvfState = 2;
    g_McwDevice->SetRenderState(14, vm->flags);
    g_McwDevice->SetTexture(0, vm->anmRes);
}

__declspec(noinline) void McwAnmManager::MarkResourceVmsForDelete(void *res) {
    McwVmNode *node = this->worldListHead;
    while (node != NULL) {
        if (node->vm->anmRes == res) {
            node->vm->flags |= 0x20000000;
        }
        node = node->next;
    }
}

__declspec(noinline) void __fastcall McwChainCut(McwChain *chain, McwChainElem *elem) {
    if (chain->head == elem) {
        chain->head = elem->next;
    }
    if (chain->tail == elem) {
        chain->tail = elem->prev;
    }
    if (elem->prev != NULL) {
        elem->prev->next = elem->next;
    }
    if (elem->next != NULL) {
        elem->next->prev = elem->prev;
    }
    if (g_McwSink == 99998) {
        g_McwFlags ^= 1;
        g_McwChain = NULL;
    }
}

__declspec(noinline) McwBullet::~McwBullet() {
    g_McwSink += data[0];
}

__declspec(noinline) McwItem::~McwItem() {
    g_McwSink += data[0];
}

__declspec(noinline) McwMemStream::~McwMemStream() {
    g_McwSink += (int)size;
}

__declspec(noinline) void McwMemStream::Close() {
    g_McwSink += (int)this;
}

__declspec(noinline) static McwBulletManager *McwGetBM(int k) { return (McwBulletManager *)k; }
__declspec(noinline) static McwSoundTest *McwGetST(int k) { return (McwSoundTest *)k; }
__declspec(noinline) static McwItemManager *McwGetIM(int k) { return (McwItemManager *)k; }
__declspec(noinline) static McwScoreHeader *McwGetSH(int k) { return (McwScoreHeader *)k; }

} // namespace th128

static void *volatile g_McwKeep[4];
static bool (th128::McwMemStream::*volatile g_McwLoadResFn)(const char *, int) = NULL;

extern "C" int __stdcall MediumCleanWaveProbe(int a, int b) {
    using namespace th128;
    volatile int k = a ^ b;
    g_McwKeep[0] = (void *)&McwChainCut;

    McwAnmManager *am = (McwAnmManager *)(int)k;
    McwAnmVm *vm = (McwAnmVm *)(int)a;
    g_McwSink += am->DrawAnmVmVertices(vm, (const void *)(int)b, k + 3);
    g_McwSink += (am + 1)->DrawAnmVmVertices(vm + 1, (const void *)(int)(b + 4), k + 5);

    McwTextImage *ti = (McwTextImage *)(int)k;
    McwD3dSurface *surf = (McwD3dSurface *)(int)a;
    g_McwSink += ti->UploadToSurface(surf);
    g_McwSink += (ti + 1)->UploadToSurface(surf + 1);
    g_McwSink += ti->DownloadFromSurface(surf);
    g_McwSink += (ti + 1)->DownloadFromSurface(surf + 1);

    McwSoundManager *sm = (McwSoundManager *)(int)k;
    g_McwSink += sm->FindBgmIndex((const char *)(int)a);
    g_McwSink += (sm + 1)->FindBgmIndex((const char *)(int)b);

    McwStageObj *so = (McwStageObj *)(int)k;
    so->PauseAllStageAnmsAndEnemies();
    (so + 1)->PauseAllStageAnmsAndEnemies();

    delete McwGetBM(k);
    delete McwGetBM(k + 1);

    delete McwGetST(k);
    delete McwGetST(k + 1);

    delete McwGetIM(k);
    delete McwGetIM(k + 1);

    McwGetSH(k)->InitHeader();
    McwGetSH(k + 1)->InitHeader();

    g_McwLoadResFn = &McwMemStream::LoadFromResource;
    McwMemStream *ms = (McwMemStream *)(int)k;
    g_McwSink += (ms->*g_McwLoadResFn)((const char *)(int)a, b);

    return g_McwSink;
}
