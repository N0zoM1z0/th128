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

    // Natural C++ polar transform. The target helper uses x87 FSINCOS, so
    // this implementation intentionally remains a codegen frontier rather
    // than using inline assembly to force an exact match.
    void SetPolarXY(float angle, float radius);
};
