#pragma once

#include "CardInf.hpp"
#include "AsciiInf.hpp"
#include "AnmResource.hpp"

// Partial resource views observed by the CardInf entry points. These do not
// establish the original owning classes or their complete allocation sizes.
struct CardAsciiResourceView
{
    unsigned char unknown000[0x1C894];
    AnmResource *resource;
};

struct CardEffectResourceView
{
    unsigned char unknown000[0x8A7F9C];
    AnmResource *resource;
};

struct CardActorResourceView
{
    unsigned char unknown000[0x5C];
    struct CardActorPositionView *actor;
    unsigned char unknown060[0x2C];
    AnmResource *resource;              // +0x8C
};

struct CardActorPositionView
{
    unsigned char unknown000[0x11FC];
    ZunVec3 position;
};

struct CardHistoryRecord
{
    unsigned char nameArea[0x80];       // Name starts here; its original capacity is unresolved.
    int counter080, counter084, counter088, counter08C;
    unsigned char unknown090[8];
};

typedef char CardHistoryRecordSize[(sizeof(CardHistoryRecord) == 0x98) ? 1 : -1];

struct CardAuxiliaryStateView;
struct CardTrackedActorManagerView;
struct CardGateStateView;
extern CardAuxiliaryStateView *g_CardAuxiliaryState;           // 0x004B8928
extern CardTrackedActorManagerView *g_CardTrackedActorManager; // 0x004B8948
extern CardGateStateView *g_CardGateState;                     // 0x004B892C
extern CardEffectResourceView *g_CardEffectResources;          // 0x004B8930
extern void *g_CardHistoryStorage;                            // 0x004B8A88
extern int g_Score;                                          // 0x004B4CC4
extern int g_Difficulty;                                     // 0x004B4D0C

inline AnmResource *CardAsciiResource()
{
    return reinterpret_cast<CardAsciiResourceView *>(g_AsciiInf)->resource;
}

inline CardActorResourceView *CardActorResources()
{
    return reinterpret_cast<CardActorResourceView *>(g_CardTrackedActorManager);
}

inline CardHistoryRecord *CardHistory(int index)
{
    return reinterpret_cast<CardHistoryRecord *>(
        static_cast<unsigned char *>(g_CardHistoryStorage) + 0x37A0) + index;
}
