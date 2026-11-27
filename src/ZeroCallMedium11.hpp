#ifndef TH128_ZERO_CALL_MEDIUM11_HPP
#define TH128_ZERO_CALL_MEDIUM11_HPP

struct Zc11Float3 {
    float x;
    float y;
    float z;
};

struct Zc11IceField {
    unsigned char pad00[0x65de4];
    unsigned int bitmap[448 * 12];
};

struct Zc11EffectSlot {
    float f00;
    float f04;
    unsigned char pad08[0x10];
    Zc11Float3 pos;
    unsigned char pad24[0x4c - 0x24];
    int timerPrev;
    int timerCur;
    float timerCurF;
    float *timerScale;
    unsigned int timerFlags;
    int f60;
    int f64;
    int f68;
    int f6c;
    int f70;
    unsigned int flags;
};

struct Zc11EffectMgr {
    unsigned char pad00[0x8edc];
    Zc11EffectSlot slots[2000];
    unsigned char pad43a5c[0x43a8c - (0x8edc + 2000 * 0x78)];
    int cursor;

    Zc11EffectSlot *AllocSlot(const Zc11Float3 *pos, float a, float b, int startTime, int param60, int param70);
};

struct Zc11DebugBuf {
    unsigned char pad00[0x9a0];
    int len;
    const char *text;
};

struct Zc11BulletObj {
    unsigned char pad00[0xa4];
    Zc11Float3 fA4;
    Zc11Float3 fB0;
    Zc11Float3 fBC;
    Zc11Float3 fC8;
    int timerPrev;
    int timerCur;
    float timerCurF;
    float *timerScale;
    unsigned int timerFlags;
    int fE8;
    int fEC;
};

struct Zc11SurfaceDesc {
    unsigned int Format;
    unsigned int pad[7];
};

struct Zc11LockedRect {
    int Pitch;
    void *pBits;
};

struct Zc11Rect {
    int left;
    int top;
    int right;
    int bottom;
};

struct Zc11SurfaceVtbl {
    void *pad[12];
    int (__stdcall *GetDesc)(void *self, Zc11SurfaceDesc *desc);
    int (__stdcall *LockRect)(void *self, Zc11LockedRect *locked, const Zc11Rect *rc, unsigned int flags);
    int (__stdcall *UnlockRect)(void *self);
};

struct Zc11Surface {
    Zc11SurfaceVtbl *vtbl;
};

struct Zc11ImgBuf {
    unsigned char pad00[0x100];
    unsigned int format;
    int width;
    int height;
    int pad10c;
    int pitch;
    unsigned char pad114[8];
    int valid;
    unsigned char *pixels;
};

struct Zc11SoundEntry {
    char name[0x34];
};

struct Zc11SoundTable {
    unsigned char pad00[0x1984];
    Zc11SoundEntry *entries;
};

struct Zc11AnmVm {
    unsigned char pad00[0x42c];
    int anmId;
    unsigned char pad430[0x4b0 - 0x430];
    unsigned int flags;
};

struct Zc11AnmNode {
    Zc11AnmVm *vm;
    Zc11AnmNode *next;
};

struct Zc11AnmMgr {
    unsigned char pad00[0x4e90e0];
    int ids[8];
    unsigned char pad4e9100[0x8b9704 - 0x4e9100];
    Zc11AnmNode *list1;
    int pad8b9708;
    Zc11AnmNode *list2;
};

struct Zc11VirtObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5(int arg);
};

struct Zc11VirtNode {
    Zc11VirtObj *obj;
    Zc11VirtNode *next;
};

struct Zc11FlagNode {
    int f00;
    unsigned int flags;
};

struct Zc11StageObj {
    int pad00[2];
    Zc11FlagNode *node1;
    Zc11FlagNode *node2;
    unsigned char pad10[0x5c - 0x10];
    int f5C;
    unsigned char pad60[0xb8 - 0x60];
    Zc11VirtNode *head;
    unsigned char padBC[8];
    int fC4;
};

extern float g_Zc11TimeScale;
extern Zc11Float3 g_Zc11CamPos;
extern Zc11AnmMgr *g_Zc11AnmMgr;

int Zc11PopcountIceBitmap(const Zc11IceField *field);
int __stdcall Zc11CheckDebugKey(Zc11DebugBuf *buf, const char *key, int v1, int v2);
void __stdcall Zc11InitBulletTimer(Zc11BulletObj *obj, const Zc11Float3 *v1, const Zc11Float3 *v2, int paramE8, unsigned char paramEC);
bool Zc11BlitToSurface(Zc11ImgBuf *img, Zc11Surface *surf);
bool Zc11CopyFromSurface(Zc11ImgBuf *img, Zc11Surface *surf);
int __stdcall Zc11FindSoundByName(const char *path, Zc11SoundTable *tbl);
void __stdcall Zc11CleanupStageAnms(Zc11StageObj *obj);

#endif
