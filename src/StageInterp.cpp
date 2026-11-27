#include "StageInterp.hpp"

#include <math.h>
#include <stdlib.h>

inline void StageTimerMini::EnsureInit()
{
    if ((flags & 1U) == 0)
    {
        currentF = 0.0f;
        current = 0;
        previous = -999999;
        gameSpeed = &g_StageDefaultTimeScale;
        flags |= 1U;
    }
}

inline void StageTimerMini::SetInt(int v)
{
    EnsureInit();
    current = v;
    previous = v - 1;
    currentF = (float)v;
}

inline void StageTimerMini::ResetToMinus1()
{
    EnsureInit();
    currentF = 0.0f;
    current = 0;
    previous = -1;
}

void StageBgStateMini::JumpToLabel(int labelId)
{
    StageBgStateMini *st = g_StageBgState;
    StageScriptCmd *cmd = st->scriptStart;
    while (cmd->time >= 0)
    {
        if (cmd->opcode == 0x10 && cmd->arg0 == labelId)
        {
            cmd = (StageScriptCmd *)((char *)cmd + cmd->size);
            st->scriptCur = cmd;
            int t = cmd->time;
            st->scriptTimer.EnsureInit();
            st->scriptTimer.current = t;
            st->scriptTimer.previous = t - 1;
            st->scriptTimer.currentF = (float)t;
            return;
        }
        cmd = (StageScriptCmd *)((char *)cmd + cmd->size);
    }
}

void StageBgStateMini::SetVecPair(const StageVec3 *a, const StageVec3 *b)
{
    flag2918 = 1;
    vec291C = *a;
    vec2928 = *b;
}

void StageBgStateMini::SetFlag2910()
{
    flag2910 = 1;
}

static inline void StopVmTree(StageAnmManagerMini *mgr, int id)
{
    StageAnmVmMini *vm = mgr->GetVmById(id);
    if (vm != NULL)
    {
        vm->flags4B0 |= 0x20000000U;
        if (vm->parentPtr == NULL)
        {
            for (StageAnmNodeMini *n = vm->childHead; n != NULL; n = n->next)
            {
                n->vm->flags4B0 |= 0x20000000U;
            }
        }
    }
}

void StageBgStateMini::StopBgVms()
{
    StageAnmManagerMini *mgr = g_StageAnmManager;
    flag2910 = 0;
    StopVmTree(mgr, vmId3834);
    StopVmTree(mgr, vmId3838);
}

void StageBgStateMini::StartFadeOut60()
{
    StageBgStateMini *st = g_StageBgState;
    st->fadeTimer.SetInt(60);
    st->flags37F8 |= 4U;
}

template <typename T>
float StageInterp<T>::EvalT()
{
    float t = timer.currentF / (float)duration;
    if ((unsigned int)(mode - 1) <= 15U)
    {
        t = StageEaseTransition(t, mode - 1);
    }
    return t;
}

template <typename T>
void StageInterp<T>::ResetTimer()
{
    timer.ResetToMinus1();
}

template <typename T>
T StageInterp<T>::Step()
{
    if (duration > 0)
    {
        timer.Increment();
        if (timer.current >= duration)
        {
            timer.SetInt(duration);
            duration = 0;
            if (mode != 7)
            {
                return end;
            }
            return start;
        }
    }

    if (mode == 7)
    {
        start = start + end;
        return start;
    }
    if (mode == 17)
    {
        start = start + vel1;
        vel1 = end + vel1;
        return start;
    }
    if (mode == 8)
    {
        float t = timer.currentF / (float)duration;
        float tm1 = t - 1.0f;
        float w0 = (2.0f * t + 1.0f) * tm1 * tm1;
        float w1 = (float)((3.0 - 2.0f * t) * t * t);
        float w2 = tm1 * tm1 * t;
        float w3 = tm1 * t * t;
        return start * w0 + end * w1 + vel0 * w2 + vel1 * w3;
    }

    float t = EvalT();
    return (end - start) * t + start;
}

// Explicit template instantiations for Vec3 and StageFog
template struct StageInterp<StageVec3>;
template struct StageInterp<StageFog>;

StageVec3 StageVec3::operator*(float s) const
{
    StageVec3 r;
    r.x = x * s;
    r.y = y * s;
    r.z = z * s;
    return r;
}

StageVec3 StageVec3::operator/(float s) const
{
    float inv = 1.0f / s;
    StageVec3 r;
    r.x = x * inv;
    r.y = y * inv;
    r.z = z * inv;
    return r;
}

float StageVec3::LengthSq() const
{
    return x * x + y * y + z * z;
}

void StageFog::PackRgba()
{
    cr = (unsigned char)(int)r;
    cg = (unsigned char)(int)g;
    cb = (unsigned char)(int)b;
    ca = (unsigned char)(int)a;
}

StageFog::StageFog(float p0, float p1, float pr, float pg, float pb, float pa)
{
    f0 = p0;
    f1 = p1;
    r = pr;
    g = pg;
    b = pb;
    a = pa;
    PackRgba();
}

StageFog StageFog::operator-(const StageFog &rhs) const
{
    StageFog res;
    res.f0 = f0 - rhs.f0;
    res.f1 = f1 - rhs.f1;
    res.r = r - rhs.r;
    res.g = g - rhs.g;
    res.b = b - rhs.b;
    res.a = a - rhs.a;
    res.PackRgba();
    return res;
}

StageFog StageFog::operator*(float s) const
{
    StageFog res;
    res.f0 = f0 * s;
    res.f1 = f1 * s;
    res.r = r * s;
    res.g = g * s;
    res.b = b * s;
    res.a = a * s;
    res.PackRgba();
    return res;
}

StageFog StageFog::operator+(const StageFog &rhs) const
{
    StageFog res;
    res.f0 = f0 + rhs.f0;
    res.f1 = f1 + rhs.f1;
    res.r = r + rhs.r;
    res.g = g + rhs.g;
    res.b = b + rhs.b;
    res.a = a + rhs.a;
    res.PackRgba();
    return res;
}

float __stdcall StageCosf(float a)
{
    return (float)cos((double)a);
}

float * __stdcall StageNormalizeAngleInto(float *out, float a)
{
    *out = StageNormalizeAngle(a);
    return out;
}

void StageAnmVmMini::ClearFlag0()
{
    flags4B0 &= ~1U;
}

bool StageAnmVmMini::HasField424()
{
    return field424 != 0;
}

void StageSetAnmDirty()
{
    g_StageAnmManager->dirtyFlag = 1;
}

int __stdcall StageUpdateVm(StageAnmVmMini *vm)
{
    return vm->UpdateFrame();
}

void *StageMalloc(size_t n)
{
    return malloc(n);
}
