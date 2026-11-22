// Non-runnable compiler context: virtual dispatch keeps the overrides'
// thiscall convention, as the vtables do in the original.
#include "../src/Laser.hpp"

extern "C" void __stdcall LaserProbe(LaserDataInf *laser, float distance, float *point)
{
    laser->Slot00(distance, point);
    LaserLineInf line;
    LaserCurveInf curve;
    LaserInfiniteInf infinite;
    LaserBeamInf beam;
    line.Slot00(distance, point);
    curve.Slot00(distance, point);
    infinite.Slot00(distance, point);
    beam.Slot00(distance, point);
    infinite.Slot01();
}
