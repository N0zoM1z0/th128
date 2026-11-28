#pragma once

#include <windows.h>

namespace th128 {

struct CbnListNode {
    void* owner;
    CbnListNode* next;
    CbnListNode* prev;
};

struct CbnAnmVm {
    unsigned int id;
    unsigned char pad4[0x10];
    struct CbnAnmNode* children14;
    void* parent18;
    unsigned char pad1c[0x4b0 - 0x1c];
    unsigned int flags4b0;
};

struct CbnAnmNode {
    CbnAnmVm* vm;
    CbnAnmNode* next;
};

struct CbnAnmMgr {
    unsigned char pad0[0x8b9704];
    CbnAnmNode* listA;
    int pad8b9708;
    CbnAnmNode* listB;

    CbnAnmVm* FindVmInline(unsigned int id) {
        if (id == 0) return 0;
        for (CbnAnmNode* n = listA; n != 0; n = n->next) {
            if (n->vm->id == id) return n->vm;
        }
        for (CbnAnmNode* n = listB; n != 0; n = n->next) {
            if (n->vm->id == id) return n->vm;
        }
        return 0;
    }
};

struct CbnEnemyMgr {
    unsigned char pad0[0x5c];
    void* bossSlots[16];
    unsigned char pad9c[0xb8 - 0x9c];
    CbnListNode* head;
    CbnListNode* tail;
    int count;
};

struct CbnBaseNode {
    void* data;
    CbnBaseNode* next;
};

struct CbnEnemySub {
    int dummy;
    void Cleanup();
};

struct CbnEnemyBase {
    unsigned char pad4[0x11b4 - 4];
    CbnBaseNode* baseList11b4;

    virtual void DummyVFunc() {}
    ~CbnEnemyBase() {
        CbnBaseNode* n = baseList11b4;
        while (n != 0) {
            void* d = n->data;
            CbnBaseNode* next = n->next;
            ::operator delete(d);
            ::operator delete(n);
            n = next;
        }
    }
};

struct CbnEnemy : public CbnEnemyBase {
    unsigned char pad11b8[0x12a8 - 0x11b8];
    unsigned int anmIds[16];
    unsigned char pad12e8[0x145c - 0x12e8];
    CbnListNode listEntry145c;
    unsigned char pad1468[0x50af - 0x1468];
    unsigned char flags50af;
    unsigned char pad50b0[0x50bc - 0x50b0];
    int bossIdx50bc;
    unsigned char pad50c0[0x5148 - 0x50c0];
    CbnEnemySub* sub5148;

    ~CbnEnemy();
};

struct CbnSurfaceDesc {
    unsigned int Format;
    unsigned int Type;
    unsigned int Usage;
    unsigned int Pool;
    unsigned int MultiSampleType;
    unsigned int MultiSampleQuality;
    unsigned int Width;
    unsigned int Height;
};

struct CbnLockedRect {
    int Pitch;
    void* pBits;
};

struct CbnSurface {
    virtual void V0() = 0;
    virtual void V1() = 0;
    virtual void V2() = 0;
    virtual void V3() = 0;
    virtual void V4() = 0;
    virtual void V5() = 0;
    virtual void V6() = 0;
    virtual void V7() = 0;
    virtual void V8() = 0;
    virtual void V9() = 0;
    virtual void V10() = 0;
    virtual void V11() = 0;
    virtual int __stdcall GetDesc(CbnSurfaceDesc* desc) = 0;                       // +0x30
    virtual int __stdcall LockRect(CbnLockedRect* lr, const RECT* r, DWORD f) = 0; // +0x34
    virtual int __stdcall UnlockRect() = 0;                                        // +0x38
};

struct CbnTextTex {
    unsigned char pad0[0x100];
    unsigned int format100;
    int width104;
    int height108;
    int pad10c;
    int srcPitch110;
    unsigned char pad114[0x11c - 0x114];
    int ready11c;
    unsigned char* srcBits120;

    bool __stdcall BlitToSurface(CbnSurface* surf, int y, int w, int h, int x);
};

struct CbnWaveHdr {
    unsigned char pad0[0x24];
    unsigned int sampleRate24;
    unsigned char pad28[4];
    unsigned short blockAlign2c;
};

struct CbnGuid16 {
    unsigned int a, b, c, d;
};

struct CbnStreaming {
    int dummy;
    int __stdcall CreateFromMem(void* dsound, void* buf, CbnWaveHdr* hdr, CbnGuid16 guid, HANDLE ev, unsigned int bytes, unsigned int size);
};

extern CbnEnemyMgr* g_CbnEnemyMgr;
extern CbnAnmMgr* g_CbnAnmMgr;

extern void* g_CbnDsound;
extern unsigned char g_CbnBgmEnabled;
extern void* g_CbnSoundMgr;
extern unsigned char g_CbnSoundFlags;
extern char g_CbnBgmNames[16][256];
extern void* g_CbnBgmBufs[16];
extern CbnWaveHdr* g_CbnBgmHdrs[16];
extern unsigned int g_CbnBgmSizes[16];
extern unsigned int g_CbnBgmAux[16];
extern HANDLE g_CbnBgmEvent;
extern HANDLE g_CbnBgmThread;
extern DWORD g_CbnBgmThreadId;
extern void* g_CbnSoundParam;
extern CbnGuid16 g_CbnGuid;
extern CbnStreaming g_CbnStreaming;
extern int g_CbnCurBgmSlot;

int __fastcall CbnPlayBgmFallback(const char* name);
void __cdecl CbnLogPrintf(const char* fmt, ...);
DWORD __stdcall CbnBgmThreadProc(LPVOID param);

int CbnPlayBgmSlot(int slot);

} // namespace th128
