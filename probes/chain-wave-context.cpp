#include "../src/ChainWave.hpp"

namespace th128 {

unsigned int g_CwGameFlags = 0;
CRITICAL_SECTION g_CwChainCs;
unsigned char g_CwChainLockDepth = 0;
CwChain *g_CwChain = NULL;

static volatile int g_Sink = 0;

} // namespace th128

extern "C" int __stdcall ChainWaveProbe(int selector, int arg) {
    using namespace th128;
    CwChain *c = (CwChain *)arg;
    CwChainElem *e = (CwChainElem *)(arg + 4);
    if (selector == 1) {
        CwChain *p = new CwChain();
        return (int)p;
    }
    if (selector == 2) {
        int r1 = c->RunCalcChain();
        int r2 = c->RunCalcChain();
        return r1 + r2;
    }
    if (selector == 3) {
        return CwChain::RunDrawChain();
    }
    if (selector == 4) {
        c->CutList(e);
        return g_Sink;
    }
    if (selector == 5) {
        c->Cut(e);
        return g_Sink;
    }
    return 0;
}
