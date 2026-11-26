#include "../src/CardInf.hpp"

extern "C" void __stdcall CardInfProbe()
{
    CardInf::EnableChains();
}
