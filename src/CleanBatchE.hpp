#pragma once

#include <windows.h>

struct CbeListNode {
    void *data;
    CbeListNode *next;
    CbeListNode *prev;
};

struct CbeChainElem {
    int priority;       // +0x00
    unsigned int flags; // +0x04
    void *cb;           // +0x08
    void *onReg;        // +0x0c
    void *onCut;        // +0x10
    CbeListNode node;   // +0x14
    void *arg;          // +0x20 (total 0x24)

    CbeChainElem() {
        flags &= ~1U;
        cb = 0;
        onReg = 0;
        onCut = 0;
        priority = 0;
        node.data = this;
        node.next = 0;
        node.prev = 0;
    }
};

struct CbeChain {
    unsigned char pad00[0x14];
    CbeListNode calcHead; // +0x14
    unsigned char pad20[0x18];
    CbeListNode drawHead; // +0x38

    void RegisterCalc(CbeChainElem *elem, int prio);
    void RegisterDraw(CbeChainElem *elem, int prio);
};

struct CbeLog {
    char buffer[0x2000];
    char *cursor;
    unsigned char fatal;
    const char *__cdecl Print(const char *msg, ...);
};

// 1. 0x0041f8c0 (137B)
struct CbeObj41f8c0 {
    unsigned int flags;       // +0x00
    unsigned int pad04;       // +0x04
    CbeChainElem *calcElem;   // +0x08
    CbeChainElem *drawElem;   // +0x0c
    unsigned char pad10[0x7c]; // total 0x8c
    CbeObj41f8c0();
};

// 2. 0x0040e820 (232B)
struct CbeObj40e820 {
    int pad00[2];
    CbeChainElem *calcElem;    // +0x08
    CbeChainElem *drawElem;    // +0x0c
    unsigned char pad10[0x65dcc];
    void *anm;                 // +0x65ddc
};

// 3. 0x00411e50 (245B)
struct CbeObj411e50 {
    int pad00[2];
    CbeChainElem *calcElem;    // +0x08
    CbeChainElem *drawElem;    // +0x0c
    void *anm;                 // +0x10
};

// 4. 0x00433110 (245B)
struct CbeThreadObj {
    int pad00;           // +0x00
    unsigned int handle; // +0x04
    unsigned int tid;    // +0x08
    int flag0c;          // +0x0c
    int flag10;          // +0x10
    int pad14;           // +0x14
    void *proc;          // +0x18
    void Reset();
    void Start(unsigned int (__stdcall *fn)(void *), void *arg);
};

struct CbeObj433110 {
    int pad00[2];
    CbeChainElem *calcElem; // +0x08
    CbeChainElem *drawElem; // +0x0c
    CbeThreadObj thread;    // +0x10
};

// 5. 0x00429ea0 (257B)
struct CbeObj429ea0 {
    int pad00[2];
    CbeChainElem *calcElem;    // +0x08
    CbeChainElem *drawElem;    // +0x0c
    unsigned char arr10[0x5c0]; // +0x10
    void *ptr5d0;              // +0x5d0
    unsigned char pad5d4[0x20];
    void *anm;                 // +0x5f4
};

// 6. 0x00445330 (278B)
struct CbeObj445330 {
    int pad00[3];
    CbeChainElem *calcElem; // +0x0c
    CbeChainElem *drawElem; // +0x10
    void *anm14;            // +0x14
    void *anm18;            // +0x18
    unsigned char pad1c[0xdc];
    int flagf8;             // +0xf8
};

// 7. 0x00451d00 (166B)
struct CbePixel1555 {
    unsigned short rgb : 15;
    unsigned short a : 1;
};

// Globals & helpers
extern CbeChain g_CbeChain;
extern CbeLog g_CbeLog;
extern CbeObj41f8c0 *g_CbeObj41f8c0;
extern CbeObj445330 *g_CbeObj445330;
extern int g_CbeFlag4d2ad4;
extern int g_CbeTexFmt;
extern int g_CbeTexPitch;
extern unsigned char *g_CbeTexBuf;

void *CbeLoadAnm(int slot, const char *name);
int __fastcall CbeCb41fb70(void *arg);
int __fastcall CbeCb4101b0(void *arg);
int __fastcall CbeCb4101c0(void *arg);
int __fastcall CbeCb4120e0(void *arg);
int __fastcall CbeCb4120f0(void *arg);
int __fastcall CbeCb4335b0(void *arg);
int __fastcall CbeCb4335c0(void *arg);
unsigned int __stdcall CbeThread432f50(void *arg);
int __fastcall CbeCb42a370(void *arg);
int __fastcall CbeCb42a3e0(void *arg);
int __fastcall CbeCb445c80(void *arg);
int __fastcall CbeCb445c90(void *arg);

// Target functions (7 functions, 1560B)
CbeObj41f8c0 *CbeCreate41f8c0();
int __stdcall CbeInit40e820(CbeObj40e820 *obj);
int __stdcall CbeInit411e50(CbeObj411e50 *obj);
int __stdcall CbeInit433110(CbeObj433110 *obj);
int __stdcall CbeInit429ea0(CbeObj429ea0 *obj);
int CbeInit445330();
bool __stdcall CbeInvertAlpha(int height);
