#include "LaserData.hpp"

void LaserDataInf::Slot00(float, float *)
{
}

void LaserDataInf::Slot01()
{
}

void LaserDataInf::Slot02(int)
{
}

int LaserDataInf::Slot03(int)
{
    return 0;
}

int LaserDataInf::Slot04()
{
    return 0;
}

int LaserDataInf::Slot05()
{
    return 0;
}

int LaserDataInf::Unlink()
{
    prev->next = next;
    if (next != 0)
    {
        next->prev = prev;
    }
    return 0;
}

int LaserDataInf::Slot07(int, int)
{
    return 0;
}

int LaserDataInf::Slot08(int, int, int, int, int)
{
    return 0;
}

int LaserDataInf::Slot09(int, int, int, int)
{
    return 0;
}

int LaserDataInf::Slot10(int, int, int, int, int, int)
{
    return 0;
}

int LaserDataInf::Slot11(int, int, int, int)
{
    return 0;
}

int LaserDataInf::Slot12(int, int)
{
    return 0;
}

int LaserDataInf::Slot13()
{
    return 0;
}

int LaserDataInf::Slot14()
{
    return 0;
}

int LaserDataInf::Slot15()
{
    return 0;
}

int LaserDataInf::Slot16()
{
    return 0;
}

int LaserDataInf::Slot17()
{
    return 0;
}

int LaserDataInf::Slot18()
{
    return 0;
}

int LaserDataInf::Slot19()
{
    return 0;
}

int LaserDataInf::Slot20()
{
    return 0;
}

int LaserDataInf::Slot21()
{
    return 0;
}

int LaserDataInf::Slot22()
{
    return 0;
}

int LaserDataInf::Slot23()
{
    return 0;
}
