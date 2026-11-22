#pragma once

#include "ZunMath.hpp"

// Base class of the laser kinds (RTTI LaserDataInf, vtable 0x004A2BB4). The
// 24 virtual slots are named by index until their meaning is reviewed; the
// stack argument counts come from the base implementations' RET sizes.
// One timed laser command (0x2C bytes).
struct LaserCommand
{
    unsigned char unknown00[0x10];
    int value;              // +0x10
    unsigned char unknown14[0x20 - 0x14];
    int type;               // +0x20, 0 ends the list
    int immediate;          // +0x24, 0 waits while the laser is busy
    unsigned char unknown28[0x2C - 0x28];
};

class LaserDataInf
{
public:
    virtual void Slot00(float distance, float *position);
    virtual void Slot01();
    virtual void Slot02(int argument);
    virtual int Slot03(int argument);
    virtual int Slot04();
    virtual int Slot05();
    virtual int Unlink();
    virtual int Slot07(int first, int second);
    virtual int Slot08(int first, int second, int third, int fourth, int fifth);
    virtual int Slot09(int first, int second, int third, int fourth);
    virtual int Slot10(int first, int second, int third, int fourth, int fifth, int sixth);
    virtual int Slot11(int first, int second, int third, int fourth);
    virtual int Slot12(int first, int second);
    virtual int Slot13();
    virtual int Slot14();
    virtual int Slot15();
    virtual int Slot16();
    virtual int Slot17();
    virtual int Slot18();
    virtual int Slot19();
    virtual int Slot20();
    virtual int Slot21();
    virtual int Slot22();
    virtual int Slot23();

    LaserDataInf *prev;     // +0x04
    LaserDataInf *next;     // +0x08
    int state;              // +0x0C
    unsigned char unknown10[0x50 - 0x10];
    ZunVec3 position;       // +0x50
    unsigned char unknown5C[0x68 - 0x5C];
    float angle;            // +0x68
    float length;           // +0x6C
    float width;            // +0x70
    unsigned char unknown74[0x594 - 0x74];
    int commandIndex;       // +0x594
    int busy;               // +0x598
    unsigned char unknown59C[0x5B4 - 0x59C];
    int commandValue80;     // +0x5B4
    unsigned char unknown5B8[0x61C - 0x5B8];
    LaserCommand commands[18];  // +0x61C
    unsigned char unknown934[0xDE8 - 0x934];
    unsigned int flagBits0 : 5; // +0xDE8
    unsigned int mode : 3;
    unsigned int flagBits8 : 24;
};
