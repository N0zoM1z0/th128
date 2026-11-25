#pragma once

// Menu selection cursor. Field names are provisional.
struct MenuCursor
{
    int value;                  // +0x00
    int previous;               // +0x04
    int count;                  // +0x08
    int savedValues[16];        // +0x0C
    int savedCounts[16];        // +0x4C
    int stackDepth;             // +0x8C
    int disabled[16];           // +0x90
    int wraps;                  // +0xD0
    int disabledCount;          // +0xD4

    void Push();
    void Pop();
    int Move(int delta);
};
