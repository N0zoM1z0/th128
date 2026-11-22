#pragma once

struct ZunVec3
{
    float x;
    float y;
    float z;

    ZunVec3 &operator+=(const ZunVec3 &other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // x = cos(angle) * radius, y = sin(angle) * radius, z unchanged. The
    // original computes this with FSINCOS, which VC10 cannot emit from C, so
    // the body is inline assembly as in the original (out-of-line copy at
    // 0x00432E20 shared by the laser kinds).
    void SetPolarXY(float angle, float radius);
};
