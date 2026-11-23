#pragma once

#include "AnmVm.hpp"

// Animation manager (global pointer 0x004D2E50). Only observed members are
// named; the large gap still contains unrelated resource and rendering state.
struct AnmManager
{
    unsigned char unknown000[0x8B9704];
    AnmVmListNode *worldVms;       // +0x8B9704
    AnmVmListNode *worldVmsTail;   // +0x8B9708
    AnmVmListNode *uiVms;          // +0x8B970C
    AnmVmListNode *uiVmsTail;      // +0x8B9710
    unsigned char unknown8B9714[0x8C38FC - 0x8B9714];
    unsigned int nextVmId;         // +0x8C38FC

    AnmVm *GetVmById(unsigned int id);
};

extern AnmManager *g_AnmManager;
