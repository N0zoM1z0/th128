#pragma once

namespace th128 {

struct EgfVec3 {
    float x;
    float y;
    float z;

    float AngleToPlayer() const;
    float DistanceTo(const EgfVec3 *other) const;
};

struct EgfRng {
    unsigned int GetU32();
    float GetFloat();
    float GetFloatSigned();
};

struct EgfPlayer {
    unsigned char pad00[0x74];
    int field74;
    unsigned char pad78[0xECC - 0x78];
    EgfVec3 pos; // +0xECC
};

struct EgfEnemy;

struct EgfEnemyManager {
    unsigned char pad00[0x10];
    int field10;
    int field14;
    int field18;
    unsigned char pad1C[0x10];
    int intVars[4];    // +0x2C..+0x38
    float floatVars[8]; // +0x3C..+0x58
    EgfEnemy *boss;    // +0x5C
    unsigned char pad60[0x60];
    int fieldC0;       // +0xC0
    int fieldC4;
    int fieldC8;       // +0xC8 (200)
};

struct EgfSpellCard {
    int GetSomething();
};

struct EgfEnemy {
    unsigned char pad00[0x11C0];
    unsigned int field11C0; // +0x11C0
    unsigned int field11C4; // +0x11C4
    unsigned char pad11C8[0x11FC - 0x11C8];
    EgfVec3 pos;            // +0x11FC
    EgfVec3 vel;            // +0x1208
    unsigned char pad1214[0x1230 - 0x1214];
    EgfVec3 pos2;           // +0x1230
    unsigned char pad123C[0x1248 - 0x123C];
    float angle1;           // +0x1248
    float speed1;           // +0x124C
    float field1250;        // +0x1250
    unsigned char pad1254[0x1264 - 0x1254];
    EgfVec3 pos3;           // +0x1264
    unsigned char pad1270[0x127C - 0x1270];
    float angle2;           // +0x127C
    float speed2;           // +0x1280
    float field1284;        // +0x1284
    unsigned char pad1288[0x1404 - 0x1288];
    int intRegs[4];         // +0x1404..+0x1410
    float floatRegs[8];     // +0x1414..+0x1430
    unsigned char pad1434[8];
    float field143C;        // +0x143C
    unsigned char pad1440[0x5004 - 0x1440];
    int field5004;          // +0x5004
    unsigned char pad5008[0x50AC - 0x5008];
    unsigned int flags;     // +0x50AC

    float GetFloatVar(int varId);
};

extern EgfRng g_EgfRng;
extern EgfPlayer *g_EgfPlayer;
extern EgfEnemyManager *g_EgfEnemyManager;
extern EgfSpellCard g_EgfSpellCard;
extern int g_EgfDifficulty;
extern int g_EgfField4B4CF4;
extern int g_EgfField4B4CF8;
extern int g_EgfField4B4D30;
extern int g_EgfField4D33CC;

} // namespace th128
