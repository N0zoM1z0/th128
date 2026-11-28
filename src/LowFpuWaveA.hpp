#pragma once

#include <windows.h>

namespace th128 {

struct LfaD3dRect {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
};

struct LfaD3dDevice9 {
    virtual HRESULT __stdcall S00() = 0;
    virtual HRESULT __stdcall S01() = 0;
    virtual HRESULT __stdcall S02() = 0;
    virtual HRESULT __stdcall S03() = 0;
    virtual HRESULT __stdcall S04() = 0;
    virtual HRESULT __stdcall S05() = 0;
    virtual HRESULT __stdcall S06() = 0;
    virtual HRESULT __stdcall S07() = 0;
    virtual HRESULT __stdcall S08() = 0;
    virtual HRESULT __stdcall S09() = 0;
    virtual HRESULT __stdcall S10() = 0;
    virtual HRESULT __stdcall S11() = 0;
    virtual HRESULT __stdcall S12() = 0;
    virtual HRESULT __stdcall S13() = 0;
    virtual HRESULT __stdcall S14() = 0;
    virtual HRESULT __stdcall S15() = 0;
    virtual HRESULT __stdcall S16() = 0;
    virtual HRESULT __stdcall S17() = 0;
    virtual HRESULT __stdcall S18() = 0;
    virtual HRESULT __stdcall S19() = 0;
    virtual HRESULT __stdcall S20() = 0;
    virtual HRESULT __stdcall S21() = 0;
    virtual HRESULT __stdcall S22() = 0;
    virtual HRESULT __stdcall S23() = 0;
    virtual HRESULT __stdcall S24() = 0;
    virtual HRESULT __stdcall S25() = 0;
    virtual HRESULT __stdcall S26() = 0;
    virtual HRESULT __stdcall S27() = 0;
    virtual HRESULT __stdcall S28() = 0;
    virtual HRESULT __stdcall S29() = 0;
    virtual HRESULT __stdcall S30() = 0;
    virtual HRESULT __stdcall S31() = 0;
    virtual HRESULT __stdcall S32() = 0;
    virtual HRESULT __stdcall S33() = 0;
    virtual HRESULT __stdcall S34() = 0;
    virtual HRESULT __stdcall S35() = 0;
    virtual HRESULT __stdcall S36() = 0;
    virtual HRESULT __stdcall SetRenderTarget(DWORD idx, void *surf) = 0; // 37 = 0x94
    virtual HRESULT __stdcall S38() = 0;
    virtual HRESULT __stdcall S39() = 0;
    virtual HRESULT __stdcall S40() = 0;
    virtual HRESULT __stdcall S41() = 0;
    virtual HRESULT __stdcall S42() = 0;
    virtual HRESULT __stdcall Clear(DWORD count, const LfaD3dRect *rects, DWORD flags,
                                    DWORD color, float z, DWORD stencil) = 0; // 43 = 0xac
};

struct LfaAnmManager {
    void FlushSpriteBatch();
};

struct LfaTimer {
    int prev;
    int cur;
    float curF;
    float *scale;
    unsigned int flags;

    inline void ResetToMinusOne() {
        if (!(this->flags & 1)) {
            this->curF = 0.0f;
            this->cur = 0;
            this->prev = -999999;
            extern float g_LfaTimeScale;
            this->scale = &g_LfaTimeScale;
            this->flags |= 1;
        }
        this->curF = 0.0f;
        this->cur = 0;
        this->prev = -1;
    }
};

struct LfaAnmVmEffect {
    unsigned char pad00[0x13c];
    int f13c;
    int f140;
    int f144;
    int f148;
    LfaTimer timer14c;
    int f160;
    int f164;

    void SetEffectState(int a, unsigned char b, unsigned char c, unsigned char d);
};

struct LfaDrawNode {
    virtual void S0() = 0;
    virtual void S1() = 0;
    virtual void S2() = 0;
    virtual void S3() = 0;
    virtual void S4() = 0;
    virtual void Draw() = 0; // +0x14
    unsigned char pad04[0x04];
    LfaDrawNode *next;
    int skipFlag;
};

struct LfaLaserMgr {
    unsigned char pad00[0x18];
    LfaDrawNode *head;

    int OnDraw();
};

struct LfaEclFrame {
    float time;
    void *sub;
};

struct LfaEclSubSlot {
    unsigned char pad00[0x28];
    void *ptr28;
};

struct LfaEclVm {
    void *vtable;
    LfaEclFrame *curFrame;
    LfaEclFrame mainFrame;
    unsigned char pad10[0x1000];
    int f1010;
    int f1014;
    int f1018;
    LfaEclVm *selfPtr;
    int f1020;
    LfaEclSubSlot slots[8];
    unsigned char pad1184[0x24];
    unsigned int flags11a8;
    void *resource;
    LfaEclFrame *framePtr11b0;
    int f11b4;
    int f11b8;

    void Reset();
};

struct LfaAnmVmCtor {
    unsigned char data[0x4e8];
    LfaAnmVmCtor();
};

struct LfaAsciiEntry {
    unsigned char pad00[0x160];
    unsigned int flags;
    LfaAsciiEntry() { flags &= ~1u; }
};

struct LfaAsciiManager {
    virtual ~LfaAsciiManager();
    unsigned int flags04;
    unsigned char pad08[0x0c];
    LfaAnmVmCtor vm1;
    LfaAnmVmCtor vm2;
    LfaAsciiEntry entries[0x141];
    unsigned char padTail[0x1c84c - 0x9e4 - 0x141 * 0x164];
    int f1c84c;
    unsigned char pad1c850[0x08];
    float f1c858;
    float f1c85c;
    unsigned char pad1c860[0x04];
    int f1c864;
    unsigned char pad1c868[0x10];
    int f1c878;
    int f1c87c;
    unsigned char pad1c880[0x0c];
    int f1c88c;
    unsigned char pad1c890[0x18];

    LfaAsciiManager();
};

struct LfaVec3 {
    float x;
    float y;
    float z;
};

struct LfaPopupEntry {
    char digits[12];
    LfaVec3 pos;
    int color;
    LfaTimer timer;
    unsigned char pad30[0x08];
    unsigned char active;
    unsigned char digitCount;
    unsigned char pad3a[0x06];
};

struct LfaPopupMgr {
    unsigned char pad00[0x14];
    int nextIdx;
    unsigned char pad18[0x4e8];
    LfaPopupEntry entries[10];
};

extern void *g_LfaRenderTargetA;
extern void *g_LfaRenderTargetB;
extern LfaAnmManager *g_LfaAnmMgr;
extern LfaD3dDevice9 *g_LfaDevice;
extern int g_LfaViewX;
extern int g_LfaViewY;
extern int g_LfaViewW;
extern int g_LfaViewH;
extern DWORD g_LfaClearColor;
extern float g_LfaTimeScale;
extern LfaAsciiManager *g_LfaAsciiMgr;
extern LfaPopupMgr *g_LfaPopupMgr;

int LfaClearRenderTargetB();
int LfaClearRenderTargetA();
void LfaSpawnScorePopup(const LfaVec3 *pos, int value, int color);

} // namespace th128
