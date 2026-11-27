#pragma once

#include "AsciiInf.hpp"

namespace th128 {

struct AsReplayHeader {
    char name[12];          // +0x00
    __int64 timestamp;      // +0x0c
    int score;              // +0x14
    unsigned char pad18[0x54 - 0x18];
    float slowRate;         // +0x54
    unsigned char pad58[0x64 - 0x58];
    int route;              // +0x64
    int difficulty;         // +0x68
    int lastDigit;          // +0x6c
};

struct AsReplaySlot {
    unsigned char pad00[0x1c];
    AsReplayHeader *header; // +0x1c
};

struct AsReplayMgr {
    unsigned char pad00[0x1c];
    AsReplayHeader *curHeader;
};

struct AsReplayMenu {
    unsigned char pad00[0x24];
    int state;              // +0x24
    int cursor;             // +0x28
    unsigned char pad2c[0x2b8 - 0x2c];
    int slideTimer;         // +0x2b8
    float slideTimerF;      // +0x2bc
    unsigned char pad2c0[0x592c - 0x2c0];
    char nameBuf[12];       // +0x592c
    int nameLen;            // +0x5938
    unsigned char pad593c[0x5944 - 0x593c];
    int kbCursor;           // +0x5944
    unsigned char pad5948[0x5a2c - 0x5948];
    int selectedSlot;       // +0x5a2c
    unsigned int pad5a30;
    AsReplaySlot *slots[25];// +0x5a34
};

struct AsScoreEntry {
    int score;              // +0x00
    char stage;             // +0x04
    char cont;              // +0x05
    char name[10];          // +0x06
    __int64 timestamp;      // +0x10
    float slowRate;         // +0x18
    int pad1c;              // +0x1c
};

struct AsRouteScore {
    unsigned char pad00[0x10];
    AsScoreEntry entries[5][10]; // +0x10 .. +0x650
    unsigned int pad650[2];
    int clearCount;         // +0x658
    int playCounts[5];      // +0x65c..+0x66c? Wait: 0x66c - 0xc = 0x660!
};

struct AsRouteScoreFixed {
    unsigned char pad00[0x10];
    AsScoreEntry entries[5][10]; // +0x10..+0x64f
    int clearCount;         // +0x650
    int playCounts[5];      // +0x654..+0x667
    int pad668;
};

struct AsScoreData {
    unsigned char pad00[8];
    AsRouteScoreFixed routes[7]; // +0x08, stride 0x66c -> +0x2cfc
    unsigned char pad_after[0x33b0 - (8 + 7 * 0x66c)];
    __int64 totalPlayTime;  // +0x33b0
};

struct AsScoreMenu {
    unsigned char pad00[0x24];
    int state;              // +0x24
    int cursor;             // +0x28
    unsigned char pad2c[0x100 - 0x2c];
    int diffCursor;         // +0x100
    unsigned char pad104[0x1d8 - 0x104];
    int hideScores;         // +0x1d8
    unsigned char pad1dc[0x592c - 0x1dc];
    char nameBuf[12];       // +0x592c
    int nameLen;            // +0x5938
    int isViewingOnly;      // +0x593c
    int pad5940;
    int kbCursor;           // +0x5944
};

struct AsFreezeHud {
    unsigned char pad00[0x6b1ec];
    int animTimer;          // +0x6b1ec
    float animTimerF;       // +0x6b1f0
    unsigned char pad6b1f4[8];
    unsigned int flags;     // +0x6b1fc
    unsigned int pad6b200;
    int bonusVal;           // +0x6b204
    float percent;          // +0x6b208
    int fadeTimer;          // +0x6b20c
};

struct AsNameEntry {
    unsigned char pad00[0x110];
    int kbCursor;           // +0x110
    unsigned char pad114[0x1f0 - 0x114];
    int nameLen;            // +0x1f0
    unsigned char pad1f4[0x2cc - 0x1f4];
    char nameBuf[16];       // +0x2cc
};

extern AsReplayMgr *g_AsReplayMgr;
extern AsScoreData *g_AsScoreData;
extern int g_AsDifficulty;
extern int g_AsRoute;
extern const char *g_AsRouteNames[];
extern const char *g_AsDiffNames[];
extern const char *g_AsStageNames[];
extern const char g_AsKeyboardChars[];

int __stdcall AsDrawReplayMenu(AsReplayMenu *menu);
int __stdcall AsDrawHighScoreMenu(AsScoreMenu *menu);
int __stdcall AsDrawPlayerDataMenu(AsScoreMenu *menu);
int __stdcall AsDrawFreezeHud(AsFreezeHud *hud);
void __stdcall AsDrawNameEntry(AsNameEntry *entry, ZunVec3 pos);

} // namespace th128
