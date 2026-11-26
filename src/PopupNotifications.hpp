#pragma once

#include <stddef.h>
#include "AnmManager.hpp"

struct AnmResource;

// Observed view of the object rooted at 0x004B8950; original class unresolved.
struct PopupNotifications
{
    unsigned char unknown000[0x5860];
    AnmId digits[8];                   // +0x5860
    AnmId millionSeparator;            // +0x5880
    AnmId thousandSeparator;           // +0x5884
    AnmId primaryLabel;                // +0x5888
    AnmId secondaryLabel;              // +0x588C
    unsigned char unknown5890[0x44];
    AnmId marker;                      // +0x58D4
    int markerActive;                  // +0x58D8
    unsigned char unknown58DC[0x158];
    AnmResource *resource;             // +0x5A34

    void Show(int kind, int value);
};

typedef char PopupDigitsOffset[(offsetof(PopupNotifications, digits) == 0x5860) ? 1 : -1];
typedef char PopupResourceOffset[(offsetof(PopupNotifications, resource) == 0x5A34) ? 1 : -1];

extern PopupNotifications *g_PopupNotifications; // 0x004B8950
