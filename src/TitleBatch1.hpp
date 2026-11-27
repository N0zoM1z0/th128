#ifndef TH128_TITLE_BATCH1_HPP
#define TH128_TITLE_BATCH1_HPP

struct Tb1AnmId {
    unsigned int id;
};

struct Tb1AnmVm;

struct Tb1AnmNode {
    Tb1AnmVm *vm;
    Tb1AnmNode *next;
    Tb1AnmNode *prev;
};

struct Tb1AnmFile {
    unsigned char pad00[0x108];
    int f108;
    unsigned char pad10c[0x0c];
    unsigned char *sprites;
    unsigned char pad11c[0x08];
    int f124;
};

struct Tb1AnmVm {
    Tb1AnmId id;            // +0x000
    Tb1AnmNode mgrNode;     // +0x004
    Tb1AnmNode childNode;   // +0x010
    unsigned char pad01c[0x402];
    unsigned short scriptId;// +0x41e
    unsigned char pad420[0x0c];
    Tb1AnmFile *anmFile;    // +0x42c
    unsigned char pad430[0x80];
    unsigned int flags;     // +0x4b0

    int BindAnmFile(Tb1AnmFile *file, int spriteId);
};

struct Tb1AnmMgr {
    unsigned char pad00[0x8b9700];
    Tb1AnmNode list0;       // +0x8b9700 (next at +0x8b9704)
    Tb1AnmNode list1;       // +0x8b970c
};

struct Tb1SoundMgr {
    unsigned char pad00[0x21ac];
    struct Slot {
        int cmdId;
        int arg;
        int f08;
        char name[0x100];
    } slots[32];

    void PostCommand(const char *cmd, int cmdId, int arg);
};

struct Tb1MenuObj {
    unsigned char pad00[0x2cc];
    Tb1AnmId rootId;        // +0x2cc
};

struct Tb1Config {
    unsigned char pad00[0x28];
    signed char bgmVol;
    signed char seVol;
    unsigned char pad2a[0x16];
};

extern Tb1AnmMgr *g_Tb1AnmMgr;
extern Tb1SoundMgr g_Tb1SoundMgr;
extern Tb1Config g_Tb1Cfg;
extern int g_Tb1BgmVol;
extern int g_Tb1SeVol;
extern int g_Tb1SeDb;

Tb1AnmVm *__stdcall Tb1GetVmById(Tb1AnmMgr *mgr, Tb1AnmId id);
void Tb1UpdateOptionsVolume(Tb1MenuObj *obj);

#endif
