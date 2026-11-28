#include "ReplayInitBatch.hpp"
#include <stdio.h>
#include <string.h>
#include <windows.h>

Chain *g_Chain = 0;
volatile int g_RibSink = 0;

__declspec(noinline) static void RibTouch(int v) {
    g_RibSink ^= v;
}

__declspec(noinline) RibSubObj::RibSubObj() {
    memset(this, 0, sizeof(RibSubObj));
    data[0] = 1;
    RibTouch((int)this);
}

__declspec(noinline) short RibGameState::GetVal() {
    DWORD t = GetTickCount();
    return (short)(score + field08 + field1c + rank + (int)t);
}

__declspec(noinline) void RibReplayMgr::ResetStageBlocks(int stage) {
    if (stageHeaders[stage] != 0) {
        stageHeaders[stage] = 0;
    }
    g_RibSink += stage + (int)this;
}

__declspec(noinline) void *RibReplayMgr::AllocStageBlock(int stage) {
    unsigned char *p = new unsigned char[0x18b0];
    if (p != 0) {
        memset(p, 0, 0x18b0);
    }
    g_RibSink += (int)p + stage + (int)this + (int)stageHeaders[stage];
    return p;
}

__declspec(noinline) int RibReplayMgr::LoadReplay(const char *filename) {
    char buf[0x100];
    strcpy((char *)this + 0x3b4, filename);
    if ((g_RibGameState.flags & 0x20) == 0) {
        sprintf(buf, "replay/%s", filename);
        RibSubObj tmp = g_RibPlayerData->sub;
        dataHeader->sub = tmp;
        DWORD t = GetTickCount();
        g_RibSink += (int)t + tmp.data[0] + buf[0];
    }
    fileHeader = (RibFileHeader *)GetTickCount();
    g_RibSink += (int)this + curStage + mode;
    return mode - 1;
}

__declspec(noinline) int __fastcall RibCalc0(void *arg) { return (int)arg + 1; }
__declspec(noinline) int __fastcall RibCalc1(void *arg) { return (int)arg + 2; }
__declspec(noinline) int __fastcall RibCalc2(void *arg) { return (int)arg + 3; }
__declspec(noinline) int __fastcall RibDraw(void *arg) { return (int)arg + 4; }

__declspec(noinline) int Chain::RegisterCalc(ChainElem *elem, int priority) {
    int result = 0;
    Chain *chain = g_Chain;
    if (elem->onRegistration != 0) {
        result = elem->onRegistration(elem->arg);
        elem->onRegistration = 0;
    }
    RibTouch(0);
    ChainNode *cursor = &chain->calcChain.node;
    elem->priority = priority;
    while (cursor->next != 0) {
        if (cursor->next->owner->priority >= priority) {
            break;
        }
        cursor = cursor->next;
    }
    ChainNode *node = &elem->node;
    if (cursor->next != 0) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    RibTouch(1);
    return result;
}

__declspec(noinline) int Chain::RegisterDraw(ChainElem *elem, int priority) {
    int result = 0;
    Chain *chain = g_Chain;
    if (elem->onRegistration != 0) {
        result = elem->onRegistration(elem->arg);
        elem->onRegistration = 0;
    }
    RibTouch(0);
    ChainNode *cursor = &chain->drawChain.node;
    elem->priority = priority;
    while (cursor->next != 0) {
        if (cursor->next->owner->priority >= priority) {
            break;
        }
        cursor = cursor->next;
    }
    ChainNode *node = &elem->node;
    if (cursor->next != 0) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    RibTouch(1);
    return result;
}

__declspec(noinline) ChainElem *Chain::CreateAndRegisterCalc(ChainCallback fn, void *arg, int priority) {
    ChainCallback volatile *pFn = &fn;
    void *volatile *pArg = &arg;
    ChainElem *e = new ChainElem();
    e->SetCallback(*pFn);
    e->flags = (e->flags & ~2u) | 1u;
    e->arg = *pArg;
    g_Chain->RegisterCalc(e, priority);
    return e;
}

__declspec(noinline) ChainElem *Chain::CreateAndRegisterDraw(ChainCallback fn, void *arg, int priority) {
    ChainCallback volatile *pFn = &fn;
    void *volatile *pArg = &arg;
    ChainElem *e = new ChainElem();
    e->SetCallback(*pFn);
    e->flags = (e->flags & ~2u) | 1u;
    e->arg = *pArg;
    g_Chain->RegisterDraw(e, priority);
    return e;
}

volatile unsigned int *g_RibFlagPtr = &g_RibFlag4d33cc;
volatile int *g_RibRngCountPtr = &g_RibRngCount;
volatile unsigned short *g_RibRngSeedPtr = &g_RibRngSeed;

extern "C" int __stdcall ReplayInitBatchProbe(RibReplayMgr *mgr, RibPlayerData *pd, Chain *chain, int mode, const char *filename) {
    g_Chain = chain;
    g_RibPlayerData = pd;
    if (mode & 8) {
        RibSubObj tmp;
        g_RibSink += tmp.data[0] + g_RibGameState.GetVal();
        mgr->ResetStageBlocks(mode);
        mgr->AllocStageBlock(mode);
        mgr->LoadReplay(filename);
        chain->CreateAndRegisterCalc(&RibCalc0, pd, mode);
        chain->CreateAndRegisterDraw(&RibCalc2, pd, mode + 1);
    }
    return mgr->Initialize(filename, mode) + g_RibSink;
}
