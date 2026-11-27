#ifndef TH128_ANM_MANAGER_CTOR_HPP
#define TH128_ANM_MANAGER_CTOR_HPP

#include "Chain.hpp"

struct AmcVertex {
    float x, y, z, rhw;
    float u, v;
};

struct AmcColorVertex {
    float x, y, z, rhw;
    unsigned int color;
    float u, v;
};

struct AmcAnmVm {
    unsigned char data[0x4e8];

    AmcAnmVm();
    ~AmcAnmVm();
    void Reset();
};

struct AmcD3dDeviceVtbl {
    void *pad[0x170 / 4];
    int (__stdcall *SetSamplerOrState)(void *self, int arg);
};

struct AmcD3dDevice {
    AmcD3dDeviceVtbl *vtbl;
};

struct AmcAnmManager {
    int slot00;                         // +0x00
    unsigned char pad04[0x24];
    int slot28;                         // +0x28
    unsigned char pad2c[0x24];
    int slot50;                         // +0x50
    unsigned char pad54[0x24];
    int slot78;                         // +0x78
    unsigned char pad7c[0x40];
    AmcAnmVm vms[0x1000];               // +0xbc (size 0x4e8000 -> +0x4e80bc)
    unsigned char pad4e80bc[0x10dc];    // -> +0x4e9198
    AmcAnmVm singleVm;                  // +0x4e9198 (size 0x4e8 -> +0x4e9680)
    int pad4e9680;                      // +0x4e9680
    int field4e9684;                    // +0x4e9684
    int field4e9688;                    // +0x4e9688
    unsigned char field4e968c;          // +0x4e968c
    unsigned char field4e968d;          // +0x4e968d
    unsigned char field4e968e;          // +0x4e968e
    unsigned char field4e968f;          // +0x4e968f
    unsigned char field4e9690;          // +0x4e9690
    unsigned char pad4e9691[7];
    int field4e9698;                    // +0x4e9698
    unsigned char pad4e969c[0x3d0078];  // -> +0x8b9714
    AmcAnmVm extraVms[0x21];            // +0x8b9714 (size 0xa1e8 -> +0x8c38fc)
    unsigned char pad8c38fc[0x0c];      // -> +0x8c3908

    AmcAnmManager();
};

typedef char AmcSizeCheck[(sizeof(AmcAnmManager) == 0x8c3908) ? 1 : -1];

extern AmcVertex g_AmcQuadVerts[4];
extern AmcColorVertex g_AmcColorQuadVerts[4];
extern AmcD3dDevice *g_AmcD3dDevice;

int __fastcall AmcCalc0(void *);
int __fastcall AmcCalc1(void *);
int __fastcall AmcDraw0(void *);
int __fastcall AmcDraw1(void *);
int __fastcall AmcDraw2(void *);
int __fastcall AmcDraw3(void *);
int __fastcall AmcDraw4(void *);
int __fastcall AmcDraw5(void *);
int __fastcall AmcDraw6(void *);
int __fastcall AmcDraw7(void *);
int __fastcall AmcDraw8(void *);
int __fastcall AmcDraw9(void *);
int __fastcall AmcDraw10(void *);
int __fastcall AmcDraw11(void *);
int __fastcall AmcDraw12(void *);
int __fastcall AmcDraw13(void *);
int __fastcall AmcDraw14(void *);
int __fastcall AmcDraw15(void *);
int __fastcall AmcDraw16(void *);
int __fastcall AmcDraw17(void *);
int __fastcall AmcDraw18(void *);
int __fastcall AmcDraw19(void *);
int __fastcall AmcDraw20(void *);
int __fastcall AmcDraw21(void *);
int __fastcall AmcDraw22(void *);
int __fastcall AmcDraw23(void *);
int __fastcall AmcDraw24(void *);
int __fastcall AmcDraw25(void *);
int __fastcall AmcDraw26(void *);
int __fastcall AmcDraw27(void *);
int __fastcall AmcDraw28(void *);

#endif
