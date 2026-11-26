#include "CardInf.hpp"
#include "AnmHelpers.hpp"
#include "AnmResource.hpp"
#include "GameErrorContext.hpp"
#include "Supervisor.hpp"
#include "AsciiInf.hpp"

// Observed partial views; their original owning class names are unresolved.
struct CardAuxiliaryStateView
{
    unsigned char unknown000[0x37F8];
    unsigned int flags;
};

struct CardVisibilityStateView
{
    unsigned char unknown000[0xED0];
    float probe;
};

struct CardTrackedActorView
{
    unsigned char unknown000[0x11FC];
    ZunVec3 position;
};

struct CardTrackedActorManagerView
{
    unsigned char unknown000[0x5C];
    CardTrackedActorView *actor;
};

struct CardGateStateView
{
    unsigned char unknown000[0x3C];
    int active;
};

extern CardAuxiliaryStateView *g_CardAuxiliaryState;             // 0x004B8928
extern CardVisibilityStateView *g_CardVisibilityState;           // 0x004B8A80
extern CardTrackedActorManagerView *g_CardTrackedActorManager;   // 0x004B8948
extern CardGateStateView *g_CardGateState;                       // 0x004B892C

namespace
{
    inline AnmVm *ResolveCardVmId(AnmId *id)
    {
        AnmVm *vm = g_AnmManager->GetVmById(*id);
        if (vm == 0)
            id->id = 0;
        return vm;
    }

    inline AnmVm *FindCardNumberVm(AnmVm *root)
    {
        for (AnmVmListNode *node = &root->childNode; node != 0; node = node->next)
        {
            if (node->vm->anmScriptIndex == 3)
                return node->vm;
        }
        return 0;
    }
}

void CardInf::EnableChains()
{
    CardInf *card = g_CardInf;
    if (card->calcElement != 0)
        card->calcElement->flags |= 2U;
    if (card->drawElement != 0)
        card->drawElement->flags |= 2U;
}

CardInf *CardInf::Create()
{
    CardInf *card = new CardInf;
    if (card->Initialize() != 0)
    {
        if (card != 0)
        {
            card->Shutdown();
            delete card;
        }
        return 0;
    }
    return card;
}

void CardInf::Destroy()
{
    CardInf *card = g_CardInf;
    if (card != 0)
    {
        card->Shutdown();
        delete card;
    }
}

int CardInf::Initialize()
{
    resource = AnmManager::PreloadAnm(37, "card.anm");
    if (resource == 0)
    {
        g_GameErrorContext.Log("\x83\x66\x81\x5B\x83\x5E\x82\xAA\x8C\xA9\x82\xC2\x82\xA9\x82\xE8\x82\xDC\x82\xB9\x82\xF1\x81\x42\x83\x66\x81\x5B\x83\x5E\x82\xAA\x89\xF3\x82\xEA\x82\xC4\x82\xA2\x82\xDC\x82\xB7\x0D\x0A");
        return -1;
    }

    ChainElem *element = new ChainElem;
    element->SetCallback(OnTick);
    element->arg = this;
    element->flags = (element->flags & ~2U) | 1U;
    g_Chain->RegisterCalc(element, 26);
    calcElement = element;

    element = new ChainElem;
    element->SetCallback(OnDraw);
    element->arg = this;
    element->flags = (element->flags & ~2U) | 1U;
    g_Chain->RegisterDraw(element, 10);
    drawElement = element;

    timer.Initialize();
    timer.currentF = 0.0f;
    timer.current = 0;
    timer.previous = -1;
    return 0;
}

void CardInf::Shutdown()
{
    AnmManager *manager = g_AnmManager;
    if (manager->loadedAnms[37] != 0)
    {
        manager->loadedAnms[37]->Release();
        delete manager->loadedAnms[37];
        manager->loadedAnms[37] = 0;
    }
    AnmHelpers::DeleteVmByIdPtr(&vmIds[0]);
    AnmHelpers::DeleteVmByIdPtr(&vmIds[1]);
    AnmHelpers::DeleteVmByIdPtr(&vmIds[2]);

    ChainElem *element = calcElement;
    Chain *chain = g_Chain;
    if (element != 0)
    {
        g_Supervisor.EnterCriticalSection(0);
        chain->CutImpl(element);
        g_Supervisor.LeaveCriticalSection(0);
    }
    element = drawElement;
    chain = g_Chain;
    if (element != 0)
    {
        g_Supervisor.EnterCriticalSection(0);
        chain->CutImpl(element);
        g_Supervisor.LeaveCriticalSection(0);
    }
    g_CardInf = 0;
}

