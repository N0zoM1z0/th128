#include "../src/CardInf.hpp"

extern "C" void __stdcall CardInfProbe()
{
    CardInf *card = CardInf::Create();
    if (card != 0)
    {
        CardInf::EnableChains();
        CardInf::Destroy();
    }
}
