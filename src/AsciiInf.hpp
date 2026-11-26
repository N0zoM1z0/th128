#pragma once

#include <stddef.h>
#include "ZunMath.hpp"
#include "ZunTimer.hpp"

struct AsciiTextId
{
    unsigned int id;
};

union AsciiColor
{
    unsigned int value;
    struct
    {
        unsigned char blue, green, red, alpha;
    } channels;
};

struct AsciiTextEntry
{
    char text[0x100];
    AsciiTextId id;                   // +0x100
    ZunVec3 position;                 // +0x104
    AsciiColor color;                // +0x110
    AsciiColor initialColor;         // +0x114
    unsigned int backgroundColor;   // +0x118
    float scaleX;                    // +0x11C
    float scaleY;                    // +0x120
    unsigned int unknown124;
    int style;                       // +0x128
    int alignmentX;                  // +0x12C
    int alternateMode;               // +0x130
    int alignmentY;                  // +0x134
    int unknown138, unknown13C, unknown140;
    int font;                        // +0x144
    int unknown148, unknown14C;
    ZunTimer timer;                  // +0x150
};

// Partial layout of the text queue at the original global 0x004B8920.
struct AsciiInf
{
    unsigned char unknown000[0x9E4];
    AsciiTextEntry entries[320];      // +0x9E4, checked capacity 320
    unsigned char unknown1C6E4[0x164];
    int textCount;                    // +0x1C848
    AsciiColor color;                // +0x1C84C
    unsigned int backgroundColor;
    int font;
    float scaleX, scaleY;
    int style;
    unsigned int unknown1C864;
    int alternateMode;
    int alignmentX, alignmentY;
    int unknown1C874, unknown1C878, unknown1C87C;
    int unknown1C880, unknown1C884;
    unsigned int nextTextId;          // +0x1C888

    AsciiTextId CreateText(const ZunVec3 *position, const char *text);
    AsciiTextId __cdecl CreateTextF(const ZunVec3 *position, const char *format, ...);
};

typedef char AsciiTextEntrySize[(sizeof(AsciiTextEntry) == 0x164) ? 1 : -1];
typedef char AsciiTextQueueOffset[(offsetof(AsciiInf, entries) == 0x9E4) ? 1 : -1];
typedef char AsciiTextCountOffset[(offsetof(AsciiInf, textCount) == 0x1C848) ? 1 : -1];
typedef char AsciiColorOffset[(offsetof(AsciiInf, color) == 0x1C84C) ? 1 : -1];
typedef char AsciiNextIdOffset[(offsetof(AsciiInf, nextTextId) == 0x1C888) ? 1 : -1];

extern AsciiInf *g_AsciiInf;           // 0x004B8920
