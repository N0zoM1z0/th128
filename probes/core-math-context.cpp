// Non-runnable VC10 /GL context for CoreMath & small global helpers.
#include "../src/CoreMath.hpp"
#include "../src/GameErrorContext.hpp"

TimeScaleGlobal g_DefaultTimeScale;
Matrix4x4 g_GlobalMatrix4x4;
GameErrorContext g_GameErrorContext;

GameErrorContext *__cdecl InitGameErrorContext();

float (__stdcall *g_ProbeShortest2)(float, float) = ShortestAngleDifference2;
Matrix4x4 *(__stdcall *g_ProbeSetMat)(
    float, float, float, float, float, float, float, float,
    float, float, float, float, float, float, float, float) = SetGlobalMatrix4x4;
TimeScaleGlobal *(__cdecl *g_ProbeInitTs)() = InitDefaultTimeScale;
GameErrorContext *(__cdecl *g_ProbeInitErr)() = InitGameErrorContext;
void (__cdecl *g_ProbeStub1)() = Stub46C240;
void (__cdecl *g_ProbeStub2)() = Stub46C270;

extern "C" float __stdcall CoreMathProbe(
    ZunVec2 *v1, ZunVec2 *v2, float *o1, float *o2,
    const float *c1, const float *c2, float a1, float a2,
    TimeScaleGlobal *ts1, TimeScaleGlobal *ts2,
    ChainNode *n1, ChainNode *n2, ChainElem *e1, ChainElem *e2,
    ChainCallback cb1, ChainCallback cb2, Matrix4x4 m)
{
    ComputeLineSlopeIntercept(a1, a2, *c1, *c2, o1, o2);
    ComputeLineSlopeIntercept(a2, a1, *c2, *c1, o2, o1);
    float r = Vec2LengthSq(v1) + Vec2LengthSq(v2);
    r += ShortestAngleDifference2(a1, a2) + ShortestAngleDifference2(a2, a1);
    NormalizeAngleDelta(o1, c1, a1);
    NormalizeAngleDelta(o2, c2, a2);
    QuantizeVec2(v1);
    QuantizeVec2(v2);
    r += MultiplyFloatPtr(c1, a1) + MultiplyFloatPtr(c2, a2);
    DecTimeScaleRef(ts1);
    DecTimeScaleRef(ts2);
    Stub46C240();
    InitDefaultTimeScale();
    Stub46C270();
    InitGameErrorContext();
    DebugLog46C320("probe %f", r);
    FindChainNodeByOwner(n1, e1);
    FindChainNodeByOwner(n2, e2);
    SetChainElemCallback46C3A0(e1, cb1);
    SetChainElemCallback46C3A0(e2, cb2);
    SetGlobalMatrix4x4(
        m.m[0], m.m[1], m.m[2], m.m[3],
        m.m[4], m.m[5], m.m[6], m.m[7],
        m.m[8], m.m[9], m.m[10], m.m[11],
        m.m[12], m.m[13], m.m[14], m.m[15]);
    return r;
}
