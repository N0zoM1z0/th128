#include "CardInfActions.hpp"
#include "CardActivityViews.hpp"
#include "AnmHelpers.hpp"
#include "AnmText.hpp"
#include "PopupNotifications.hpp"
#include "PostPresentUpdates.hpp"
#include "SoundManager.hpp"
#include "Supervisor.hpp"

#include <string.h>

namespace
{
    inline int ReplayRecordMode()
    {
        return *reinterpret_cast<int *>(static_cast<unsigned char *>(g_ReplayPauseStorage) + 0x10);
    }

    inline int CardGateActive()
    {
        return *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(g_CardGateState) + 0x3C);
    }

    inline unsigned int &AuxiliaryFlags()
    {
        return *reinterpret_cast<unsigned int *>(
            reinterpret_cast<unsigned char *>(g_CardAuxiliaryState) + 0x37F8);
    }

    inline CardHistoryRecord *HistoryAt(void *storage, int index)
    {
        return reinterpret_cast<CardHistoryRecord *>(
            static_cast<unsigned char *>(storage) + 0x37A0) + index;
    }

    inline AnmVm *ResolveCardVm(AnmManager *manager, AnmId *id)
    {
        AnmVm *vm = manager->GetVmById(*id);
        if (vm == 0)
            id->id = 0;
        return vm;
    }

    inline AnmVm *ResolveCardVm(AnmId *id)
    {
        return ResolveCardVm(g_AnmManager, id);
    }

    inline AnmVm *FindCardChild(AnmVm *vm, int scriptIndex)
    {
        for (AnmVmListNode *node = &vm->childNode; node != 0; node = node->next)
        {
            if (node->vm->anmScriptIndex == scriptIndex)
                return node->vm;
        }
        return 0;
    }
}

