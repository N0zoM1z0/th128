// Non-runnable VC10 /GL context for AnmInterp, AnmVmTransitions, and vector helpers.
#include "../src/AnmInterp.hpp"
#include <windows.h>

AnmManager *g_AnmManager = 0;
float g_TimeScale = 1.0f;

float __stdcall EaseTransition(unsigned int modeIndex, float t)
{
    return (modeIndex & 1u) ? (t * t) : (t + (float)modeIndex);
}

float __stdcall NormalizeAngle(float angle)
{
    Sleep(0);
    return angle * 0.5f;
}

int AnmVm::Update()
{
    Sleep((DWORD)anmScriptIndex);
    return anmScriptIndex;
}

int (AnmVm::*volatile g_ProbeVmUpdate)() = &AnmVm::Update;
float (__stdcall *volatile g_ProbeNormAngle)(float) = &NormalizeAngle;

float (__stdcall *g_ProbeShortest3)(float, float) = AnmInterpHelpers::ShortestAngleDifference3;

extern "C" float __stdcall AnmInterpProbe(
    AnmVm *vm1, AnmVm *vm2, int count,
    AnmInterp<AnmColor3> *ic1, AnmInterp<AnmColor3> *ic2,
    AnmInterp<float> *if1, AnmInterp<float> *if2,
    AnmInterp<int> *ii1, AnmInterp<int> *ii2,
    AnmInterp<ZunVec2> *iv1, AnmInterp<ZunVec2> *iv2,
    ZunVec2 *v1, ZunVec2 *v2, ZunVec2 *vo,
    AnmColor3 *c1, AnmColor3 *c2, AnmColor3 *co,
    AnmRgbBytes *rgb1, AnmRgbBytes *rgb2,
    const unsigned char *b1, const unsigned char *b2,
    float *f1, float *f2, float *fo,
    int d1, int d2, unsigned char m1, unsigned char m2)
{
    AnmInterpHelpers::UpdateAnmVmArray(vm1, count);
    AnmInterpHelpers::UpdateAnmVmArray(vm2, d1);
    float r = 0.0f;
    r += (float)AnmInterpHelpers::AnmVmHasLoadedTexture(vm1);
    r += (float)AnmInterpHelpers::AnmVmHasLoadedTexture(vm2);

    r += ic1->EvalProgress() + ic2->EvalProgress();
    r += if1->EvalProgress() + if2->EvalProgress();
    r += ii1->EvalProgress() + ii2->EvalProgress();
    r += iv1->EvalProgress() + iv2->EvalProgress();

    if1->SetMode(d1); if2->SetMode(d2);
    if1->SetStart(*f1); if2->SetStart(*f2);
    if1->SetGoal(*f1); if2->SetGoal(*f2);
    if1->SetVel1(*f1); if2->SetVel1(*f2);
    if1->SetVel2(*f1); if2->SetVel2(*f2);
    if1->SetDuration(d1); if2->SetDuration(d2);
    r += (float)(if1->GetDuration() + if2->GetDuration());
    if1->ClearDuration(); if2->ClearDuration();

    iv1->SetDuration(d1); iv2->SetDuration(d2);
    iv1->SetMode(d1); iv2->SetMode(d2);
    iv1->SetStart(*v1); iv2->SetStart(*v2);
    iv1->SetGoal(*v1); iv2->SetGoal(*v2);
    iv1->ResetTimer(); iv2->ResetTimer();
    r += (float)(iv1->GetDuration() + iv2->GetDuration());

    ic1->SetDuration(d1); ic2->SetDuration(d2);
    ic1->SetMode(d1); ic2->SetMode(d2);
    ic1->SetStart(*c1); ic2->SetStart(*c2);
    ic1->SetGoal(*c1); ic2->SetGoal(*c2);
    ic1->SetVel1(*c1); ic2->SetVel1(*c2);
    ic1->SetVel2(*c1); ic2->SetVel2(*c2);
    ic1->ResetTimer(); ic2->ResetTimer();
    r += (float)(ic1->GetDuration() + ic2->GetDuration());

    ii1->SetDuration(d1); ii2->SetDuration(d2);
    ii1->SetMode(d1); ii2->SetMode(d2);
    ii1->SetStart(d1); ii2->SetStart(d2);
    ii1->SetGoal(d1); ii2->SetGoal(d2);
    ii1->SetVel1(d1); ii2->SetVel1(d2);
    ii1->SetVel2(d1); ii2->SetVel2(d2);
    ii1->ResetTimer(); ii2->ResetTimer();
    r += (float)(ii1->GetDuration() + ii2->GetDuration());

    AnmInterpHelpers::Vec2Sub(vo, v1, v2); AnmInterpHelpers::Vec2Sub(v1, v2, vo);
    AnmInterpHelpers::Vec2Scale(vo, v1, *f1); AnmInterpHelpers::Vec2Scale(v1, v2, *f2);
    AnmInterpHelpers::Vec2Add(vo, v1, v2); AnmInterpHelpers::Vec2Add(v1, v2, vo);
    AnmInterpHelpers::Vec2Set(v1, *f1, *f2); AnmInterpHelpers::Vec2Set(v2, *f2, *f1);
    r += AnmInterpHelpers::ShortestAngleDifference3(*f1, *f2);
    AnmInterpHelpers::AngleScaleNorm(fo, f1, *f2); AnmInterpHelpers::AngleScaleNorm(f1, f2, *fo);
    AnmInterpHelpers::AngleAddNorm(fo, f1, f2); AnmInterpHelpers::AngleAddNorm(f1, f2, fo);
    AnmInterpHelpers::Color3Sub(co, c1, c2); AnmInterpHelpers::Color3Sub(c1, c2, co);
    AnmInterpHelpers::Color3Scale(co, c1, *f1); AnmInterpHelpers::Color3Scale(c1, c2, *f2);
    AnmInterpHelpers::Color3Add(co, c1, c2); AnmInterpHelpers::Color3Add(c1, c2, co);
    AnmInterpHelpers::Color3Set(c1, d1, d2, count); AnmInterpHelpers::Color3Set(c2, count, d1, d2);
    AnmInterpHelpers::Color3FromBytes(c1, b1); AnmInterpHelpers::Color3FromBytes(c2, b2);
    AnmInterpHelpers::RgbBytesSet(rgb1, m1, m2, b1[0]); AnmInterpHelpers::RgbBytesSet(rgb2, m2, m1, b2[0]);
    r += (float)(AnmInterpHelpers::BlendColorChannel(m1, m2) + AnmInterpHelpers::BlendColorChannel(m2, m1));

    vm1->SetSecondaryScaleTransition(f1, f2, d1, m1);
    vm2->SetSecondaryScaleTransition(f2, f1, d2, m2);
    vm1->SetScaleTransition(f1, f2, d1, m1);
    vm2->SetScaleTransition(f2, f1, d2, m2);
    vm1->SetSecondaryEffectState((unsigned int)d1, m1, b1[0], b2[0]);
    vm2->SetSecondaryEffectState((unsigned int)d2, m2, b2[0], b1[0]);
    vm1->SetSecondaryColorTransition(b1, b2, d1, m1);
    vm2->SetSecondaryColorTransition(b2, b1, d2, m2);
    vm1->SetColorTransition(b1, b2, d1, m1);
    vm2->SetColorTransition(b2, b1, d2, m2);
    vm1->SetEffectState((unsigned int)d1, (unsigned int)m1, b1[0], b2[0]);
    vm2->SetEffectState((unsigned int)d2, (unsigned int)m2, b2[0], b1[0]);
    return r;
}
