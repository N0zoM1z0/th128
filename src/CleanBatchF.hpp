#pragma once

#include <windows.h>

struct CbfLog {
    char buffer[0x2000];
    char *cursor;
    unsigned char fatal;
    const char *__cdecl Print(const char *msg, ...);
};

// 1. 0x00410ea0 (222B)
struct CbfAnmVm {
    unsigned char pad000[0x42c];
    void *slot;        // +0x42c
    unsigned char pad430[0x80];
    unsigned int flags; // +0x4b0
};

struct CbfListNode {
    void *data;
    CbfListNode *next;
};

struct CbfAnmMgr {
    unsigned char pad000[0x4e90e0];
    void *slots[8];    // +0x4e90e0
    unsigned char pad4e9100[0x3d0604];
    CbfListNode *list1; // +0x8b9704
    int pad8b9708;
    CbfListNode *list2; // +0x8b970c
};

struct CbfChainElem {
    int pad00;
    unsigned int flags; // +0x04
};

struct CbfVObj {
    virtual void V0() {}
    virtual void V1() {}
    virtual void V2() {}
    virtual void V3() {}
    virtual void V4() {}
    virtual ~CbfVObj() {} // vtable +0x14
};

struct CbfCleanupObj {
    int pad00[2];
    CbfChainElem *calcElem; // +0x08
    CbfChainElem *drawElem; // +0x0c
    unsigned char pad10[0x4c];
    void *field5c;          // +0x5c
    unsigned char pad60[0x58];
    CbfListNode *itemList;  // +0xb8
    int padbc[2];
    void *fieldc4;          // +0xc4
};

// 2. 0x00420020 (165B)
struct CbfPlayerData {
    unsigned char pad00[0xa8];
    int encodedA8; // +0xa8
};

// 3. 0x004154b0 (341B)
struct CbfPreloadAnmObj {
    unsigned char pad000[0x124];
    int busy124; // +0x124
};

struct CbfStageObj {
    unsigned char pad00[0x84];
    CbfPreloadAnmObj *anms[8]; // +0x84
};

struct CbfChunkHeader {
    unsigned int magic;
    unsigned int count;
};

struct CbfStageLoader {
    virtual void V0() {}
    virtual void V1() {}
    virtual void LoadEcl(const char *name); // vtable +0x08
    int LoadResources(CbfChunkHeader *hdr);
};

// 4. 0x00405130 (299B)
struct CbfStdHeader {
    short count00;  // +0x00
    short count02;  // +0x02
    int off04;      // +0x04
    int off08;      // +0x08
    int pad0c;      // +0x0c
    char anmName[1]; // +0x10
};

struct CbfStdObj {
    unsigned char pad00[0x10];
    unsigned char *buf10;   // +0x10
    unsigned int *ptr14;    // +0x14
    unsigned char *ptr18;   // +0x18
    unsigned char *ptr1c;   // +0x1c
    unsigned char pad20[0x1a4];
    void *anm;              // +0x1c4
    void *ptr1c8;           // +0x1c8
    unsigned char pad1cc[0x3644];
    unsigned int flags3810; // +0x3810
    unsigned char pad3814[0x2c];
    void *rawData;          // +0x3840
    int rawSize;            // +0x3844
};

// Globals & helpers
extern CbfAnmMgr *g_CbfAnmMgr;
extern CbfPlayerData *g_CbfPlayerData;
extern CbfStageObj *g_CbfStage;
extern unsigned int g_CbfSupFlags;
extern CbfLog g_CbfLog;
extern char g_CbfPathBuf[256];
extern void *g_CbfMainMgr;

void __cdecl CbfDebugLog(...);
CbfPreloadAnmObj *__stdcall CbfPreloadAnm(CbfAnmMgr *mgr, int slot, const char *name);
void *__stdcall CbfOpenFile(const char *path, int *outSize, int mode);
void *CbfLoadAnm(int slot, const char *name);
void __stdcall CbfSetText(void *mgr, const char *str, int arg, void *out);

// Target functions (5 functions, 1297B)
void __stdcall CbfCleanup410ea0(CbfCleanupObj *obj);
void __stdcall CbfDecodeLivesBombs(int *outLives, int *outBombs);
int __stdcall CbfLoadStd(CbfStdObj *obj, const char *name);
void *__stdcall CbfFormatCommaNumber(void *out, int val, int arg);
