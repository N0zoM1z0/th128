#pragma once

#include <windows.h>

namespace th128 {

struct Ew1bVec2 {
    float x;
    float y;
};

struct Ew1bVec3 {
    float x;
    float y;
    float z;
};

struct Ew1bTimer {
    int previous;
    int current;
    float currentF;
    const float *gameSpeed;

    int Tick();
};

struct Ew1bErrorContext {
    char buffer[0x2000];
    char *cursor;
    unsigned char fatal;
};

extern Ew1bErrorContext g_Ew1bErrorContext;

float __stdcall Ew1bShortestAngleDiff(float a, float b);
float __stdcall Ew1bNormalizeAngle(float a);

void __stdcall Ew1bRotateVec2(Ew1bVec2 *out, const Ew1bVec2 *in, float angle);
int __stdcall Ew1bPointInRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py);
int __stdcall Ew1bCircleIntersectsRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r);
int __stdcall Ew1bCircleInsideRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r);
int __stdcall Ew1bRayCircleIntersect(const Ew1bVec2 *p2, const Ew1bVec2 *p1, float *outT1, float *outT2, float angle, float radius);
float *__stdcall Ew1bAdvanceAngle(float *out, const float *cur, float target);
Ew1bErrorContext *Ew1bInitGameErrorContext();

} // namespace th128
