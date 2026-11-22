#pragma once

// Menu selection cursor. Field names are provisional.
struct MenuCursor
{
    int value;                  // +0x00
    int previous;               // +0x04
    int count;                  // +0x08
    unsigned char unknown0C[0x90 - 0x0C];
    int disabled[16];           // +0x90
    int wraps;                  // +0xD0
    int disabledCount;          // +0xD4

    int Move(int delta);
};
