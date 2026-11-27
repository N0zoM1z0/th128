#pragma once

#include <windows.h>

namespace th128 {

struct Zc2Vec3 {
    float x, y, z;
    Zc2Vec3() {}
    Zc2Vec3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}
};

struct Zc2CameraSlot {
    Zc2Vec3 pos;        // +0x00
    Zc2Vec3 at;         // +0x0c
    Zc2Vec3 up;         // +0x18
    unsigned char pad24[0x3c - 0x24];
    Zc2Vec3 offset;     // +0x3c
    float fovOrFar;     // +0x48
    unsigned char pad4c[0xcc - 0x4c];
    int vpX;            // +0xcc
    int vpY;            // +0xd0
    int vpW;            // +0xd4
    int vpH;            // +0xd8
    float vpMinZ;       // +0xdc
    float vpMaxZ;       // +0xe0
    int mode;           // +0xe4
    unsigned char pade8[0x118 - 0xe8];
};

struct Zc2StageCamOwner {
    unsigned char pad00[0x590];
    unsigned int flags;

    void InitGameCameras();
};

struct Zc2ScoreEntry {
    int score;          // +0x00
    unsigned char stage;// +0x04
    unsigned char cont; // +0x05
    char name[10];      // +0x06
    int timeSec;        // +0x10
    int pad14;          // +0x14
    float slowRate;     // +0x18
    int pad1c;          // +0x1c
};

struct Zc2ScoreFile {
    unsigned char pad00[0x10];
    Zc2ScoreEntry entries[5][10];
};

struct Zc2FpsCounter {
    unsigned char pad00[0x24];
    double totalTime;   // +0x24
    double frameCount;  // +0x2c
};

struct Zc2ZunTimer {
    int prev;
    int cur;
    float curF;
    float *speedPtr;
    unsigned int flags;
};

struct Zc2IceShard {
    float f00;          // +0x00
    float f04;          // +0x04
    unsigned char pad08[0x18 - 0x08];
    Zc2Vec3 pos;        // +0x18
    unsigned char pad24[0x4c - 0x24];
    Zc2ZunTimer timer;  // +0x4c
    int f60;            // +0x60
    int f64;            // +0x64
    int f68;            // +0x68
    int f6c;            // +0x6c
    int f70;            // +0x70
    unsigned int flags; // +0x74
};

struct Zc2Player {
    unsigned char pad00[0x8edc];
    Zc2IceShard shards[2000];
    unsigned char pad_after[0x43a8c - (0x8edc + 2000 * 0x78)];
    int nextShardIdx;

    Zc2IceShard *SpawnIceShard(const Zc2Vec3 *pos, float a, float b, int timerVal, int c, int d);
};

struct Zc2Fade {
    unsigned char pad00[0x18];
    int alpha;          // +0x18
    int duration;       // +0x1c
    unsigned char pad20[0x2c - 0x20];
    int mode;           // +0x2c
    int timerPrev;      // +0x30
    int timerCur;       // +0x34
    float timerCurF;    // +0x38
    float *speedPtr;    // +0x3c

    int OnCalc();
};

struct Zc2QuadVertex {
    Zc2Vec3 pos;
    float u, v;
};

struct Zc2QuadVertexD {
    Zc2Vec3 pos;
    unsigned int pad;
    float u, v;
};

struct Zc2D3DVtxBufVtbl {
    void *pad[11];
    HRESULT (__stdcall *Lock)(void *self, unsigned int offset, unsigned int size, void **data, unsigned int flags);
    HRESULT (__stdcall *Unlock)(void *self);
};

struct Zc2D3DVtxBuf {
    Zc2D3DVtxBufVtbl *vtbl;
};

struct Zc2D3DDevVtbl {
    void *pad00[26];
    HRESULT (__stdcall *CreateVertexBuffer)(void *self, unsigned int length, unsigned int usage, unsigned int fvf, unsigned int pool, Zc2D3DVtxBuf **ppVB, void *shared);
    void *pad1b[73];
    HRESULT (__stdcall *SetStreamSource)(void *self, unsigned int stream, Zc2D3DVtxBuf *vb, unsigned int offset, unsigned int stride);
};

