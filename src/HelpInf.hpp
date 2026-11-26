#pragma once

#include "AnmManager.hpp"
#include "AnmResource.hpp"
#include "MenuCursor.hpp"
#include "Chain.hpp"

#include <string.h>

// TH128 HelpInf, identified by help.anm/help_%.2d.png and help.cpp strings.
struct HelpInf
{
    unsigned int flags;             // +0x000
    unsigned int unknown004;
    ChainElem *calcElement;          // +0x008
    ChainElem *drawElement;          // +0x00C
    int state;                      // +0x010
    ZunTimer timer;                  // +0x014
    MenuCursor cursor;               // +0x028
    AnmId vmIds[10];                 // +0x100, nine labels and one page VM
    int exitRequested;               // +0x128
    float originX;                   // +0x12C
    AnmResource *resource;           // +0x130
    void *pageData;                  // +0x134, owned until the upload completes
    int pageState;                   // +0x138
    char pageFilename[128];          // +0x13C
    int pageSize;                    // +0x1BC

    HelpInf();
    static HelpInf *Create();
    static void Destroy();
    static int __stdcall Initialize(HelpInf *help);
    void Shutdown();
    void CreateVm(int scriptIndex, const ZunVec3 *position);
    int Draw() { return 1; }
    static int __stdcall Update(HelpInf *help);
    static int __fastcall OnTick(void *help);
    static int __fastcall OnDraw(void *help);
    static void __cdecl LoadResourcesWorker();
    static void __cdecl LoadPageWorker();
};

typedef char HelpInfSize[(sizeof(HelpInf) == 0x1C0) ? 1 : -1];
typedef char HelpInfCursorOffset[(offsetof(HelpInf, cursor) == 0x28) ? 1 : -1];
typedef char HelpInfVmIdsOffset[(offsetof(HelpInf, vmIds) == 0x100) ? 1 : -1];
typedef char HelpInfPageSizeOffset[(offsetof(HelpInf, pageSize) == 0x1BC) ? 1 : -1];

extern HelpInf *g_HelpInf;

inline HelpInf::HelpInf()
{
    memset(this, 0, sizeof(*this));
    flags |= 2U;
    g_HelpInf = this;
}

// Reviewed launcher at 0x00434CB0. The target CRT calls these plain-RET,
// zero-argument bodies directly; the C++ callback type bridge remains open.
typedef void(__cdecl *HelpWorkerEntry)();
int __stdcall QueueHelpWorker(HelpWorkerEntry worker);
