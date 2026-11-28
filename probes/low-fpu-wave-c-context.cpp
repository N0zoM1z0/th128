#include "LowFpuWaveC.hpp"

namespace th128 {

float LfcTimer::g_LfcGameSpeed = 1.0f;
LfcGameGlobal *g_LfcGameGlobal = NULL;
LfcPopupMgr *g_LfcPopupMgr = NULL;
LfcAnmManager *g_LfcAnmManager = NULL;
LfcD3dDevice *g_LfcD3dDevice = NULL;
void *g_LfcSurfaceA = NULL;
void *g_LfcSurfaceB = NULL;
int g_LfcViewportX = 0;
int g_LfcViewportY = 0;
int g_LfcViewportW = 0;
int g_LfcViewportH = 0;
DWORD g_LfcClearColor = 0;
LfcAbilityMgr *g_LfcAbilityMgr = NULL;

static volatile int g_Sink = 0;

LfcEndingSubBase::~LfcEndingSubBase() { g_Sink += 1; }
LfcEndingSub::~LfcEndingSub() { g_Sink += 1; }
int LfcEndingSub::V1() { g_Sink += 2; return g_Sink; }
int LfcEndingSub::Load(int arg) { g_Sink += arg; return g_Sink; }

__declspec(noinline) LfcAnmVm::LfcAnmVm() {
    int x = g_Sink;
    g_Sink = x + *(int *)this;
}

__declspec(noinline) LfcAnmVm *LfcAnmManager::GetVmById(int id) {
    int *p = (int *)this;
    int s = g_Sink;
    for (int i = 0; i < 4; i++) {
        s += p[i] ^ id;
    }
    g_Sink = s;
    return (LfcAnmVm *)(int)g_Sink;
}

LfcAbilityMgr::~LfcAbilityMgr() { g_Sink += 3; }

LfcLaser::~LfcLaser() { g_Sink += 4; }
void LfcLaser::V1() { g_Sink += 5; }
void LfcLaser::V2() { g_Sink += 6; }
void LfcLaser::V3() { g_Sink += 7; }
void LfcLaser::V4() { g_Sink += 8; }
void LfcLaser::OnDraw() { g_Sink += 9; }

LfcEclVm::~LfcEclVm() { g_Sink += 10; }

__declspec(noinline) void *LfcSptResourceInf::FindSub(const char *name) {
    if (this == NULL) {
        return NULL;
    }
    int r = *(int *)this;
    const char *const *p = &name;
    g_Sink += r + (int)p;
    return (void *)(int)g_Sink;
}

__declspec(noinline) void LfcRegisterCalc(LfcChainNode *node, int prio) {
    int a = g_Sink;
    Sleep(1);
    g_Sink += a + node->priority + prio + (int)node;
}

__declspec(noinline) void LfcRegisterDraw(LfcChainNode *node, int prio) {
    int a = g_Sink;
    Sleep(1);
    g_Sink += a + node->priority + prio + (int)node;
}

__declspec(noinline) void LfcFlushAnmSpriteBatch(LfcAnmManager *mgr) {
    g_Sink += *(int *)mgr;
    Sleep(1);
    g_Sink += (int)mgr;
}

__declspec(noinline) void LfcResetEnemySub(void *sub) {
    g_Sink += *(int *)sub;
    if (g_Sink == 126) Sleep(1);
}

__declspec(noinline) void LfcEndingOnCalc() { g_Sink += 11; }
__declspec(noinline) void LfcEndingOnDraw() { g_Sink += 12; }
__declspec(noinline) void LfcObj36470OnCalc() { g_Sink += 13; }
__declspec(noinline) void LfcObj36470OnDraw() { g_Sink += 14; }

static void *volatile g_Keep[] = {
    (void *)&LfcPauseDrawTargetA,
    (void *)&LfcPauseDrawTargetB,
};

} // namespace th128

extern "C" int __stdcall LowFpuWaveCProbe(int a, int b) {
    using namespace th128;
    int r = (int)g_Keep[a & 1];

    LfcEnding *e1 = (LfcEnding *)(int)a;
    LfcEnding *e2 = (LfcEnding *)(int)(a + 4);
    r += e1->Init(b);
    r += e2->Init(b + 1);

    LfcObj36470 *o1 = (LfcObj36470 *)(int)a;
    LfcObj36470 *o2 = (LfcObj36470 *)(int)(a + 4);
    r += o1->Register();
    r += o2->Register();

    LfcSpawnPopupNumber((const LfcVec3 *)(int)a, b, a ^ b);
    LfcSpawnPopupNumber((const LfcVec3 *)(int)(a + 12), b + 1, a + b);

    LfcHud *h1 = (LfcHud *)(int)a;
    LfcHud *h2 = (LfcHud *)(int)(a + 4);
    h1->SetBarFill((float)b, a);
    h2->SetBarFill((float)(b + 1), a + 1);
    LfcAnmManager *amgr = (LfcAnmManager *)(int)a;
    r += (int)amgr->GetVmById(b);
    LfcFlushAnmSpriteBatch(amgr);
    LfcRegisterCalc((LfcChainNode *)(int)a, b);
    LfcRegisterDraw((LfcChainNode *)(int)a, b);

    r += LfcPauseDrawTargetA();
    r += LfcPauseDrawTargetB();

    LfcAbilityMgr *am1 = new LfcAbilityMgr();
    LfcAbilityMgr *am2 = new LfcAbilityMgr();
    LfcAnmVm *vmArr = new LfcAnmVm[a & 3];
    r += (int)am1 + (int)am2 + (int)vmArr;

    int (LfcLaserMgr::*volatile pOnDraw)() = &LfcLaserMgr::OnDraw;
    LfcLaserMgr *lm1 = (LfcLaserMgr *)(int)a;
    r += (lm1->*pOnDraw)();

    LfcAnmVm *vm1 = (LfcAnmVm *)(int)a;
    LfcAnmVm *vm2 = (LfcAnmVm *)(int)(a + 4);
    vm1->SetEffectState(a, (unsigned char)b, (unsigned char)(a + 1), (unsigned char)(b + 1));
    vm2->SetEffectState(b, (unsigned char)a, (unsigned char)(b + 2), (unsigned char)(a + 2));

    LfcSptResourceInf *sr1 = (LfcSptResourceInf *)(int)a;
    LfcSptResourceInf *sr2 = (LfcSptResourceInf *)(int)(a + 4);
    r += (int)sr1->CreateVm((const char *)(int)b);
    r += (int)sr2->CreateVm((const char *)(int)(b + 1));

    return r;
}