void __stdcall CardInfActions::Start(int cardIndex, const char *name, int durationFrames)
{
    CardInf *card = g_CardInf;
    card->timer.Initialize();
    card->timer.currentF = 0.0f;
    card->timer.current = 0;
    card->timer.previous = -1;
    card->unknown078 = cardIndex;
    strcpy(card->name, name);
    card->flags = (card->flags & ~0x30U) | 7U;
    if (ReplayRecordMode() != 1)
    {
        CardHistoryRecord *history = CardHistory(cardIndex);
        strcpy(reinterpret_cast<char *>(history->nameArea), name);
        if (history->counter08C < 99999)
            ++history->counter08C;
    }
    card->flags &= ~0x40U;
    if (CardGateActive() != 0)
        card->flags |= 0x40U;
    card->flags &= ~0x80U;
    card->activeFrameCount = 1;
    card->vmIds[0] = CardAsciiResource()->CreateVm(0, 1, 23, 0);
    AnmResource *supervisorResource = *reinterpret_cast<AnmResource **>(
        reinterpret_cast<unsigned char *>(&g_Supervisor) + 0x590);
    card->vmIds[1] = supervisorResource->CreateVm(0, 0x4A, 23, 0);
    card->vmIds[2] = CardAsciiResource()->CreateVm(0, 2, 23, 0);

    void *historyStorage = g_CardHistoryStorage;
    if (HistoryAt(historyStorage, card->unknown078)->counter084 > 0)
    {
        AnmVm *child = FindCardChild(ResolveCardVm(&card->vmIds[2]), 4);
        if (child->resource != 0)
            child->BindAnmFile(child->resource, 0x135);
    }
    if (HistoryAt(historyStorage, card->unknown078)->counter080 > 0)
    {
        AnmVm *child = FindCardChild(ResolveCardVm(&card->vmIds[2]), 5);
        if (child->resource != 0)
            child->BindAnmFile(child->resource, 0x136);
    }
    if (HistoryAt(historyStorage, card->unknown078)->counter088 > 0)
    {
        AnmVm *child = FindCardChild(ResolveCardVm(&card->vmIds[2]), 6);
        if (child->resource != 0)
            child->BindAnmFile(child->resource, 0x137);
    }
    reinterpret_cast<AnmTextVmView *>(ResolveCardVm(&card->vmIds[1]))->DrawCenteredText(
        0x00FFFFFF, 0, 0, 0, name);
    g_SoundManager.PlaySfx(0x22);
    card->positionVmId = g_CardEffectResources->resource->CreateVm(0, 0x9E, 23, 0);
    AnmManager *manager = g_AnmManager;
    card->trackedPosition = CardActorResources()->actor->position;
    AnmVm *vm = manager->GetVmById(card->positionVmId);
    if (vm != 0)
    {
        vm->position.x = (float)(card->trackedPosition.x + 128.0 + 192.0);
        vm->position.y = (float)(card->trackedPosition.y + 16.0);
        vm->position.z = card->trackedPosition.z;
    }
    FindCardChild(ResolveCardVm(manager, &card->positionVmId), 0x9C)->integerVariables[2] = durationFrames;
    FindCardChild(ResolveCardVm(manager, &card->positionVmId), 0x9D)->integerVariables[2] = durationFrames;
    card->bonusDurationFrames = durationFrames;

    switch (g_CurrentReplayStage)
    {
    case 0: case 1: case 6: case 11:
        card->bonus = card->initialBonus = (g_Difficulty + 1) * 100000;
        break;
    case 2: case 4: case 7: case 9: case 12: case 14:
        card->bonus = card->initialBonus = (g_Difficulty + 2) * 100000;
        break;
    case 3: case 5: case 8: case 10: case 13: case 15:
        card->bonus = card->initialBonus = (g_Difficulty + 3) * 100000;
        break;
    case 16:
        card->bonus = card->initialBonus = 500000;
        break;
    }
    if (card->initialBonus >= 100000000)
        card->initialBonus = 99999999;
    g_CardEffectResources->resource->CreateVm(0, 0xA8, 23, 0);
    if (g_CurrentReplayStage != 16)
        card->auxiliaryVmId = card->resource->CreateVm(0, 0, 23, 0);
    else
        card->auxiliaryVmId = CardActorResources()->resource->CreateVm(0, 0x12, 23, 0);

    switch (g_CurrentReplayStage)
    {
    case 1: case 9: case 14:
        CardActorResources()->resource->CreateVm(0, 0x0B, 23, 0);
        break;
    case 2: case 7: case 11:
        CardActorResources()->resource->CreateVm(0, 0x0B, 23, 0);
        break;
    case 3: case 5: case 8: case 10: case 13: case 15:
        if ((cardIndex >= 20 && cardIndex <= 23) ||
            (unsigned int)cardIndex - 216U <= 3)
            CardActorResources()->resource->CreateVm(0, 0x24, 23, 0);
        else if ((cardIndex >= 100 && cardIndex <= 103) ||
            (unsigned int)cardIndex - 180U <= 3)
            CardActorResources()->resource->CreateVm(0, 0x25, 23, 0);
        else if ((cardIndex >= 56 && cardIndex <= 59) ||
            (unsigned int)cardIndex - 136U <= 3)
            CardActorResources()->resource->CreateVm(0, 0x26, 23, 0);
        else
            CardActorResources()->resource->CreateVm(0, 0x27, 23, 0);
        break;
    case 4: case 6: case 12:
        CardActorResources()->resource->CreateVm(0, 0x0B, 23, 0);
        return;
    case 16:
        CardActorResources()->resource->CreateVm(0, 0x11, 23, 0);
        break;
    }
}

void CardInfActions::Finish()
{
    CardInf *card = g_CardInf;
    if ((card->flags & 1U) == 0)
        return;
    AuxiliaryFlags() |= 1U;
    g_AnmManager->StopVmById(card->vmIds[0], 1);
    g_AnmManager->StopVmById(card->vmIds[1], 1);
    g_AnmManager->StopVmById(card->vmIds[2], 1);
    card->flags &= ~1U;
    AnmHelpers::DeleteVmByIdPtr(&card->auxiliaryVmId);
    card->flags &= ~0x40U;
    AnmHelpers::DeleteVmByIdPtr(&card->positionVmId);
    CardHistoryRecord *history = CardHistory(card->unknown078);
    if (history->counter088 < 99999)
        ++history->counter088;
    if ((card->flags & 2U) != 0)
    {
        if (ReplayRecordMode() != 1)
        {
            history = CardHistory(card->unknown078);
            if (history->counter080 < 99999)
                ++history->counter080;
        }
        g_SoundManager.PlaySfx(0x30);
        if ((card->flags & 4U) != 0)
        {
            g_PopupNotifications->Show(7, card->bonus);
            history = CardHistory(card->unknown078);
            if (history->counter084 < 99999)
                ++history->counter084;
        }
        else
            g_PopupNotifications->Show(8, card->bonus);
        g_Score += card->bonus / 10;
        if (g_Score >= 1000000000)
            g_Score = 999999999;
    }
    else
        g_PopupNotifications->Show(9, 0);
}

void __stdcall CardInfActions::AddScore(int amount)
{
    g_Score += amount / 10;
    if (g_Score >= 1000000000)
        g_Score = 999999999;
}