int CardInf::Update()
{
    if ((flags & 1U) == 0)
        return 1;
    ++activeFrameCount;
    if (timer.current >= 60)
        g_CardAuxiliaryState->flags &= ~1U;

    const int frame = timer.current;
    if (frame >= 300 && (flags & 0x10U) == 0)
    {
        bonus = initialBonus - (initialBonus * 3 / 4) * (frame - 300) / (bonusDurationFrames - 300);
        bonus -= bonus % 10;
    }
    else
        bonus = initialBonus;
    timer.Increment();

    if (timer.current >= 120)
    {
        if ((flags & 8U) == 0)
        {
            if (g_CardVisibilityState->probe < 96.0f)
            {
                AnmId *id = vmIds;
                int remaining = 3;
                do
                {
                    g_AnmManager->StopVmById(*id, 3);
                    ++id;
                } while (--remaining);
                flags |= 8U;
            }
        }
        else if (g_CardVisibilityState->probe > 128.0f)
        {
            AnmId *id = vmIds;
            int remaining = 3;
            do
            {
                g_AnmManager->StopVmById(*id, 2);
                ++id;
            } while (--remaining);
            flags &= ~8U;
        }
    }

    const ZunVec3 &position = g_CardTrackedActorManager->actor->position;
    AnmManager *manager = g_AnmManager;
    ZunVec3 delta;
    delta.x = position.x - trackedPosition.x;
    delta.y = position.y - trackedPosition.y;
    delta.z = position.z - trackedPosition.z;
    ZunVec3 step;
    step.x = delta.x * 0.05f;
    step.y = delta.y * 0.05f;
    step.z = delta.z * 0.05f;
    trackedPosition += step;

    AnmVm *vm = manager->GetVmById(positionVmId);
    if (vm != 0)
    {
        vm->position.x = (float)(trackedPosition.x + 128.0 + 192.0);
        vm->position.y = (float)(trackedPosition.y + 16.0);
        vm->position.z = trackedPosition.z;
    }
    if ((flags & 0x40U) != 0 && g_CardGateState->active == 0)
        flags &= ~0x40U;
    return 1;
}

int CardInf::Draw()
{
    if ((flags & 1U) != 0)
    {
        AnmVm *root = ResolveCardVmId(&vmIds[2]);
        if (root == 0)
        {
            goto finish;
        }
        AnmVm *number = FindCardNumberVm(root);
        AsciiInf *ascii = g_AsciiInf;
        ascii->alignmentX = 2;
        ascii->alignmentY = 1;
        ascii->color.channels.alpha = (unsigned char)(number->color >> 24);
        if ((flags & 2U) != 0)
        {
            ZunVec3 position = { 365.0f, 61.0f, 0.0f };
            if ((flags & 0x100U) != 0)
                ascii->color.value = 0xFFFF0000U;
            ascii->CreateTextF(&position, "%8d", bonus);
            if ((flags & 0x100U) != 0)
                g_AsciiInf->color.value = 0xFFFFFFFFU;
        }
        else
        {
            ZunVec3 position = { 387.0f, 61.0f, 0.0f };
            ascii->CreateTextF(&position, "$");
        }
        g_AsciiInf->alignmentX = 0;
        g_AsciiInf->alignmentY = 0;
        g_AsciiInf->color.channels.alpha = 0xFF;
    }
finish:
    flags &= ~0x100U;
    return 1;
}

int __fastcall CardInf::OnTick(void *card)
{
    return static_cast<CardInf *>(card)->Update();
}

int __fastcall CardInf::OnDraw(void *card)
{
    return static_cast<CardInf *>(card)->Draw();
}
