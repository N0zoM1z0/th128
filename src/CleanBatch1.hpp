#ifndef TH128_CLEAN_BATCH1_HPP
#define TH128_CLEAN_BATCH1_HPP

namespace th128 {

struct Cb1Vec3 {
    float x;
    float y;
    float z;

    Cb1Vec3() {}
    Cb1Vec3(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

    inline Cb1Vec3 operator+(const Cb1Vec3& rhs) const {
        return Cb1Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    inline Cb1Vec3& operator+=(const Cb1Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    void SetPolarXY(float angle, float radius);
};

struct Cb1Timer {
    int prev;      // +0x00
    int cur;       // +0x04
    float curF;    // +0x08
    void* ctrl;    // +0x0c
    unsigned int flags; // +0x10

    inline void Reset();
};

struct Cb1PhaseEntry {
    int hpThreshold; // +0x00
    int timeLimit;   // +0x04
    int hpSub;       // +0x08
    int timeSub;     // +0x0c
};

struct Cb1Enemy {
    unsigned char pad0000[0x1434];
    Cb1Timer timer;             // +0x1434
    unsigned char pad1448[0x3bbc];
    int hp;                     // +0x5004
    int f5008;                  // +0x5008
    int hpDelta;                // +0x500c
    int f5010;                  // +0x5010
    int hpFloor;                // +0x5014
    unsigned char pad5018[0x94];
    unsigned int flags50ac;     // +0x50ac
    unsigned char pad50b0[0x14];
    Cb1PhaseEntry phases[8];    // +0x50c4

    int UpdatePhases();
};

struct Cb1Gui {
    unsigned char pad0000[0x5a24];
    int timerSec; // +0x5a24
    int timerCs;  // +0x5a28
};

struct Cb1SpellCard {
    unsigned char pad00[0x28];
    int timer28;          // +0x28
    unsigned char pad2c[0x50];
    unsigned int flags7c; // +0x7c
    int bonus80;          // +0x80
};

struct Cb1Player {
    unsigned char pad00[0x3c];
    int f3c; // +0x3c
};

struct Cb1Boss {
    unsigned char pad00[0x18];
    int f18; // +0x18
};

struct Cb1RingVert {
    Cb1Vec3 pos;        // +0x00
    float rhw;          // +0x0c
    unsigned int color; // +0x10
    float u;            // +0x14
    float v;            // +0x18
};

struct Cb1RingMesh {
    Cb1RingVert verts[33];  // +0x000..+0x39b (33 * 0x1c = 0x39c)
    unsigned int pad39c;    // +0x39c
    float radii[33];        // +0x3a0..+0x423
    float radiusVel[32];    // +0x424..+0x4a3
    float uvStep;           // +0x4a4
};

struct Cb1AnmVm {
    unsigned char pad000[0x3f0];
    unsigned int color;     // +0x3f0
    unsigned char pad3f4[0x64];
    Cb1Vec3 pos1;           // +0x458
    Cb1Vec3 pos2;           // +0x464
    unsigned char pad470[0x3c];
    Cb1RingMesh* ringMesh;  // +0x4ac

    int UpdateRingMesh();
};

struct Cb1InputState {
    unsigned int curr;            // +0x000
    unsigned int prev;            // +0x004
    unsigned int repeatMask;      // +0x008
    unsigned int pressed;         // +0x00c
    unsigned int released;        // +0x010
    unsigned int repeatCount[32]; // +0x014..+0x093
    unsigned int pad094[32];      // +0x094..+0x113
    unsigned int holdFrames[32];  // +0x114..+0x193
    unsigned int pad194[38];      // +0x194..+0x22b
    unsigned int heldMask;        // +0x22c
    unsigned int pad230[6];       // +0x230..+0x247

    void Update();
};

extern int g_Cb1TimerCtrl;
extern Cb1Gui* g_Cb1Gui;
extern Cb1SpellCard* g_Cb1Spell;
extern Cb1Player* g_Cb1Player;
extern Cb1Boss* g_Cb1Boss;

struct Cb1Vec2 {
    float x;
    float y;
};

int __stdcall Cb1SegmentsIntersect(Cb1Vec2 p1, Cb1Vec2 p2, Cb1Vec2 p3, Cb1Vec2 p4);

} // namespace th128

#endif