struct Zc2D3DDev {
    Zc2D3DDevVtbl *vtbl;
};

struct Zc2AnmMgrVB {
    unsigned char pad00[0x4e9698];
    Zc2D3DVtxBuf *quadVb;
    Zc2QuadVertex quadVerts[4];
};

struct Zc2LockedRect {
    int pitch;
    void *pBits;
};

struct Zc2SurfDesc {
    unsigned int format;
    unsigned int type;
    unsigned int usage;
    unsigned int pool;
    unsigned int msType;
    unsigned int msQual;
    unsigned int width;
    unsigned int height;
};

struct Zc2D3DSurfVtbl {
    void *pad00[2];
    unsigned long (__stdcall *Release)(void *self);
    void *pad03[9];
    HRESULT (__stdcall *GetDesc)(void *self, Zc2SurfDesc *desc);
    HRESULT (__stdcall *LockRect)(void *self, Zc2LockedRect *rect, const void *r, unsigned int flags);
    HRESULT (__stdcall *UnlockRect)(void *self);
};

struct Zc2D3DSurf {
    Zc2D3DSurfVtbl *vtbl;
};

struct Zc2D3DTexVtbl {
    void *pad00[18];
    HRESULT (__stdcall *GetSurfaceLevel)(void *self, unsigned int level, Zc2D3DSurf **ppSurf);
};

struct Zc2D3DTex {
    Zc2D3DTexVtbl *vtbl;
};

struct Zc2EclHeader {
    unsigned int magic;     // +0x00 'SCPT'
    unsigned short version; // +0x04
    unsigned short includeCount; // +0x06
    unsigned int pad08[2];  // +0x08
    unsigned short subCount;// +0x10
    unsigned short pad12;
    unsigned int pad14[4];  // +0x14..+0x23
};

struct Zc2EclSubEntry {
    char *name;
    unsigned char *code;
};

struct Zc2EclManager {
    virtual void DtorStub() {}
    virtual void LoadIncludes(void *includeBlock) {}

    int fileCount;          // +0x04
    int totalSubs;          // +0x08
    Zc2EclHeader *files[32];// +0x0c
    Zc2EclSubEntry *subs;   // +0x8c

    int LoadFileData(Zc2EclHeader *hdr);
};

extern Zc2CameraSlot g_Zc2Cameras[3];
extern int g_Zc2Difficulty;
extern int g_Zc2Score;
extern unsigned char g_Zc2Continues;
extern unsigned char g_Zc2StageNum;
extern Zc2FpsCounter *g_Zc2Fps;
extern float g_Zc2GameSpeed;

extern unsigned char g_Zc2WinMode;
extern unsigned char g_Zc2CfgByte4e;
extern unsigned char g_Zc2CfgByte53;
extern int g_Zc2WinX;
extern int g_Zc2WinY;
extern unsigned int g_Zc2CfgFlags;
extern unsigned int g_Zc2SupFlags;
extern int g_Zc2IsWindowed;
extern int g_Zc2WindowActive;
extern int g_Zc2WindowClosing;
extern HWND g_Zc2MainHwnd;
extern HWND g_Zc2SupHwnd;
extern RECT g_Zc2WinRect;
extern int g_Zc2InputCfg[13];
extern LRESULT CALLBACK Zc2MainWndProc(HWND, UINT, WPARAM, LPARAM);

extern Zc2AnmMgrVB *g_Zc2AnmMgr;
extern Zc2D3DDev *g_Zc2D3DDev;
extern Zc2QuadVertexD g_Zc2QuadVertsGlobal[4];

void Zc2InitMenuCameras();
int __stdcall Zc2InsertHighScore(Zc2ScoreFile *sf);
int __stdcall Zc2CreateMainWindow(HINSTANCE hInst);
INT_PTR CALLBACK Zc2ConfigDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void Zc2InitQuadVertexBuffer();
void __stdcall Zc2ClearTextureSurface(Zc2D3DTex **ppTex);

} // namespace th128
