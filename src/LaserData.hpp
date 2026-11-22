#pragma once

// Base class of the laser kinds (RTTI LaserDataInf, vtable 0x004A2BB4). The
// 24 virtual slots are named by index until their meaning is reviewed; the
// stack argument counts come from the base implementations' RET sizes.
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

    LaserDataInf *prev;
    LaserDataInf *next;
};
