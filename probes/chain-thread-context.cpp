// Non-runnable VC10 /GL context linking Chain and ThreadInf with realistic
// callers and escaped globals matching TH128's whole-program call graph.
#include "../src/Chain.hpp"
#include "../src/Supervisor.hpp"
#include "../src/ThreadInf.hpp"

Supervisor g_Supervisor;
Chain *g_Chain = 0;
int g_ThreadFlag4B4D8C = 0;

unsigned int *g_ProbeSupervisorFlagsAddr = &g_Supervisor.flags;
CRITICAL_SECTION *g_ProbeCsAddr = g_Supervisor.criticalSections;
unsigned char *g_ProbeLockDepthAddr = g_Supervisor.lockDepths;

extern "C" void __stdcall ChainThreadProbe(
    Chain *c1, Chain *c2, ChainElem *e1, ChainElem *e2,
    ChainCallback cb1, ChainCallback cb2, void *arg1, void *arg2,
    ThreadInf *t1, ThreadInf *t2, ThreadStartRoutine fn1, ThreadStartRoutine fn2,
    int p1, int p2, unsigned int *flagPtr)
{
    ChainDebugLog("probe %d", p1);

    ChainElem local1;
    ChainElem local2(cb1);
    Chain localChain;

    c1->RegisterCalc(e1, p1);
    c1->RegisterCalc(e2, p2);
    c1->RegisterDraw(e1, p1);
    c1->RegisterDraw(e2, p2);

    c1->RunCalcChain();
    c2->RunCalcChain();
    c1->RunDrawChain();
    c2->RunDrawChain();

    c1->CutList(&c1->calcChain);
    c2->CutList(&c2->drawChain);
    c1->Release();
    c2->Release();

    ChainElem *ce = c1->CreateElem(cb1);
    c1->Cut(ce);
    c2->Cut(e1);
    g_Chain->Cut(e1);
    g_Chain->Cut(e2);
    g_Chain->Cut(ce);

    c1->CreateAndRegisterCalcImmediate(cb1, arg1, p1);
    c2->CreateAndRegisterCalcImmediate(cb2, arg2, p2);
    c1->CreateAndRegisterDrawImmediate(cb1, arg1, p1);
    c2->CreateAndRegisterDrawImmediate(cb2, arg2, p2);
    c1->CreateAndRegisterCalc(cb1, arg1, p1);
    c2->CreateAndRegisterCalc(cb2, arg2, p2);
    c1->CreateAndRegisterDraw(cb1, arg1, p1);
    c2->CreateAndRegisterDraw(cb2, arg2, p2);

    ThreadInf *pt = new ThreadInf();
    pt->Start(fn1, arg1);
    t1->Start(fn2, arg2);
    t1->StartSuspended(fn1, arg1);
    t2->StartSuspended(fn2, arg2);
    t1->Resume();
    t2->Resume();
    t1->Stop();
    t2->Stop();
    delete pt;
    t1->~ThreadInf();

    SetThreadGlobalFlag4B4D8C();
    SetThreadFlagBit2(flagPtr);

    delete e2;
}
