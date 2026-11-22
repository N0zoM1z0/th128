#pragma once

#include "AnmVm.hpp"

struct AnmVmListNode
{
    AnmVm *vm;
    AnmVmListNode *next;
};

// Animation manager (global pointer 0x004D2E50). Only the members used by the
// reconstructed methods are named.
struct AnmManager
{
    unsigned char unknown000[0x8B9704];
    AnmVmListNode *worldVms;    // +0x8B9704
    unsigned char unknown8B9708[4];
    AnmVmListNode *uiVms;       // +0x8B970C

    AnmVm *GetVmById(unsigned int id);
};

extern AnmManager *g_AnmManager;
