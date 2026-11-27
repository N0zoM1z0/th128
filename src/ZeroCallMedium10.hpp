#ifndef TH128_ZERO_CALL_MEDIUM10_HPP
#define TH128_ZERO_CALL_MEDIUM10_HPP

struct Zc10Float2 {
    float x;
    float y;
};

struct Zc10AnmVm {
    unsigned char pad00[0x68];
    float f68;
    float f6c;
    unsigned char pad70[0x84 - 0x70];
    float f84;
    float f88;
    float f8c;
    unsigned char pad90[0x98 - 0x90];
    float f98;
    unsigned char pad9c[0x3f0 - 0x9c];
    unsigned int color;
};

struct Zc10Vertex28 {
    float x;
    float y;
    float z;
    float rhw;
    unsigned int color;
    float u;
    float v;
};

struct Zc10Laser {
    unsigned char pad00[0x4c];
    float posX;
    float posY;
    unsigned char pad58[0x68 - 0x58];
    float angle;
    float length;
    float width;

    virtual int CheckHitA(const Zc10Float2 *pos, float radius);
};

struct Zc10Enemy {
    unsigned char pad00[0x11fc];
    float posX;
    float posY;
    unsigned char pad1204[0x5004 - 0x1204];
    int hp;
    unsigned char pad5008[0x5010 - 0x5008];
    int f5010;
    int f5014;
    unsigned int f5018;
    unsigned char pad501c[0x50ac - 0x501c];
    unsigned int flags;
};

struct Zc10EnemyNode {
    Zc10Enemy *enemy;
    Zc10EnemyNode *next;
};

struct Zc10EnemyMgr {
    unsigned char pad00[0xb8];
    Zc10EnemyNode *head;
};

unsigned char *__stdcall Zc10DecryptBlocks(unsigned char key, unsigned char *data, int size, unsigned char step, int blockSize, int limit);
unsigned char *__stdcall Zc10EncryptBlocks(unsigned char key, unsigned char *data, int size, unsigned char step, int blockSize, int limit);
int Zc10BuildRibbonU(int count, Zc10AnmVm *vm, Zc10Vertex28 *verts);
int Zc10BuildRibbonV(int count, Zc10AnmVm *vm, Zc10Vertex28 *verts);
void __stdcall Zc10KillEnemiesInBox(Zc10EnemyMgr *mgr, const Zc10Float2 *pos, const Zc10Float2 *size);

#endif
