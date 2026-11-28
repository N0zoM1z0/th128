#pragma once

#include <windows.h>
#include <string>

namespace th128 {

struct CbmBgHeader {
    short count;
};

struct CbmBgObj {
    unsigned char pad0[3];
    unsigned char flag3;
};

struct CbmBgQuad {
    short type;
    short size2;
    short scriptId4;
    short vmIdx6;
};

struct CbmBgVm {
    unsigned char pad0[0x424];
    int field424;
    unsigned char pad428[0x4e8 - 0x428];

    void Update();
};

struct CbmStageBg {
    unsigned char pad0[0x10];
    CbmBgHeader* header10;
    CbmBgObj** objs14;
    unsigned char pad18[0x1c8 - 0x18];
    CbmBgVm* vms1c8;

    int UpdateQuads();
};

struct CbmSeMgr {
    int dummy;
    void PlaySe(int id);
};

struct CbmListEntry {
    void* owner;
    CbmListEntry* next;
    CbmListEntry* prev;
};

struct CbmLaserNode {
    unsigned char pad0[0x1518];
    CbmLaserNode* self1518;
    int sub151c;
    unsigned char pad1520[0x18a0 - 0x1520];
    int* p18a0;
    CbmListEntry entry; // +0x18a4 (size 12 -> total 0x18b0)
};

struct CbmLaserMgr {
    unsigned char pad0[0x64];
    CbmListEntry lists[16];
    unsigned char pad124[0x134 - 0x124];
    int count134;

    void SpawnNode(int listIdx);
};

struct CbmVert28 {
    int d[7];
};

struct CbmStripRow {
    unsigned char pad0[0x4ac];
    CbmVert28* verts4ac;
};

struct CbmStrip {
    int rows;
    int cols;
    int pad8;
    CbmStripRow** rowObjsC;
    CbmVert28* srcVerts10;

    void CopyStripVerts();
};

extern int g_CbmPower;
extern int g_CbmPowerFlag;
extern void* g_CbmGui;
extern CbmSeMgr g_CbmSeMgr;

void CbmShowPopup(void* gui, int a, int b, int c);

void __stdcall CbmAddPower(int delta);
unsigned int CbmSumU32(const unsigned int* data, int byteLen);
std::string& CbmStringErase(std::string* s, size_t pos, size_t count);

} // namespace th128
