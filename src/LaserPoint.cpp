#include "Laser.hpp"

#include <math.h>

void ZunVec3::SetPolarXY(float angle, float radius)
{
    const double angleRadians = (double)angle;
    x = (float)(cos(angleRadians) * (double)radius);
    y = (float)(sin(angleRadians) * (double)radius);
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
