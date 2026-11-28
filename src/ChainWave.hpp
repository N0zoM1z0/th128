#pragma once

#include <windows.h>

namespace th128 {

struct CwChainElem;

struct CwChainNode {
    CwChainElem *owner; // +0x00
    CwChainNode *next;  // +0x04
    CwChainNode *prev;  // +0x08
};

typedef int (__fastcall *CwChainCallback)(void *arg);

struct CwChainElem {
    int priority;           // +0x00
    unsigned int flags;     // +0x04 (bit 0 = heap?, bit 1 = active)
    CwChainCallback cb;     // +0x08
    CwChainCallback onReg;  // +0x0C
    CwChainCallback onUnreg;// +0x10
    CwChainNode node;       // +0x14 (owner=+0x14, next=+0x18, prev=+0x1C)
    void *arg;              // +0x20

    CwChainElem() {
        flags &= ~1u;
        cb = NULL;
        onReg = NULL;
        onUnreg = NULL;
        priority = 0;
        node.owner = this;
        node.next = NULL;
        node.prev = NULL;
    }
};

struct CwChain {
    CwChainElem calcHead;   // +0x00 (calcHead.node.next = +0x18)
    CwChainElem drawHead;   // +0x24 (drawHead.node.next = +0x3C)
    CwChainNode *nextNode;  // +0x48
    int pauseFlag;          // +0x4C

    CwChain();
    int RunCalcChain();
    static int RunDrawChain();
    void CutList(CwChainElem *head);
    void Cut(CwChainElem *elem);
};

extern unsigned int g_CwGameFlags;
extern CRITICAL_SECTION g_CwChainCs;
extern unsigned char g_CwChainLockDepth;
extern CwChain *g_CwChain;

void __fastcall CwChainCutInternal(CwChain *chain, CwChainElem *elem);

} // namespace th128
