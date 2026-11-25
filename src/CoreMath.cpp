#include "CoreMath.hpp"

#include <math.h>

#include "GameErrorContext.hpp"

int __stdcall ComputeLineSlopeIntercept(float x1, float y1, float x2, float y2, float *outSlope, float *outIntercept)
{
    float dx = x2 - x1;
    if (fabsf(dx) < 0.01f)
    {
        *outSlope = 0.0f;
        *outIntercept = x1;
        return 1;
    }
    float slope = (y2 - y1) / dx;
    *outSlope = slope;
    *outIntercept = y1 - slope * x1;
    return 0;
}

float __stdcall Vec2LengthSq(const ZunVec2 *v)
{
    return v->x * v->x + v->y * v->y;
}

float __stdcall ShortestAngleDifference2(float target, float current)
{
    if (target - current > 3.141592653589793)
    {
        return (float)(target - (current + 6.283185307179586));
    }
    if (current - target > 3.141592653589793)
    {
        return (float)(target - (current - 6.283185307179586));
    }
    return target - current;
}

float *__stdcall NormalizeAngleDelta(float *out, const float *current, float target)
{
    *out = NormalizeAngle(ShortestAngleDifference2(*current, target));
    return out;
}

void __stdcall QuantizeVec2(ZunVec2 *v)
{
    v->x = (float)(floor((float)(v->x * 100.0)) / 100.0);
    v->y = (float)(floor((float)(v->y * 100.0)) / 100.0);
}

float __stdcall MultiplyFloatPtr(const float *p, float v)
{
    return *p * v;
}

void __stdcall DecTimeScaleRef(TimeScaleGlobal *ts)
{
    --ts->refCount;
}

void __cdecl Stub46C240()
{
}

TimeScaleGlobal *__cdecl InitDefaultTimeScale()
{
    g_DefaultTimeScale.refCount = 0;
    g_DefaultTimeScale.scale = 1.0f;
    return &g_DefaultTimeScale;
}

void __cdecl Stub46C270()
{
}

GameErrorContext *__cdecl InitGameErrorContext()
{
    g_GameErrorContext.buffer[0] = '\0';
    g_GameErrorContext.fatal = 0;
    g_GameErrorContext.cursor = g_GameErrorContext.buffer;
    return &g_GameErrorContext;
}

void __cdecl DebugLog46C320(const char *, ...)
{
}

ChainNode *__stdcall FindChainNodeByOwner(ChainNode *head, ChainElem *owner)
{
    while (head != 0)
    {
        if (head->owner == owner)
        {
            return head;
        }
        head = head->next;
    }
    return 0;
}

void __stdcall SetChainElemCallback46C3A0(ChainElem *elem, ChainCallback fn)
{
    elem->callback = fn;
    elem->onRegistration = 0;
    elem->onDeletion = 0;
}

Matrix4x4 *__stdcall SetGlobalMatrix4x4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    g_GlobalMatrix4x4.m[0] = m00;
    g_GlobalMatrix4x4.m[1] = m01;
    g_GlobalMatrix4x4.m[2] = m02;
    g_GlobalMatrix4x4.m[3] = m03;
    g_GlobalMatrix4x4.m[4] = m10;
    g_GlobalMatrix4x4.m[5] = m11;
    g_GlobalMatrix4x4.m[6] = m12;
    g_GlobalMatrix4x4.m[7] = m13;
    g_GlobalMatrix4x4.m[8] = m20;
    g_GlobalMatrix4x4.m[9] = m21;
    g_GlobalMatrix4x4.m[10] = m22;
    g_GlobalMatrix4x4.m[11] = m23;
    g_GlobalMatrix4x4.m[12] = m30;
    g_GlobalMatrix4x4.m[13] = m31;
    g_GlobalMatrix4x4.m[14] = m32;
    g_GlobalMatrix4x4.m[15] = m33;
    return &g_GlobalMatrix4x4;
}
