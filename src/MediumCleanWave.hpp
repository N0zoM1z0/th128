#pragma once

#include <windows.h>

namespace th128 {

struct McwLockedRect {
    int Pitch;
    void *pBits;
};

struct McwSurfaceDesc {
    unsigned int Format;
    unsigned int Type;
    unsigned int Usage;
    unsigned int Pool;
    unsigned int MultiSampleType;
    unsigned int MultiSampleQuality;
    unsigned int Width;
    unsigned int Height;
};

struct McwD3dSurface {
    virtual HRESULT __stdcall QueryInterface(const IID &, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetDevice(void **) = 0;
    virtual HRESULT __stdcall SetPrivateData(const GUID &, const void *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall GetPrivateData(const GUID &, void *, DWORD *) = 0;
    virtual HRESULT __stdcall FreePrivateData(const GUID &) = 0;
    virtual DWORD __stdcall SetPriority(DWORD) = 0;
    virtual DWORD __stdcall GetPriority() = 0;
    virtual void __stdcall PreLoad() = 0;
    virtual unsigned int __stdcall GetType() = 0;
    virtual HRESULT __stdcall GetContainer(const IID &, void **) = 0;
    virtual HRESULT __stdcall GetDesc(McwSurfaceDesc *pDesc) = 0;
    virtual HRESULT __stdcall LockRect(McwLockedRect *pLockedRect, const RECT *pRect, DWORD Flags) = 0;
    virtual HRESULT __stdcall UnlockRect() = 0;
};

struct McwD3dDevice {
    virtual HRESULT __stdcall Slot00() = 0;
    virtual HRESULT __stdcall Slot01() = 0;
    virtual HRESULT __stdcall Slot02() = 0;
    virtual HRESULT __stdcall Slot03() = 0;
    virtual HRESULT __stdcall Slot04() = 0;
    virtual HRESULT __stdcall Slot05() = 0;
    virtual HRESULT __stdcall Slot06() = 0;
    virtual HRESULT __stdcall Slot07() = 0;
    virtual HRESULT __stdcall Slot08() = 0;
    virtual HRESULT __stdcall Slot09() = 0;
    virtual HRESULT __stdcall Slot10() = 0;
    virtual HRESULT __stdcall Slot11() = 0;
    virtual HRESULT __stdcall Slot12() = 0;
    virtual HRESULT __stdcall Slot13() = 0;
    virtual HRESULT __stdcall Slot14() = 0;
    virtual HRESULT __stdcall Slot15() = 0;
    virtual HRESULT __stdcall Slot16() = 0;
    virtual HRESULT __stdcall Slot17() = 0;
    virtual HRESULT __stdcall Slot18() = 0;
    virtual HRESULT __stdcall Slot19() = 0;
    virtual HRESULT __stdcall Slot20() = 0;
    virtual HRESULT __stdcall Slot21() = 0;
    virtual HRESULT __stdcall Slot22() = 0;
    virtual HRESULT __stdcall Slot23() = 0;
    virtual HRESULT __stdcall Slot24() = 0;
    virtual HRESULT __stdcall Slot25() = 0;
    virtual HRESULT __stdcall Slot26() = 0;
    virtual HRESULT __stdcall Slot27() = 0;
    virtual HRESULT __stdcall Slot28() = 0;
    virtual HRESULT __stdcall Slot29() = 0;
    virtual HRESULT __stdcall Slot30() = 0;
    virtual HRESULT __stdcall Slot31() = 0;
    virtual HRESULT __stdcall Slot32() = 0;
    virtual HRESULT __stdcall Slot33() = 0;
    virtual HRESULT __stdcall Slot34() = 0;
    virtual HRESULT __stdcall Slot35() = 0;
    virtual HRESULT __stdcall Slot36() = 0;
    virtual HRESULT __stdcall Slot37() = 0;
    virtual HRESULT __stdcall Slot38() = 0;
    virtual HRESULT __stdcall Slot39() = 0;
    virtual HRESULT __stdcall Slot40() = 0;
    virtual HRESULT __stdcall Slot41() = 0;
    virtual HRESULT __stdcall Slot42() = 0;
    virtual HRESULT __stdcall Slot43() = 0;
    virtual HRESULT __stdcall Slot44() = 0;
    virtual HRESULT __stdcall Slot45() = 0;
    virtual HRESULT __stdcall Slot46() = 0;
    virtual HRESULT __stdcall Slot47() = 0;
    virtual HRESULT __stdcall Slot48() = 0;
    virtual HRESULT __stdcall Slot49() = 0;
    virtual HRESULT __stdcall Slot50() = 0;
    virtual HRESULT __stdcall Slot51() = 0;
    virtual HRESULT __stdcall Slot52() = 0;
    virtual HRESULT __stdcall Slot53() = 0;
    virtual HRESULT __stdcall Slot54() = 0;
    virtual HRESULT __stdcall Slot55() = 0;
    virtual HRESULT __stdcall Slot56() = 0;
    virtual HRESULT __stdcall SetRenderState(DWORD state, DWORD value) = 0; // 57 = 0xe4
    virtual HRESULT __stdcall Slot58() = 0;
    virtual HRESULT __stdcall Slot59() = 0;
    virtual HRESULT __stdcall Slot60() = 0;
    virtual HRESULT __stdcall Slot61() = 0;
    virtual HRESULT __stdcall Slot62() = 0;
    virtual HRESULT __stdcall Slot63() = 0;
    virtual HRESULT __stdcall Slot64() = 0;
    virtual HRESULT __stdcall SetTexture(DWORD stage, void *tex) = 0; // 65 = 0x104
    virtual HRESULT __stdcall Slot66() = 0;
    virtual HRESULT __stdcall SetTextureStageState(DWORD stage, DWORD type, DWORD val) = 0; // 67 = 0x10c
    virtual HRESULT __stdcall Slot68() = 0;
    virtual HRESULT __stdcall Slot69() = 0;
    virtual HRESULT __stdcall Slot70() = 0;
    virtual HRESULT __stdcall Slot71() = 0;
    virtual HRESULT __stdcall Slot72() = 0;
    virtual HRESULT __stdcall Slot73() = 0;
    virtual HRESULT __stdcall Slot74() = 0;
    virtual HRESULT __stdcall Slot75() = 0;
    virtual HRESULT __stdcall Slot76() = 0;
    virtual HRESULT __stdcall Slot77() = 0;
    virtual HRESULT __stdcall Slot78() = 0;
    virtual HRESULT __stdcall Slot79() = 0;
    virtual HRESULT __stdcall Slot80() = 0;
    virtual HRESULT __stdcall Slot81() = 0;
    virtual HRESULT __stdcall Slot82() = 0;
    virtual HRESULT __stdcall DrawPrimitiveUP(DWORD type, UINT count, const void *verts, UINT stride) = 0; // 83 = 0x14c
    virtual HRESULT __stdcall Slot84() = 0;
    virtual HRESULT __stdcall Slot85() = 0;
    virtual HRESULT __stdcall Slot86() = 0;
    virtual HRESULT __stdcall Slot87() = 0;
    virtual HRESULT __stdcall Slot88() = 0;
    virtual HRESULT __stdcall SetFVF(DWORD fvf) = 0; // 89 = 0x164
};

struct McwAnmSpriteEntry {
    unsigned char pad00[0x08];
    void **texSlot;
};

struct McwAnmVm {
    unsigned char pad00[0x428];
    McwAnmSpriteEntry *sprite;
    void *anmRes;
    unsigned char pad430[0x80];
    unsigned int flags;
};

struct McwVmNode {
    McwAnmVm *vm;
    McwVmNode *next;
};

struct McwAnmManager {
    unsigned char pad00[0x4e90e0];
    void *stageAnms[8];
    unsigned char pad4e9100[0x588];
    void **curTexSlot;
    unsigned char pad4e968c[0x02];
    unsigned char fvfState;
    unsigned char pad4e968f[0x5d];
    int batchCount;
    unsigned char pad4e96f0[0x3d0014];
    McwVmNode *worldListHead;
    unsigned char pad8b9708[0x04];
    McwVmNode *uiListHead;

    void FlushSpriteBatch();
    void ApplyState(McwAnmVm *vm);
    void MarkResourceVmsForDelete(void *res);
    int DrawAnmVmVertices(McwAnmVm *vm, const void *verts, int count);
};

struct McwTextImage {
    unsigned char pad00[0x100];
    unsigned int fmt;
    int width;
    int height;
    unsigned char pad10c[0x04];
    int stride;
    unsigned char pad114[0x08];
    void *hBitmap;
    unsigned char *bits;

    bool UploadToSurface(McwD3dSurface *surf);
    bool DownloadFromSurface(McwD3dSurface *surf);
};

struct McwBgmEntry {
    char name[0x34];
};

struct McwSoundManager {
    unsigned char pad00[0x1984];
    McwBgmEntry *bgmTable;

    int FindBgmIndex(const char *path);
};

struct McwEnemyObj {
    virtual void Slot00() = 0;
    virtual void Slot01() = 0;
    virtual void Slot02() = 0;
    virtual void Slot03() = 0;
    virtual void Slot04() = 0;
    virtual void Pause(int flag) = 0; // +0x14
};

struct McwEnemyNode {
    McwEnemyObj *enemy;
    McwEnemyNode *next;
};

struct McwChainElem {
    McwChainElem *prev;
    McwChainElem *next;
    unsigned int flags;
};

struct McwStageChainElem {
    unsigned char pad00[0x04];
    unsigned int flags;
};

struct McwStageObj {
    unsigned char pad00[0x08];
    McwStageChainElem *calcElem;
    McwStageChainElem *drawElem;
    unsigned char pad10[0x4c];
    int f5c;
    unsigned char pad60[0x58];
    McwEnemyNode *enemyListHead;
    unsigned char padbc[0x08];
    int fc4;

    void PauseAllStageAnmsAndEnemies();
};

struct McwChain {
    McwChainElem *head;
    McwChainElem *tail;
};

void __fastcall McwChainCut(McwChain *chain, McwChainElem *elem);

struct McwBullet {
    unsigned char data[0x11b8];
    ~McwBullet();
};

struct McwBulletManager {
    unsigned char pad00[0x08];
    McwChainElem *calcElem;
    McwChainElem *drawElem;
    unsigned char pad10[0x54];
    McwBullet bullets[0x7d1];
    void *anmRes;

    ~McwBulletManager();
};

struct McwSoundTest {
    unsigned char pad00[0x08];
    McwChainElem *calcElem;
    McwChainElem *drawElem;
    unsigned char pad10[0x4b4];
    void *buf4c4;

    ~McwSoundTest();
};

struct McwItem {
    unsigned char data[0xa40];
    ~McwItem();
};

struct McwItemManager {
    unsigned char pad00[0x08];
    McwChainElem *calcElem;
    McwChainElem *drawElem;
    unsigned char pad10[0x04];
    McwItem items[0x2cc];

    ~McwItemManager();
};

struct McwScoreHeader {
    unsigned int magic;
    unsigned int pad04;
    unsigned int size;
    char name[9];
    unsigned char pad15[0x3b];
    unsigned short rndData[0x1ee];

    void InitHeader();
};

struct McwMemStream {
    unsigned int size;
    void *cur;
    void *base;

    virtual ~McwMemStream();
    virtual void Close();
    bool LoadFromResource(const char *resName, int unusedFlag);
};

extern McwD3dDevice *g_McwDevice;
extern McwAnmManager *g_McwAnmMgr;
extern McwChain *g_McwChain;
extern unsigned int g_McwFlags;
extern CRITICAL_SECTION g_McwChainCs;
extern unsigned char g_McwChainCsDepth;
extern CRITICAL_SECTION g_McwRngCs;
extern unsigned char g_McwRngCsDepth;
extern unsigned short g_McwRngSeed;
extern unsigned int g_McwRngCount;
extern McwBulletManager *g_McwBulletMgr;
extern McwSoundTest *g_McwSoundTest;
extern McwItemManager *g_McwItemMgr;

} // namespace th128
