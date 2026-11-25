#ifndef TH128_CORE_MATH_HPP
#define TH128_CORE_MATH_HPP

#include "Chain.hpp"
#include "ZunMath.hpp"

float __stdcall NormalizeAngle(float angle);

struct ZunVec2
{
    float x;
    float y;
};

struct TimeScaleGlobal
{
    float scale;
    int refCount;
};

struct Matrix4x4
{
    float m[16];
};

extern TimeScaleGlobal g_DefaultTimeScale;
extern Matrix4x4 g_GlobalMatrix4x4;

int __stdcall ComputeLineSlopeIntercept(float x1, float y1, float x2, float y2, float *outSlope, float *outIntercept);
float __stdcall Vec2LengthSq(const ZunVec2 *v);
float __stdcall ShortestAngleDifference2(float target, float current);
float *__stdcall NormalizeAngleDelta(float *out, const float *current, float target);
void __stdcall QuantizeVec2(ZunVec2 *v);
float __stdcall MultiplyFloatPtr(const float *p, float v);
void __stdcall DecTimeScaleRef(TimeScaleGlobal *ts);
void __cdecl Stub46C240();
TimeScaleGlobal *__cdecl InitDefaultTimeScale();
void __cdecl Stub46C270();
void __cdecl DebugLog46C320(const char *fmt, ...);
ChainNode *__stdcall FindChainNodeByOwner(ChainNode *head, ChainElem *owner);
void __stdcall SetChainElemCallback46C3A0(ChainElem *elem, ChainCallback fn);
Matrix4x4 *__stdcall SetGlobalMatrix4x4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33);

#endif
