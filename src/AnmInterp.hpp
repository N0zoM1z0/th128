#ifndef TH128_ANM_INTERP_HPP
#define TH128_ANM_INTERP_HPP

#include "AnmManager.hpp"
#include "AnmResource.hpp"
#include "AnmVm.hpp"
#include "CoreMath.hpp"

struct AnmColor3
{
    int r;
    int g;
    int b;
};

struct AnmRgbBytes
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

template <typename T>
struct AnmInterp
{
    T start;
    T goal;
    T vel1;
    T vel2;
    ZunTimer timer;
    int duration;
    unsigned int mode;

    void SetMode(unsigned int m);
    void SetStart(const T &v);
    void SetGoal(const T &v);
    void SetVel1(const T &v);
    void SetVel2(const T &v);
    void SetDuration(int d);
    int GetDuration() const;
    void ClearDuration();
    void ResetTimer();
    float EvalProgress();
};

float __stdcall EaseTransition(unsigned int mode, float progress);

namespace AnmInterpHelpers
{
void __stdcall UpdateAnmVmArray(AnmVm *vms, int count);
int __stdcall AnmVmHasLoadedTexture(AnmVm *vm);
ZunVec2 *__stdcall Vec2Sub(ZunVec2 *out, const ZunVec2 *a, const ZunVec2 *b);
ZunVec2 *__stdcall Vec2Scale(ZunVec2 *out, const ZunVec2 *a, float s);
ZunVec2 *__stdcall Vec2Add(ZunVec2 *out, const ZunVec2 *a, const ZunVec2 *b);
void __stdcall Vec2Set(ZunVec2 *out, float x, float y);
float __stdcall ShortestAngleDifference3(float target, float current);
float *__stdcall AngleScaleNorm(float *out, const float *a, float s);
float *__stdcall AngleAddNorm(float *out, const float *a, const float *b);
AnmColor3 *__stdcall Color3Sub(AnmColor3 *out, const AnmColor3 *a, const AnmColor3 *b);
AnmColor3 *__stdcall Color3Scale(AnmColor3 *out, const AnmColor3 *a, float s);
AnmColor3 *__stdcall Color3Add(AnmColor3 *out, const AnmColor3 *a, const AnmColor3 *b);
void __stdcall Color3Set(AnmColor3 *out, int r, int g, int b);
void __stdcall Color3FromBytes(AnmColor3 *out, const unsigned char *rgb);
void __stdcall RgbBytesSet(AnmRgbBytes *out, unsigned char r, unsigned char g, unsigned char b);
unsigned int __stdcall BlendColorChannel(unsigned char a, unsigned char b);
}

#endif
