// Non-runnable linked context for the two real Chain callback contracts.
#include "../src/HelpInf.hpp"

ChainCallback g_HelpTickCallback = HelpInf::OnTick;
ChainCallback g_HelpDrawCallback = HelpInf::OnDraw;

extern "C" int __stdcall HelpProbe(HelpInf *help, int scriptIndex, const ZunVec3 *position)
{
    help->CreateVm(scriptIndex, position);
    return g_HelpTickCallback(help) + g_HelpDrawCallback(help);
}
