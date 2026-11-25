#include "AnmInterp.hpp"

#include "AngleMath.hpp"

template <typename T> void AnmInterp<T>::SetMode(unsigned int m) { mode = m; }
template <typename T> void AnmInterp<T>::SetStart(const T &v) { start = v; }
template <typename T> void AnmInterp<T>::SetGoal(const T &v) { goal = v; }
template <typename T> void AnmInterp<T>::SetVel1(const T &v) { vel1 = v; }
template <typename T> void AnmInterp<T>::SetVel2(const T &v) { vel2 = v; }
template <typename T> void AnmInterp<T>::SetDuration(int d) { duration = d; }
template <typename T> int AnmInterp<T>::GetDuration() const { return duration; }
template <typename T> void AnmInterp<T>::ClearDuration() { duration = 0; }
template <typename T> void AnmInterp<T>::ResetTimer()
{
    timer.Initialize();
    timer.currentF = 0.0f;
    timer.current = 0;
    timer.previous = -1;
}

template <typename T>
float AnmInterp<T>::EvalProgress()
{
    float progress = timer.currentF / (float)duration;
    unsigned int m = mode - 1u;
    return (m <= 0xFu) ? EaseTransition(m, progress) : progress;
}

template struct AnmInterp<AnmColor3>;
template struct AnmInterp<float>;
template struct AnmInterp<int>;
template struct AnmInterp<ZunVec2>;

namespace AnmInterpHelpers
{
void __stdcall UpdateAnmVmArray(AnmVm *vms, int count)
{
    while (count != 0)
    {
        if (vms->anmScriptIndex >= 0)
        {
            vms->Update();
        }
        ++vms;
        --count;
    }
}

int __stdcall AnmVmHasLoadedTexture(AnmVm *vm)
{
    AnmFileEntry *entry = vm->anmFileEntry;
    AnmManager *mgr = g_AnmManager;
    if (entry == 0)
    {
        return 0;
    }
    int slot = *(int *)entry;
    if (slot < 0)
    {
        return 0;
    }
    if (mgr->loadedAnms[slot] == 0)
    {
        return 0;
    }
    return mgr->loadedAnms[slot]->textures != 0;
}

ZunVec2 *__stdcall Vec2Sub(ZunVec2 *out, const ZunVec2 *a, const ZunVec2 *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    return out;
}

ZunVec2 *__stdcall Vec2Scale(ZunVec2 *out, const ZunVec2 *a, float s)
{
    out->x = a->x * s;
    out->y = a->y * s;
    return out;
}

ZunVec2 *__stdcall Vec2Add(ZunVec2 *out, const ZunVec2 *a, const ZunVec2 *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    return out;
}

void __stdcall Vec2Set(ZunVec2 *out, float x, float y)
{
    out->x = x;
    out->y = y;
}

float __stdcall ShortestAngleDifference3(float target, float current)
{
    if (target - current > 3.1415927410125732)
    {
        return (float)(target - (current + 6.2831854820251465));
    }
    if (current - target > 3.1415927410125732)
    {
        return (float)(target - (current - 6.2831854820251465));
    }
    return target - current;
}

float *__stdcall AngleScaleNorm(float *out, const float *a, float s)
{
    *out = NormalizeAngle(*a * s);
    return out;
}

float *__stdcall AngleAddNorm(float *out, const float *a, const float *b)
{
    *out = NormalizeAngle(*a + *b);
    return out;
}

AnmColor3 *__stdcall Color3Sub(AnmColor3 *out, const AnmColor3 *a, const AnmColor3 *b)
{
    int r = a->r - b->r;
    int g = a->g - b->g;
    int bl = a->b - b->b;
    out->r = r;
    out->g = g;
    out->b = bl;
    return out;
}

AnmColor3 *__stdcall Color3Scale(AnmColor3 *out, const AnmColor3 *a, float s)
{
    int r = (int)(a->r * s);
    int g = (int)(a->g * s);
    int b = (int)(a->b * s);
    out->r = r;
    out->g = g;
    out->b = b;
    return out;
}

AnmColor3 *__stdcall Color3Add(AnmColor3 *out, const AnmColor3 *a, const AnmColor3 *b)
{
    int r = b->r + a->r;
    int g = b->g + a->g;
    int bl = b->b + a->b;
    out->r = r;
    out->g = g;
    out->b = bl;
    return out;
}

void __stdcall Color3Set(AnmColor3 *out, int r, int g, int b)
{
    out->r = r;
    out->g = g;
    out->b = b;
}

void __stdcall Color3FromBytes(AnmColor3 *out, const unsigned char *rgb)
{
    out->b = rgb[2];
    out->g = rgb[1];
    out->r = rgb[0];
}

void __stdcall RgbBytesSet(AnmRgbBytes *out, unsigned char r, unsigned char g, unsigned char b)
{
    out->r = r;
    out->g = g;
    out->b = b;
}

unsigned int __stdcall BlendColorChannel(unsigned char a, unsigned char b)
{
    unsigned int v = ((unsigned int)a * (unsigned int)b) >> 7;
    if (v >= 256u)
    {
        return 255u;
    }
    return v;
}
}
