#include "CleanBatch1.hpp"
#include <math.h>

namespace th128 {

int g_Cb1TimerCtrl;
Cb1Gui* g_Cb1Gui;
Cb1SpellCard* g_Cb1Spell;
Cb1Player* g_Cb1Player;
Cb1Boss* g_Cb1Boss;

static Cb1Enemy g_Cb1Enemy;
static Cb1AnmVm g_Cb1Vm;
static Cb1InputState g_Cb1Input;

__declspec(noinline) void Cb1Vec3::SetPolarXY(float angle, float radius) {
    const double angleRadians = (double)angle;
    x = (float)(cos(angleRadians) * (double)radius);
    y = (float)(sin(angleRadians) * (double)radius);
}

typedef int (Cb1Enemy::*PhaseFn)();
typedef int (Cb1AnmVm::*RingFn)();
typedef void (Cb1InputState::*InputFn)();
typedef void (Cb1Vec3::*PolarFn)(float, float);
typedef int (__stdcall* SegFn)(Cb1Vec2, Cb1Vec2, Cb1Vec2, Cb1Vec2);

volatile PhaseFn g_Cb1PhasePtr = &Cb1Enemy::UpdatePhases;
volatile RingFn g_Cb1RingPtr = &Cb1AnmVm::UpdateRingMesh;
volatile InputFn g_Cb1InputPtr = &Cb1InputState::Update;
volatile PolarFn g_Cb1PolarPtr = &Cb1Vec3::SetPolarXY;
volatile SegFn g_Cb1SegPtr = &Cb1SegmentsIntersect;

} // namespace th128

extern "C" int __cdecl main(int argc, char** argv) {
    int r = 0;
    if (argc & 1) {
        r ^= (th128::g_Cb1Enemy.*th128::g_Cb1PhasePtr)();
    }
    if (argc & 2) {
        th128::Cb1Vec2 a = { (float)argc, 1.0f }, b = { 2.0f, 3.0f }, c = { 4.0f, 5.0f }, d = { 6.0f, 7.0f };
        r ^= th128::g_Cb1SegPtr(a, b, c, d);
    }
    if (argc & 4) {
        r ^= (th128::g_Cb1Vm.*th128::g_Cb1RingPtr)();
    }
    if (argc & 8) {
        (th128::g_Cb1Input.*th128::g_Cb1InputPtr)();
    }
    if (argc & 16) {
        (th128::g_Cb1Vm.pos1.*th128::g_Cb1PolarPtr)(1.0f, 2.0f);
    }
    return r;
}
