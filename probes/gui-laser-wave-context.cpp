#include "../src/GuiLaserWave.hpp"

namespace th128 {

unsigned int g_GlwSupFlags = 0;
CRITICAL_SECTION g_GlwSupCs0;
unsigned char g_GlwSupCsDepth0 = 0;
GlwChainMgr *g_GlwChainMgr = NULL;
GlwSpellCard *g_GlwSpellCard = NULL;
GlwGui *g_GlwGui = NULL;
unsigned int g_GlwGameFlags = 0;
GlwAnmMgr *g_GlwAnmMgr = NULL;
int g_GlwFreezeGauge = 0;
int g_GlwFreezeEmpty = 0;
int g_GlwMotivGauge = 0;
GlwPopupMgr *g_GlwPopupMgr = NULL;
struct GlwInputKeyBlock {
    unsigned int keys[4];
    unsigned short key10;
};
unsigned int g_GlwInputKeys[5];
GlwLaserItemMgr *g_GlwLaserItemMgr = NULL;
GlwLaserMgr *g_GlwLaserMgr = NULL;
static volatile int g_Sink = 0;

__declspec(noinline) void __fastcall GlwChainCutImpl(GlwChainMgr *mgr, GlwChainElem *elem) {
    DWORD t = GetTickCount();
    g_Sink += (int)mgr + (int)elem + (int)t;
}
typedef void (__fastcall *GlwCutFn)(GlwChainMgr *, GlwChainElem *);
volatile GlwCutFn g_GlwCutPtr = &GlwChainCutImpl;

__declspec(noinline) void GlwDestroyMsgObj(void *obj) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)obj + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)obj;
}
__declspec(noinline) void GlwReleaseAnm(void *anm) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)anm + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)anm;
}
static short g_GlwSoundTbl[64];
__declspec(noinline) void GlwPlaySfx(int id) { int v = g_GlwSoundTbl[id * 10]; if (v >= 0) g_Sink ^= v + id; }
__declspec(noinline) void __stdcall GlwPopupMgr::Show(int kind, int value) {
    DWORD t = GetTickCount();
    g_Sink += (int)this + kind + value + (int)t;
}
__declspec(noinline) void __stdcall GlwGuiUpdateMeterAnms(GlwPopupMgr *gui) {
    DWORD t = GetTickCount();
    g_Sink += (int)gui + (int)t;
}
__declspec(noinline) GlwLaserElem::~GlwLaserElem() { g_Sink += (int)this; }
__declspec(noinline) GlwLaserType0::GlwLaserType0() { id80 = 0; g_Sink += (int)this; }
__declspec(noinline) GlwLaserType1::GlwLaserType1() { id80 = 1; g_Sink += (int)this; }
__declspec(noinline) GlwLaserType2::GlwLaserType2() { id80 = 2; g_Sink += (int)this; }
__declspec(noinline) GlwLaserType3::GlwLaserType3() { id80 = 3; g_Sink += (int)this; }

} // namespace th128

extern "C" int __stdcall GuiLaserWaveProbe(int selector, int arg) {
    using namespace th128;
    int a = 0, b = 0;
    if (selector == 1) { GlwDecodeSpellBonus(&a, &b); return a + b; }
    if (selector == 2) { GlwGui::ReleaseMsgAndSlot27(); return g_Sink; }
    if (selector == 3) { GlwAddFreezeGauge(arg); return g_Sink; }
    if (selector == 4) { GlwAddMotivGauge(arg); return g_Sink; }
    if (selector == 5) { ((GlwConfig *)arg)->ResetDefaults(); return g_Sink; }
    if (selector == 6) { ((GlwLaserItemMgr *)arg)->~GlwLaserItemMgr(); return g_Sink; }
    if (selector == 7) return GlwLaserMgr::Spawn(arg, (void *)(arg + 4));
    if (selector == 8) { ((GlwLaserInfinite *)arg)->Slot01(); return g_Sink; }
    if (selector == 9) { GlwPlaySfx(arg); return g_Sink; }
    return 0;
}
