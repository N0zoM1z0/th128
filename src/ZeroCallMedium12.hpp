#ifndef TH128_ZERO_CALL_MEDIUM12_HPP
#define TH128_ZERO_CALL_MEDIUM12_HPP

struct Zc12Float3 {
    float x;
    float y;
    float z;
};

struct Zc12DatEntry {
    int f00;
    int f04;
    const char *name;
    int f0c;
};

struct Zc12PopupSlot {
    unsigned char digits[12];
    Zc12Float3 pos;
    int color;
    int timerPrev;
    int timerCur;
    float timerCurF;
    float *timerScale;
    unsigned int timerFlags;
    unsigned char pad30[8];
    unsigned char active;
    unsigned char digitCount;
    unsigned char pad3a[6];
};

struct Zc12PopupMgr {
    unsigned char pad00[0x14];
    int cursor;
    unsigned char pad18[0x500 - 0x18];
    Zc12PopupSlot slots[10];
};

struct Zc12MeterVm {
    unsigned char pad00[0x3f8];
    short val;
    unsigned char pad3fa[0x4c8 - 0x3fa];
    void (__fastcall *cb)(Zc12MeterVm *self, int val);
    unsigned char pad4cc[0x4e8 - 0x4cc];
};

struct Zc12GuiMeters {
    unsigned char pad00[0x10];
    Zc12MeterVm metersB[8];
    Zc12MeterVm metersA[8];
};

struct Zc12BulletMove {
    unsigned char pad00[0x9d8];
    Zc12Float3 pos;
    unsigned char pad9e4[0xa20 - 0x9e4];
    unsigned int flags;
    unsigned char pada24[0xfe4 - 0xa24];
    int timerPrev;
    int timerCur;
    float timerCurF;
    float *timerScale;
    unsigned int timerFlags;
    unsigned char padff8[0x100c - 0xff8];
    Zc12Float3 vel;
    int limit;
};

struct Zc12LaserAction {
    int pad00[4];
    int param10;
    int pad14[3];
    int type;
    int cond24;
    int pad28;
};

struct Zc12LaserObj {
    int pad00[2];
    int state0C;
    unsigned char pad10[0x594 - 0x10];
    int actionIdx;
    int flag598;
    unsigned char pad59c[0x5b4 - 0x59c];
    int val5B4;
    unsigned char pad5b8[0x61c - 0x5b8];
    Zc12LaserAction actions[18];
    unsigned char pad934[0xde8 - (0x61c + 18 * 0x2c)];
    unsigned int flagsDE8;

    virtual void RunActions();
};

struct Zc12WaveReader {
    int pad00[2];
    unsigned int remaining;
    unsigned char pad0c[0x7c - 0x0c];
    int isMemory;
    unsigned int memSize;
    unsigned char *memCur;
    unsigned char *memBase;
    void *fileHandle;
};

struct Zc12SfxInfo {
    int pad00[2];
    short baseVol;
    short pad0a;
    unsigned int playFlags;
};

struct Zc12DsBufVtbl {
    void *pad[12];
    int (__stdcall *Play)(void *self, unsigned int r1, unsigned int r2, unsigned int flags);
    int (__stdcall *SetCurrentPosition)(void *self, unsigned int pos);
    void *pad2[1];
    int (__stdcall *SetVolume)(void *self, int vol);
    int (__stdcall *SetPan)(void *self, int pan);
    void *pad3;
    int (__stdcall *Stop)(void *self);
};

struct Zc12DsBuf {
    Zc12DsBufVtbl *vtbl;
};

struct Zc12SfxChan {
    Zc12DsBuf *buf;
    int pad04;
    Zc12SfxInfo *info;
    int pad0c;
    int pan;
};

extern int g_Zc12DatCount;
extern Zc12DatEntry g_Zc12DatTable[32];
extern Zc12PopupMgr *g_Zc12PopupMgr;
extern float g_Zc12TimeScale;
extern int g_Zc12SfxVol;
extern int g_Zc12TexFmt;
extern int g_Zc12TexPitch;
extern unsigned char *g_Zc12TexPixels;

Zc12DatEntry *Zc12FindDatEntry(const char *path);
void __stdcall Zc12SpawnScorePopup(const Zc12Float3 *pos, int score, int color);
void __stdcall Zc12SetMeterAnmsA(Zc12GuiMeters *gui, int full, int sub);
void __stdcall Zc12SetMeterAnmsB(Zc12GuiMeters *gui, int full, int sub);
int Zc12StepBulletMove(Zc12BulletMove *b);
void Zc12PlaySfxChannel(Zc12SfxChan *chan, int pan);
int __stdcall Zc12ReadWaveData(Zc12WaveReader *r, unsigned int size, unsigned char *dst, unsigned int *outRead);
bool __stdcall Zc12InvertTexAlpha(int rows);

#endif
