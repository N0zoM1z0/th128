#pragma once

#include <windows.h>
#include <d3d9.h>
#include "AnmD3DX.hpp"

namespace th128 {

struct AdcwAnmSprite {
    unsigned char pad0[8];
    IDirect3DTexture9 **texSlot8;
};

struct AdcwAnmVm {
    unsigned char pad0[0x3f3];
    unsigned char alpha3f3;
    unsigned char pad3f4[0x428 - 0x3f4];
    AdcwAnmSprite *sprite428;
    unsigned char pad42c[0x4b0 - 0x42c];
    unsigned int flags4b0;
};

struct AdcwTexEntry {
    IDirect3DTexture9 *tex0;
    unsigned char pad4[0x14 - 4];
};

struct AdcwAnmResource {
    unsigned char pad0[0x120];
    AdcwTexEntry *textures120;
};

struct AdcwCopyBackBufReq {
    int anmSlot0;
    int texIdx4;
    int srcX8;
    int srcYC;
    int srcW10;
    int srcH14;
    int dstX18;
    int dstY1C;
    int dstW20;
    int dstH24;
};

struct AdcwAnmManager {
    unsigned char pad0[0x4e90c0];
    AdcwAnmResource *slots4e90c0[38];
    unsigned char padMid[0x4e9688 - (0x4e90c0 + 38 * 4)];
    IDirect3DTexture9 **curTex4e9688;
    unsigned char blendMode4e968c;
    unsigned char colorOp4e968d;
    unsigned char fvfMode4e968e;
    unsigned char zWrite4e968f;
    unsigned char pad4e9690[3];
    unsigned char stageMode4e9693;
    unsigned char pad4e9694[0x4e96ec - 0x4e9694];
    int batchCount4e96ec;

    void FlushSpriteBatch();
    void ApplyState(AdcwAnmVm *vm);
    void ApplyStateNoVm();

    int DrawMeshStrip(AdcwAnmVm *vm, const void *verts, int vertCount);
    int DrawTriangleStrip2D(const void *verts, int vertCount);
    int DrawTriangleFan2D(const void *verts, int vertCount);
    void CopyBackBufferToTexture(const AdcwCopyBackBufReq *req);
    void CopyTextureToTexture(int dstSlot, int dstTex, int srcSlot, int srcTex, const RECT *dstRect, const RECT *srcRect);
    void *PreloadAnm(int slot, const char *name);
};

void __stdcall AdcwClearTextureBlackOpaque(IDirect3DTexture9 **texPtr);

struct AdcwChainElem;
struct AdcwChainNode {
    AdcwChainElem *elem0;
    AdcwChainNode *next4;
    AdcwChainNode *prev8;
};

struct AdcwChainElem {
    short prio0;
    unsigned short pad2;
    unsigned int flags4;
    void *cb8;
    void *onRegC;
    void *onCut10;
    AdcwChainNode node14;
    void *arg20;

    AdcwChainElem() {
        flags4 &= ~1u;
        cb8 = NULL;
        onRegC = NULL;
        onCut10 = NULL;
        *(unsigned int *)&prio0 = 0;
        node14.elem0 = this;
        node14.next4 = NULL;
        node14.prev8 = NULL;
    }
};

struct AdcwChain {
    AdcwChainElem calcChain;
    AdcwChainElem drawChain;
    int RegisterCalc(AdcwChainElem *elem, int prio);
    int RegisterDraw(AdcwChainElem *elem, int prio);
};

struct AdcwBulletMgr {
    unsigned char pad0[8];
    AdcwChainElem *chain8;
    AdcwChainElem *chainC;
    void *firstBullet10;
    unsigned char pad14[0x64 - 0x14];
    unsigned char bullets64[0x8a780e - 0x64];
    short sentinel8a780e;
    unsigned char pad8a7810[0x8a7f9c - 0x8a7810];
    void *bulletAnm8a7f9c;

    int RegisterChains();
};

struct AdcwThreadInf {
    void *vtable0;
    HANDLE handle4;
    unsigned int threadId8;
    int flagsC;
    int running10;
    int pad14;
    unsigned int (__stdcall *entry18)(void *);

    void Stop();
};

struct AdcwMother {
    unsigned char pad0[8];
    AdcwChainElem *chain8;
    AdcwChainElem *chainC;
    AdcwThreadInf thread10;

    int __stdcall RegisterChainsAndThread();
};

struct AdcwErrorCtx {
    char buffer[0x2000];
    char *cursor;
    int fatal;
    const char *__cdecl Log(const char *fmt, ...);
};

extern IDirect3DDevice9 *g_AdcwDevice;
extern AdcwAnmManager *g_AdcwAnmMgr;
extern AdcwChain *g_AdcwChain;
extern AdcwErrorCtx g_AdcwErrorCtx;

int __fastcall AdcwBulletCalcCb(void *arg);
int __fastcall AdcwBulletDrawCb(void *arg);
int __fastcall AdcwMotherCalcCb(void *arg);
int __fastcall AdcwMotherDrawCb(void *arg);
unsigned int __stdcall AdcwMotherThreadProc(void *arg);

} // namespace th128
