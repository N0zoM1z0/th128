#include "Laser.hpp"

void ZunVec3::SetPolarXY(float angle, float radius)
{
    __asm
    {
        mov eax, this
        fld angle
        fsincos
        fmul radius
        fstp dword ptr [eax]
        fmul radius
        fstp dword ptr [eax + 4]
    }
}

// Point `distance` along the laser's direction from its origin.
void LaserLineInf::Slot00(float distance, float *point)
{
    ZunVec3 *result = (ZunVec3 *)point;
    result->SetPolarXY(angle, distance);
    *result += position;
}

// Point `distance` along the laser's direction from its origin.
void LaserCurveInf::Slot00(float distance, float *point)
{
    ZunVec3 *result = (ZunVec3 *)point;
    result->SetPolarXY(angle, distance);
    *result += position;
}

// Point `distance` along the laser's direction from its origin.
void LaserInfiniteInf::Slot00(float distance, float *point)
{
    ZunVec3 *result = (ZunVec3 *)point;
    result->SetPolarXY(angle, distance);
    *result += position;
}

// Point `distance` along the laser's direction from its origin.
void LaserBeamInf::Slot00(float distance, float *point)
{
    ZunVec3 *result = (ZunVec3 *)point;
    result->SetPolarXY(angle, distance);
    *result += position;
}
