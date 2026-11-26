#include "CardInf.hpp"
#include "AnmHelpers.hpp"
#include "AnmResource.hpp"
#include "GameErrorContext.hpp"
#include "Supervisor.hpp"

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
