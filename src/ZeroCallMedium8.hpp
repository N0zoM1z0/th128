#pragma once

#include <d3d9.h>

namespace th128 {

struct Zc8SpriteDesc {
    unsigned char pad00[0x1c];
    float texH;             // +0x1c
    float texW;             // +0x20
    float u0;               // +0x24
    float v0;               // +0x28
    float u1;               // +0x2c
    float v1;               // +0x30
    float h;                // +0x34
    float w;                // +0x38
    float uvScaleX;         // +0x3c
    float uvScaleY;         // +0x40
    unsigned char pad44[0x48 - 0x44]; // total 0x48 (72 = 9*8) bytes
};

struct Zc8AnmLoaded {
    unsigned char pad00[0x108];
    void *data108;          // +0x108
    unsigned char pad10c[0x118 - 0x10c];
    Zc8SpriteDesc *sprites; // +0x118
    unsigned char pad11c[0x124 - 0x11c];
    int flag124;            // +0x124
};

struct Zc8AnmVm {
    unsigned char pad00[0x60];
    float spriteW;          // +0x60
    float spriteH;          // +0x64
    unsigned char pad68[0x84 - 0x68];
    float uv84;             // +0x84
    float uv88;             // +0x88
    float uv8c;             // +0x8c
    float uv90;             // +0x90
    float uv94;             // +0x94
    float uv98;             // +0x98
    float uv9c;             // +0x9c
    float uvA0;             // +0xa0
    unsigned char padA4[0x330 - 0xa4];
    D3DMATRIX mat1;         // +0x330..+0x36f
    D3DMATRIX mat2;         // +0x370..+0x3af
    D3DMATRIX mat3;         // +0x3b0..+0x3ef
    unsigned char pad3f0[0x418 - 0x3f0];
    short spriteId;         // +0x418
    unsigned char pad41a[0x428 - 0x41a];
    Zc8SpriteDesc *sprite;  // +0x428
};

struct Zc8LineVert {
    float x;
    float y;
    float z;
    float rhw;
    unsigned int color;
};

struct Zc8Vec2 {
    float x;
    float y;
};

struct Zc8AnmMgrLine {
    unsigned char pad00[0xac];
    int drawCount;          // +0xac
    unsigned char padB0[0x4e968e - 0xb0];
    unsigned char state8e;  // +0x4e968e
    unsigned char pad8f[4];
    unsigned char state93;  // +0x4e9693
    unsigned char pad94[0x8b96fc - 0x4e9694];
    Zc8LineVert *vertCursor;// +0x8b96fc
};

extern unsigned char g_Zc8LzssRing[0x2000];
extern Zc8AnmMgrLine *g_Zc8AnmMgr;
extern IDirect3DDevice9 *g_Zc8D3DDevice;

int Zc8AnmSetSprite(Zc8AnmVm *vm, Zc8AnmLoaded *loaded, int spriteId);
unsigned char * __stdcall Zc8LzssDecompress(
    const unsigned char *src, int srcLen, unsigned char *dst, int dstLen);
int Zc8AnmDrawLineStrip(int count, const Zc8Vec2 *pts, const unsigned int *colors);

} // namespace th128
