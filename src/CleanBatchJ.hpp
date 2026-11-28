#pragma once

#include <windows.h>

namespace th128 {

struct CbjIceGrid {
    unsigned char pad0[0x65de4];
    unsigned int bits[0x1500];
};

unsigned int CbjCountIceBits(CbjIceGrid* grid);

struct CbjAnmVm {
    unsigned char pad0[0x42c];
    void* anm42c;
    unsigned char pad430[0x4b0 - 0x430];
    unsigned int flags4b0;
};

struct CbjAnmListNode {
    CbjAnmVm* vm;
    CbjAnmListNode* next;
};

struct CbjAnmMgr {
    unsigned char pad0[0x4e90e0];
    void* slots[8];
    unsigned char pad4e9100[0x8b9704 - 0x4e9100];
    CbjAnmListNode* listA;
    int pad8b9708;
    CbjAnmListNode* listB;
};

struct CbjVObj {
    virtual void Slot0() = 0;
    virtual void Slot1() = 0;
    virtual void Slot2() = 0;
    virtual void Slot3() = 0;
    virtual void Slot4() = 0;
    virtual void Slot5(int arg) = 0;
};

struct CbjVObjNode {
    CbjVObj* obj;
    CbjVObjNode* next;
};

struct CbjChainNode {
    int pad0;
    unsigned int flags;
};

struct CbjStageClearObj {
    unsigned char pad0[8];
    CbjChainNode* chain8;
    CbjChainNode* chainC;
    unsigned char pad10[0x4c];
    int field5c;
    unsigned char pad60[0x58];
    CbjVObjNode* listB8;
    int padBC[2];
    int fieldC4;

    void Clear();
};

struct CbjScoreState {
    unsigned char pad0[0xa8];
    int valA8;
};

struct CbjJoyPropHeader {
    DWORD dwSize;
    DWORD dwHeaderSize;
    DWORD dwObj;
    DWORD dwHow;
};

struct CbjJoyPropRange {
    CbjJoyPropHeader diph;
    LONG lMin;
    LONG lMax;
};

struct CbjJoyObjInst {
    unsigned char pad0[0x18];
    DWORD dwType;
};

struct CbjJoyDevice {
    virtual HRESULT __stdcall QueryInterface(void*, void**) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetCapabilities(void*) = 0;
    virtual HRESULT __stdcall EnumObjects(void*, void*, DWORD) = 0;
    virtual HRESULT __stdcall GetProperty(const GUID*, void*) = 0;
    virtual HRESULT __stdcall SetProperty(const GUID*, const CbjJoyPropHeader*) = 0;
};

struct CbjVertex {
    int v[7];
};

struct CbjMeshAnm {
    unsigned char pad0[0x4ac];
    CbjVertex* dst;
};

struct CbjMesh {
    int rows;
    int cols;
    int pad8;
    CbjMeshAnm** anms;
    CbjVertex* src;

    void CopyVerts();
};

extern CbjAnmMgr* g_CbjAnmMgr;
extern CbjScoreState* g_CbjScoreState;
extern int g_CbjBombVal;
extern int g_CbjBombLocked;
extern int g_CbjLifeVal;
extern void* g_CbjGui;
extern int g_CbjJoyIdx;
extern CbjJoyDevice* g_CbjJoyDevices[16];

void CbjPlaySfx(int id);
void CbjShowPopup(void* gui, int type, int subtype, int arg);
void CbjOnLifeUp(void* gui);

void __stdcall CbjDecodeScoreVal(int* outHigh, int* outLow);
void __stdcall CbjAddBombPiece(int delta);
void __stdcall CbjAddLifePiece(int delta);
BOOL __stdcall CbjEnumJoyAxisCb(const CbjJoyObjInst* obj, void* ctx);
unsigned int CbjChecksumDwords(const unsigned int* data, int size);

} // namespace th128
