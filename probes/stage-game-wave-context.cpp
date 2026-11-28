#include "../src/StageGameWave.hpp"

namespace th128 {

unsigned int g_SgwSupFlags = 0;
CRITICAL_SECTION g_SgwSupCs0;
unsigned char g_SgwSupCsDepth0 = 0;
SgwChainMgr *g_SgwChainMgr = NULL;
SgwAnmMgr *g_SgwAnmMgr = NULL;
SgwStage *g_SgwStage = NULL;
SgwEnemyFlagMgr *g_SgwEnemyFlagMgr = NULL;
SgwBulletMgr *g_SgwBulletMgr = NULL;
SgwScoreState *g_SgwScoreState = NULL;
SgwGameMgr *g_SgwGameMgr = NULL;
SgwEnemyMgr *g_SgwEnemyMgr = NULL;
SgwItemMgr *g_SgwItemMgr = NULL;
unsigned int g_SgwInputHeld = 0;
int g_SgwSceneNext = 0;
static volatile int g_Sink = 0;

__declspec(noinline) void __fastcall SgwChainCutImpl(SgwChainMgr *mgr, SgwChainElem *elem) {
    DWORD t = GetTickCount();
    g_Sink += (int)mgr + (int)elem + (int)t;
}
typedef void (__fastcall *SgwCutFn)(SgwChainMgr *, SgwChainElem *);
volatile SgwCutFn g_SgwCutPtr = &SgwChainCutImpl;

__declspec(noinline) SgwAnmVm::SgwAnmVm() { id0 = 0; g_Sink += (int)this; }
__declspec(noinline) void SgwAnmVm::ResetForReuse() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t; }
__declspec(noinline) void SgwAnmVm::Update() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t + 1; }
__declspec(noinline) void SgwAnmResource::BindVmScript(SgwAnmVm *vm, int scriptId) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + (int)vm + scriptId + (int)t;
}
__declspec(noinline) SgwAnmVm *SgwAnmMgr::GetVmById(unsigned int id) {
    if (id == 0) return NULL;
    for (SgwAnmNode *n = world8b9704; n != NULL; n = n->next) if (n->vm->id0 == id) return n->vm;
    for (SgwAnmNode *n = ui8b970c; n != NULL; n = n->next) if (n->vm->id0 == id) return n->vm;
    return NULL;
}
__declspec(noinline) void SgwAnmMgr::StopVmById(unsigned int id, short intr) {
    SgwAnmVm *vm = g_SgwAnmMgr->GetVmById(id);
    if (vm == NULL) return;
    DWORD t = GetTickCount();
    g_Sink += (int)vm + intr + (int)t;
}
__declspec(noinline) void SgwAnmMgr::MarkResourceVmsForDelete(void *res) {
    for (SgwAnmNode *n = world8b9704; n != NULL; ) {
        SgwAnmNode *next = n->next;
        if (n->vm->res42c == res) {
            n->vm->flags4b0 |= 0x20000000u;
        }
        n = next;
    }
    for (SgwAnmNode *n = ui8b970c; n != NULL; ) {
        SgwAnmNode *next = n->next;
        if (n->vm->res42c == res) {
            n->vm->flags4b0 |= 0x20000000u;
        }
        n = next;
    }
}
__declspec(noinline) void SgwTimer::Increment() {
    prev = current;
    currentF += 1.0f;
    current = (int)currentF;
}
__declspec(noinline) int __stdcall SgwEnemyFlagMgr::Register() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t; return g_Sink; }
__declspec(noinline) void SgwEnemyFlagMgr::Cleanup() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t + 2; }
__declspec(noinline) SgwBullet::~SgwBullet() { g_Sink += (int)this; }
__declspec(noinline) void SgwMotionState::UpdateVelocity() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t; }
__declspec(noinline) void SgwMotionState::UpdatePos() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t + 3; }
__declspec(noinline) int SgwEffect::CheckExpire() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t; return g_Sink; }
__declspec(noinline) int SgwMenuCursor::Move(int delta) {
    if (count > 0) {
        int tries = flagC8;
    retry:
        cur += delta;
        while (cur >= count) {
            if (flagCC) cur -= count;
            else cur = count - 1;
        }
        while (cur < 0) {
            if (flagCC) cur += count;
            else cur = 0;
        }
        for (int i = 0; i < tries; ++i) {
            if (padC[i] == cur) goto retry;
        }
    }
    return cur;
}
__declspec(noinline) int __stdcall SgwEndingInner::CheckDone() { DWORD t = GetTickCount(); g_Sink += (int)this + (int)t; return g_Sink; }

static short g_SgwSoundTbl[64];
__declspec(noinline) void SgwPlaySfx(int id) { int v = g_SgwSoundTbl[id * 10]; if (v >= 0) g_Sink ^= v + id; }
__declspec(noinline) void SgwCreatePlayer() { g_Sink += (int)GetTickCount(); }
__declspec(noinline) void SgwCreateBulletMgr() { g_Sink += (int)GetTickCount() + 1; }
__declspec(noinline) void SgwCreateLaserMgr() { g_Sink += (int)GetTickCount() + 2; }
__declspec(noinline) void SgwCreateSub443b40() { g_Sink += (int)GetTickCount() + 3; }
__declspec(noinline) void __stdcall SgwCreateEnemyMgr(const char *eclName) { g_Sink += (int)eclName + (int)GetTickCount(); }
__declspec(noinline) void SgwCreateGui() { g_Sink += (int)GetTickCount() + 4; }

} // namespace th128

extern "C" int __stdcall StageGameWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((SgwStage *)arg)->UpdateBgMeshes();
    if (selector == 2) { ((SgwStage *)arg)->ClearBgVms(); return g_Sink; }
    if (selector == 3) { SgwStage::EnableAndResetMeshes(); return g_Sink; }
    if (selector == 4) return (int)SgwEnemyFlagMgr::Create();
    if (selector == 5) { ((SgwBulletMgr *)arg)->~SgwBulletMgr(); return g_Sink; }
    if (selector == 6) return ((SgwEffect *)arg)->Update();
    if (selector == 7) { ((SgwEnemyMgr *)arg)->ResetList(); return g_Sink; }
    if (selector == 8) return (int)new SgwGameMgr();
    if (selector == 9) return SgwGameMgr::InitSubsystems();
    if (selector == 10) { ((SgwItemMgr *)arg)->~SgwItemMgr(); return g_Sink; }
    if (selector == 11) { ((SgwMeshStrip *)arg)->CopyVertices(); return g_Sink; }
    if (selector == 12) return ((SgwEnding *)arg)->Step();
    if (selector == 13) {
        g_SgwAnmMgr->StopVmById((unsigned int)arg, (short)arg);
        ((SgwAnmMgr *)arg)->MarkResourceVmsForDelete((void *)(arg + 4));
        SgwPlaySfx(arg);
        return g_Sink;
    }
    return 0;
}
