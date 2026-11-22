#pragma once

// One node of the SptInf data list.
struct SptNode
{
    void *data;
    SptNode *next;
};

// RTTI SptInf, vtable 0x004A26E8: five virtual defaults, then the virtual
// destructor. Slot names are by index until their meaning is reviewed.
class SptInf
{
public:
    virtual int Slot00();
    virtual int Slot01(int argument);
    virtual int Slot02(int argument);
    virtual float Slot03(int argument);
    virtual int Slot04(int argument);
    virtual ~SptInf();

    unsigned char unknown004[0x11B4 - 0x004];
    SptNode *dataList;      // +0x11B4
};
