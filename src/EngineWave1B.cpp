#include "EngineWave1B.hpp"

#include <math.h>

namespace th128 {

void __stdcall Ew1bRotateVec2(Ew1bVec2 *out, const Ew1bVec2 *in, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);
    float rx = in->x * c - in->y * s;
    out->y = in->x * s + in->y * c;
    out->x = rx;
}

int Ew1bTimer::Tick() {
    const float *spd = gameSpeed;
    int c = current;
    previous = c;
    if (*spd > (double)0.99f && *spd < 1.01f) {
        c++;
        current = c;
        currentF = (float)(currentF + 1.0);
        return c;
    }
    currentF += *spd;
    current = (int)currentF;
    return current;
}

int __stdcall Ew1bPointInRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py) {
    Ew1bVec3 d;
    d.x = px - cx;
    d.y = py - cy;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float rx = c * d.x - s * d.y;
    if (fabsf(rx) <= w * 0.5) {
        float ry = s * d.x + c * d.y;
        if (fabsf(ry) <= h * 0.5) {
            return 1;
        }
    }
    return 0;
}

int __stdcall Ew1bCircleIntersectsRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r) {
    Ew1bVec3 d;
    d.x = px - cx;
    d.y = py - cy;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float rx = c * d.x - s * d.y;
    float ry = s * d.x + c * d.y;
    float hw = (float)(w * 0.5);
    float hh = (float)(h * 0.5);

    if (fabsf(rx) <= hw + r && fabsf(ry) <= hh) {
        return 1;
    }
    if (fabsf(rx) <= hw && fabsf(ry) <= hh + r) {
        return 1;
    }
    float r2 = r * r;
    float x1 = rx - hw;
    float y1 = ry - hh;
    if (x1 * x1 + y1 * y1 < r2) {
        return 1;
    }
    float x2 = rx + hw;
    if (x2 * x2 + y1 * y1 < r2) {
        return 1;
    }
    float y2 = ry + hh;
    if (x1 * x1 + y2 * y2 < r2) {
        return 1;
    }
    if (x2 * x2 + y2 * y2 < r2) {
        return 1;
    }
    return 0;
}

int __stdcall Ew1bCircleInsideRotatedRect(float cx, float cy, float w, float h, float angle, float px, float py, float r) {
    Ew1bVec3 d;
    d.x = px - cx;
    d.y = py - cy;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float rx = c * d.x - s * d.y;
    if (fabsf(rx) <= w * 0.5 - r) {
        float ry = s * d.x + c * d.y;
        if (fabsf(ry) <= h * 0.5 - r) {
            return 1;
        }
    }
    return 0;
}

int __stdcall Ew1bRayCircleIntersect(const Ew1bVec2 *p2, const Ew1bVec2 *p1, float *outT1, float *outT2, float angle, float radius) {
    Ew1bVec3 d;
    d.x = p1->x - p2->x;
    d.y = p1->y - p2->y;
    float negAngle = -angle;
    float s = sinf(negAngle);
    float c = cosf(negAngle);
    float rx = c * d.x - s * d.y;
    float ry = s * d.x + c * d.y;
    if (fabsf(ry) < radius) {
        if (rx >= -radius) {
            if (rx < 0.0f) {
                if (rx * rx + ry * ry > radius * radius) {
                    return 0;
                }
            }
            float t = ry / radius;
            float halfChord = sqrtf(1.0f - t * t) * radius;
            *outT1 = rx - halfChord;
            *outT2 = rx + halfChord;
            return 1;
        }
    }
    return 0;
}

float *__stdcall Ew1bAdvanceAngle(float *out, const float *cur, float target) {
    *out = Ew1bNormalizeAngle(Ew1bShortestAngleDiff(*cur, target));
    return out;
}

Ew1bErrorContext *Ew1bInitGameErrorContext() {
    g_Ew1bErrorContext.buffer[0] = '\0';
    g_Ew1bErrorContext.fatal = 0;
    g_Ew1bErrorContext.cursor = g_Ew1bErrorContext.buffer;
    return &g_Ew1bErrorContext;
}

} // namespace th128
