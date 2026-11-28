#include "../src/ItemWave1.hpp"

#include <windows.h>

namespace th128 {

float Iw1Timer::g_Iw1TimeScale = 1.0f;
const int g_Iw1FreezeThresholds[2][15] = {
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
    { 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30 }
};
Iw1Globals g_Iw1Globals;
Iw1Gui *g_Iw1Gui = 0;
void *g_Iw1Player = 0;
Iw1ItemManager *g_Iw1ItemManager = 0;
Iw1ChainManager *g_Iw1ChainManager = 0;
unsigned int g_Iw1SupervisorFlags = 0;
CRITICAL_SECTION g_Iw1ChainCritSec;
unsigned char g_Iw1ChainCritSecDepth = 0;

static volatile int g_sink = 0;
static volatile float g_fsink = 0.0f;

Iw1Item::Iw1Item() {
    g_sink += *(int *)this;
    Sleep(1);
}

Iw1Item::~Iw1Item() {
    g_sink += *(int *)this;
    Sleep(1);
}

void __fastcall Iw1ChainManager::Cut(void *elem) {
    g_sink += *(int *)this + *(int *)elem;
    Sleep(1);
    g_sink += *(int *)this + *(int *)elem;
}

void Iw1Gui::ShowPopup(int kind, int a, int b) {
    int *pa = &a;
    int *pb = &b;
    int *p1 = (int *)g_Iw1ChainManager;
    g_sink += *p1 + *(int *)this + kind + *pa + *pb;
    Sleep(1);
    g_sink += *p1 + *(int *)this;
}

void Iw1Gui::UpdateMeterAnms() {
    Iw1Gui *self = this;
    Iw1Gui **ps = &self;
    g_sink += *(int *)(*ps);
    Sleep(1);
}

void PlaySfx(int id) {
    g_sink += id;
    Sleep(1);
}

void PlaySfxWithPan(int id, float pan) {
    float *pp = &pan;
    g_sink += id;
    g_fsink += *pp;
    Sleep(1);
}

void AddScorePopup(const Iw1Vec3 *pos, int value, int color) {
    int *pv = &value;
    int *pc = &color;
    int *p1 = (int *)g_Iw1ChainManager;
    g_sink += *p1 + (int)pos->x + *pv + *pc;
    Sleep(1);
    g_sink += *p1 + (int)pos->x;
}

void TriggerItemBonus(void *player) {
    int *p1 = (int *)g_Iw1ChainManager;
    g_sink += *p1 + *(int *)player;
    Sleep(1);
    g_sink += *p1 + *(int *)player;
}

void RegisterCalcChain(Iw1ChainElem *elem, int prio) {
    int *p1 = (int *)g_Iw1Gui;
    int *p2 = (int *)g_Iw1ChainManager;
    g_sink += *p1 + *p2 + *(int *)elem + prio;
    Sleep(1);
    g_sink += *p1 + *p2 + *(int *)elem + prio;
}

void RegisterDrawChain(Iw1ChainElem *elem, int prio) {
    int *p1 = (int *)g_Iw1Gui;
    int *p2 = (int *)g_Iw1ChainManager;
    g_sink += *p1 + *p2 + *(int *)elem + prio;
    Sleep(1);
    g_sink += *p1 + *p2 + *(int *)elem + prio;
}

void StartLoaderThread(void (*fn)()) {
    int *p1 = (int *)g_Iw1Gui;
    g_sink += *p1 + (int)(intptr_t)fn;
    Sleep(1);
    g_sink += *p1 + (int)(intptr_t)fn;
}

void HelpLoaderEntry() {
    g_sink += 7;
}

int __cdecl HelpCalcCb(void *arg) {
    return *(int *)arg + g_sink;
}

int __cdecl HelpDrawCb(void *arg) {
    return *(int *)arg + g_sink + 1;
}

} // namespace th128

static __declspec(noinline) th128::Iw1ItemManager *GetItemMgr() {
    Sleep(1);
    return th128::g_Iw1ItemManager;
}

extern "C" int __stdcall ItemWave1Probe(int a, int b) {
    using namespace th128;
    void (__fastcall Iw1ChainManager::*fcut)(void *) = &Iw1ChainManager::Cut;
    void (__stdcall *fm)(int) = &AddMotivation;
    void (__stdcall *fp)(int) = &AddPerfectFreeze;
    (g_Iw1ChainManager->*fcut)((void *)(intptr_t)b);

    int r = GetFreezeLevel(&g_Iw1Globals);
    fm(a);
    fp(b);

    Iw1HelpInf *hi = (Iw1HelpInf *)(intptr_t)a;
    r += Iw1HelpInf::Initialize(hi);

    Iw1Item *it = (Iw1Item *)(intptr_t)b;
    it->CollectPoint();

    Iw1ItemManager *im = GetItemMgr();
    im->~Iw1ItemManager();
    return r + g_sink;
}

int main() {
    return ItemWave1Probe(1, 2);
}
