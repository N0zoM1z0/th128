#pragma once

#include <windows.h>

extern unsigned int g_CoreRuntimeFlags;     // 0x004D33F8
extern CRITICAL_SECTION g_CoreRngLock;      // 0x004D3768
extern unsigned char g_CoreRngLockDepth;    // 0x004D37A2

struct CoreTimeScale
{
    float scale;                            // +0x00
    int refCount;                           // +0x04
};

extern CoreTimeScale g_CoreDefaultTimeScale; // 0x004B6F58

struct CoreRng
{
    unsigned short seed;                    // +0x00
    unsigned short pad02;                   // +0x02
    unsigned int callCount;                 // +0x04

    unsigned short GetU16();
    unsigned int NextU32();
    unsigned int NextU32Skip2();
};

struct CoreFloat2
{
    float x;
    float y;

    void FromPolar(float angle, float radius);
    void RotateFrom(const CoreFloat2 *in, float angle);
};

struct CoreFloat3
{
    float x;
    float y;
    float z;
};

struct CoreTimer
{
    int previous;                           // +0x00
    int current;                            // +0x04
    float subFrame;                         // +0x08
    CoreTimeScale *scale;                   // +0x0C
    unsigned int flags;                     // +0x10

    void ClearFlag0();
    void ResetScale();
    int Advance();
    float GetScale();
};

struct CoreMotionState
{
    CoreFloat3 pos;                         // +0x00
    CoreFloat3 vel;                         // +0x0C
    float speed;                            // +0x18
    float angle;                            // +0x1C
    float radius;                           // +0x20
    float radiusVel;                        // +0x24
    float ellipseAngle;                     // +0x28
    float ellipseScaleY;                    // +0x2C
    unsigned int flags;                     // +0x30

    void UpdateVelocity();
    void StepPosition();
    void ApplyPosition();
    void QuantizePosition();
};

const char *CoreReadLineSjis(char *dst, const char *src, int maxLen);
unsigned char CoreChecksumBytes(unsigned int len, const unsigned char *buf);
unsigned short CoreChecksumWords(const unsigned short *buf, int byteLen);
unsigned int CoreChecksumDwords(const unsigned int *buf, int byteLen);

float __stdcall CoreNormalizeAngle(float angle);
float __stdcall CoreShortestAngleDiff(float a, float b, float c);
