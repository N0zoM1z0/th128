#ifndef TH128_TWO_CALL_BATCH1_HPP
#define TH128_TWO_CALL_BATCH1_HPP

struct Tc1AnmId {
    unsigned int id;
};

struct Tc1AnmVm;

struct Tc1AnmNode {
    Tc1AnmVm *vm;
    Tc1AnmNode *next;
    Tc1AnmNode *prev;
};

struct Tc1AnmFile {
    unsigned char pad00[0x108];
    int f108;
    unsigned char pad10c[0x0c];
    unsigned char *sprites;
    unsigned char pad11c[0x08];
    int f124;
};

struct Tc1AnmVm {
    Tc1AnmId id;            // +0x000
    Tc1AnmNode mgrNode;     // +0x004
    Tc1AnmNode childNode;   // +0x010
    unsigned char pad01c[0x402];
    unsigned short scriptId;// +0x41e
    unsigned char pad420[0x0c];
    Tc1AnmFile *anmFile;    // +0x42c

    int BindAnmFile(Tc1AnmFile *file, int spriteId);
};

struct Tc1AnmMgr {
    unsigned char pad00[0x8b9700];
    Tc1AnmNode list0;       // +0x8b9700 (next at +0x8b9704)
    Tc1AnmNode list1;       // +0x8b970c
};

struct Tc1MenuObj {
    unsigned char pad00[0x2d0];
    Tc1AnmId rootId;        // +0x2d0
    unsigned char pad2d4[0x5748];
    short values[5];        // +0x5a1c..+0x5a24
};

extern Tc1AnmMgr *g_Tc1AnmMgr;

Tc1AnmVm *__stdcall Tc1GetVmById(Tc1AnmMgr *mgr, Tc1AnmId id);

void Tc1UpdateDigitSprites(Tc1MenuObj *obj);

#endif
