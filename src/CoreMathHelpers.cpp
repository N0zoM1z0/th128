#include "CoreMathHelpers.hpp"

#include <math.h>
#include <stddef.h>
#include <string.h>

unsigned int CoreRng::NextU32()
{
    if ((g_CoreRuntimeFlags & 0x8000) != 0)
    {
        EnterCriticalSection(&g_CoreRngLock);
        ++g_CoreRngLockDepth;
    }

    unsigned short s0 = (unsigned short)((seed ^ 0x9630) - 0x6553);
    callCount += 2;
    unsigned short s1 = (unsigned short)((s0 >> 14) + (s0 << 2));
    unsigned short s2 = (unsigned short)((s1 ^ 0x9630) - 0x6553);
    seed = (unsigned short)((s2 >> 14) + (s2 << 2));

    if ((g_CoreRuntimeFlags & 0x8000) != 0)
    {
        LeaveCriticalSection(&g_CoreRngLock);
        --g_CoreRngLockDepth;
    }

    return ((unsigned int)s0 << 16) | (unsigned int)s2;
}

unsigned int CoreRng::NextU32Skip2()
{
    unsigned short parts[4];
    parts[0] = GetU16();
    parts[1] = GetU16();
    parts[2] = GetU16();
    parts[3] = GetU16();
    return *(unsigned int *)parts;
}

void CoreFloat2::RotateFrom(const CoreFloat2 *in, float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    float nx = in->x * c - in->y * s;
    y = in->x * s + in->y * c;
    x = nx;
}

void CoreTimer::ClearFlag0()
{
    flags &= ~1U;
}

void CoreTimer::ResetScale()
{
    if (scale != NULL)
    {
        --scale->refCount;
    }
    scale = &g_CoreDefaultTimeScale;
}

int CoreTimer::Advance()
{
    int cur = current;
    previous = cur;
    if (scale->scale > (double)0.99f && scale->scale < 1.01f)
    {
        ++cur;
        current = cur;
        subFrame = (float)(subFrame + 1.0);
        return cur;
    }
    subFrame = scale->scale + subFrame;
    current = (int)subFrame;
    return current;
}

float CoreTimer::GetScale()
{
    return scale->scale;
}

const char *CoreReadLineSjis(char *dst, const char *src, int maxLen)
{
    memset(dst, 0, maxLen);
    char *out = dst;
    while (*src != '\n' && *src != '\r' && maxLen != 0)
    {
        *out++ = *src;
        unsigned char c = (unsigned char)*src;
        if ((c >= 0x81 && c <= 0x9F) || (c >= 0xE0 && c <= 0xFC))
        {
            ++src;
            --maxLen;
            *out++ = *src;
        }
        ++src;
        --maxLen;
    }
    while (*src == '\n' || *src == '\r')
    {
        ++src;
    }
    return src;
}

unsigned char CoreChecksumBytes(unsigned int len, const unsigned char *buf)
{
    unsigned char sum = 0;
    while (len != 0)
    {
        sum += *buf;
        ++buf;
        --len;
    }
    return sum;
}

unsigned short CoreChecksumWords(const unsigned short *buf, int byteLen)
{
    byteLen -= byteLen & 1;
    unsigned short sum = 0;
    while (byteLen > 0)
    {
        sum += *buf;
        ++buf;
        byteLen -= 2;
    }
    return sum;
}

unsigned int CoreChecksumDwords(const unsigned int *buf, int byteLen)
{
    byteLen -= byteLen & 3;
    unsigned int sum = 0;
    while (byteLen > 0)
    {
        sum += *buf;
        ++buf;
        byteLen -= 4;
    }
    return sum;
}

void CoreMotionState::UpdateVelocity()
{
    if ((flags & 1) == 0)
    {
        float spd = speed * g_CoreDefaultTimeScale.scale;
        ((CoreFloat2 *)&vel)->FromPolar(angle, spd);
        vel.z = 0.0f;
    }
    else
    {
        radius += radiusVel * g_CoreDefaultTimeScale.scale;
        float a = angle + speed * g_CoreDefaultTimeScale.scale;
        angle = CoreNormalizeAngle(CoreNormalizeAngle(a));
    }
}

void CoreMotionState::StepPosition()
{
    if ((flags & 1) == 0)
    {
        pos.x += vel.x;
        pos.y += vel.y;
        pos.z += vel.z;
        pos.x = (float)((float)floor((double)(float)(pos.x * 100.0)) / 100.0);
        pos.y = (float)((float)floor((double)(float)(pos.y * 100.0)) / 100.0);
        return;
    }

    CoreFloat2 offset;
    if ((flags & 2) == 0)
    {
        offset.FromPolar(angle, radius);
    }
    else
    {
        CoreFloat2 local;
        local.FromPolar(CoreNormalizeAngle(CoreNormalizeAngle(CoreShortestAngleDiff(angle, ellipseAngle, radius))), radius);
        local.y *= ellipseScaleY;
        float ea = ellipseAngle;
        float s = sinf(ea);
        float c = cosf(ea);
        offset.y = local.x * s + local.y * c;
        offset.x = local.x * c - local.y * s;
    }

    CoreFloat3 next;
    next.x = vel.x + offset.x;
    next.y = vel.y + offset.y;
    next.z = (float)(vel.z + 0.0);
    pos = next;
    QuantizePosition();
}

void CoreMotionState::ApplyPosition()
{
    if ((flags & 1) == 0)
    {
        pos.x = (float)((float)floor((double)(float)(pos.x * 100.0)) / 100.0);
        pos.y = (float)((float)floor((double)(float)(pos.y * 100.0)) / 100.0);
        return;
    }

    CoreFloat2 offset;
    if ((flags & 2) == 0)
    {
        offset.FromPolar(angle, radius);
    }
    else
    {
        CoreFloat2 local;
        local.FromPolar(CoreNormalizeAngle(CoreNormalizeAngle(CoreShortestAngleDiff(angle, ellipseAngle, radius))), radius);
        local.y *= ellipseScaleY;
        float ea = ellipseAngle;
        float s = sinf(ea);
        float c = cosf(ea);
        offset.y = local.x * s + local.y * c;
        offset.x = local.x * c - local.y * s;
    }

    CoreFloat3 next;
    next.x = vel.x + offset.x;
    next.y = vel.y + offset.y;
    next.z = (float)(vel.z + 0.0);
    pos = next;
    QuantizePosition();
}
