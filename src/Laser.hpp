#pragma once

#include "LaserData.hpp"

// The four laser kinds. Only the overrides reconstructed so far are declared.
class LaserLineInf : public LaserDataInf
{
public:
    void Slot00(float distance, float *point);
};

class LaserCurveInf : public LaserDataInf
{
public:
    void Slot00(float distance, float *point);
};

class LaserInfiniteInf : public LaserDataInf
{
public:
    void Slot00(float distance, float *point);
    void Slot01();
};

class LaserBeamInf : public LaserDataInf
{
public:
    void Slot00(float distance, float *point);
};
