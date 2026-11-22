#include "Spt.hpp"

#include <new>

int SptInf::Slot00()
{
    return 0;
}

int SptInf::Slot01(int)
{
    return 0;
}

int SptInf::Slot02(int)
{
    return 0;
}

float SptInf::Slot03(int)
{
    return 0.0f;
}

int SptInf::Slot04(int)
{
    return 0;
}

SptInf::~SptInf()
{
    SptNode *node = dataList;
    while (node != NULL)
    {
        SptNode *next = node->next;
        operator delete(node->data);
        delete node;
        node = next;
    }
}
