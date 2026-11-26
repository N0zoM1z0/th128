#include "../src/AsciiInf.hpp"

extern "C" unsigned int __stdcall AsciiInfProbe(
    AsciiInf *ascii, const ZunVec3 *position, const char *text)
{
    g_AsciiInf = ascii;
    const AsciiTextId direct = ascii->CreateText(position, text);
    return ascii->CreateTextF(position, "%u", direct.id).id;
}
