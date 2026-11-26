#include "HelpInf.hpp"
#include "AnmManager.hpp"
#include "AnmHelpers.hpp"
#include "AnmD3DX.hpp"
#include "Supervisor.hpp"
#include "SoundManager.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"
#include "Input.hpp"
#include "UpdateTimer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HelpInf *g_HelpInf = 0;

namespace
{
    inline void ResetHelpTimer(ZunTimer &timer)
    {
        timer.Initialize();
        timer.currentF = 0.0f;
        timer.current = 0;
        timer.previous = -1;
    }
}

HelpInf *HelpInf::Create()
{
    HelpInf *help = new HelpInf;
    if (Initialize(help) != 0)
    {
        if (help != 0)
        {
            help->Shutdown();
            delete help;
        }
        return 0;
    }
    return help;
}

void HelpInf::Destroy()
{
    HelpInf *help = g_HelpInf;
    if (help != 0)
    {
        help->Shutdown();
        delete help;
    }
}

int __stdcall HelpInf::Initialize(HelpInf *help)
{
    ChainElem *element = new ChainElem;
    element->SetCallback(OnTick);
    element->arg = help;
    element->flags = (element->flags & ~2U) | 1U;
    g_Chain->RegisterCalc(element, 10);
    help->calcElement = element;

    element = new ChainElem;
    element->SetCallback(OnDraw);
    element->arg = help;
    element->flags = (element->flags & ~2U) | 1U;
    g_Chain->RegisterDraw(element, 64);
    help->drawElement = element;

    QueueHelpWorker(LoadResourcesWorker);
    ResetHelpTimer(help->timer);
    help->state = 0;
    return 0;
}

void HelpInf::Shutdown()
{
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
    AnmManager *manager = g_AnmManager;
    if (manager->loadedAnms[31] != 0)
    {
        manager->loadedAnms[31]->Release();
        delete manager->loadedAnms[31];
        manager->loadedAnms[31] = 0;
    }
    g_HelpInf = 0;
}

AnmId AnmResource::CreateUiVm(const ZunVec3 *position, int scriptIndex)
{
    g_Supervisor.EnterCriticalSection(9);
    ++activeVmCount;
    AnmVm *vm = AllocateAnmVm();
    vm->stateFlags |= 2U;
    if (position == 0)
    {
        ZunVec3 zero = { 0.0f, 0.0f, 0.0f };
        vm->position = zero;
    }
    else
    {
        memcpy(&vm->position, position, sizeof(vm->position));
    }
    BindVmScript(vm, scriptIndex);
    unsigned int value = vm->RegisterUiVmAtTail().id;
    vm->stateFlags &= ~0xC0U;
    AnmId id = { value };
    g_Supervisor.LeaveCriticalSection(9);
    return id;
}

void HelpInf::CreateVm(int scriptIndex, const ZunVec3 *position)
{
    vmIds[scriptIndex] = resource->CreateUiVm(position, scriptIndex);
}

void __cdecl HelpInf::LoadResourcesWorker()
{
    g_HelpInf->resource = AnmManager::PreloadAnm(31, "help.anm");
    if (g_HelpInf->resource == 0)
    {
        g_GameErrorContext.Fatal("\x89\xe6\x96\xca\x8d\x5c\x90\xac\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8c\xa9\x82\xc2\x82\xa9\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
        return;
    }
    g_Supervisor.thread.running = 0;
    g_Supervisor.thread.stopRequested = 1;
    g_HelpInf->calcElement->flags |= 2U;
    g_HelpInf->drawElement->flags |= 2U;
}

void __cdecl HelpInf::LoadPageWorker()
{
    g_HelpInf->pageData = FileSystem::OpenPath(
        g_HelpInf->pageFilename, &g_HelpInf->pageSize, 0);
    g_HelpInf->pageState = 3;
    g_Supervisor.thread.running = 0;
    g_Supervisor.thread.stopRequested = 1;
}

int __stdcall HelpInf::Update(HelpInf *help)
{
    ZunVec3 position = { help->originX, 0.0f, 0.0f };
    switch (help->state)
    {
    case 0:
        help->state = 1;
        break;
    case 1:
        switch (help->pageState)
        {
        case 0:
            help->cursor.count = 9;
            if (help->cursor.count == 0)
                help->cursor.value = 0;
            else if (help->cursor.count < 1)
                help->cursor.value = help->cursor.count - 1;
            else
                help->cursor.value = 0;
            help->cursor.wraps = 1;
            for (int i = 0; i < 9; ++i)
            {
                help->vmIds[i] = help->resource->CreateUiVm(&position, i);
                g_AnmManager->SetInterruptAndUpdate(help->vmIds[i],
                    static_cast<short>(help->cursor.value == i ? 2 : 3));
            }
            AnmHelpers::ClearD3DTexture(reinterpret_cast<AnmD3DTextureSurfaceView **>(
                &help->resource->textures[1].texture));
            help->pageState = 1;
            // The newly created directory uses the same input gate this tick.
        case 1:
            if (help->timer.current < 20)
                break;
            help->cursor.previous = help->cursor.value;
            if ((g_InputStates[0].triggerMask & 0x10) != 0 ||
                (g_InputStates[0].repeatMask & 0x10) != 0)
                help->cursor.Move(-1);
            if ((g_InputStates[0].triggerMask & 0x20) != 0 ||
                (g_InputStates[0].repeatMask & 0x20) != 0)
                help->cursor.Move(1);
            if (help->cursor.previous != help->cursor.value)
            {
                g_SoundManager.PlaySfx(10);
                for (int i = 0; i < 9; ++i)
                    g_AnmManager->SetInterruptAndUpdate(help->vmIds[i],
                        static_cast<short>(help->cursor.value == i ? 2 : 3));
            }
            if ((g_InputStates[0].triggerMask & 0x80201) != 0)
                goto BeginPageLoad;
            if ((g_InputStates[0].triggerMask & 0x102) != 0)
            {
                g_SoundManager.PlaySfx(9);
                for (int i = 0; i < 9; ++i)
                    g_AnmManager->StopVmById(help->vmIds[i], 1);
                help->state = 2;
                help->pageState = 0;
                ResetHelpTimer(help->timer);
            }
            break;
        case 3:
            help->resource->textures[1].UploadTextureImage(help->pageData, help->pageSize);
            if (help->pageData != 0)
            {
                free(help->pageData);
                help->pageData = 0;
            }
            help->pageData = 0;
            reinterpret_cast<IDirect3DTexture9 *>(
                help->resource->textures[1].texture)->PreLoad();
            help->CreateVm(9, &position);
            help->pageState = 4;
            SetUpdateTimerCurrent(&help->timer, 0);
            // The page starts with the same twenty-frame input gate.
        case 4:
            if (help->timer.current < 20)
                break;
            if ((g_InputStates[0].triggerMask & 0x20) != 0 && help->cursor.value < 8)
            {
                g_SoundManager.PlaySfx(10);
                help->cursor.Move(1);
                g_AnmManager->StopVmById(help->vmIds[9], 1);
                goto BeginPageLoad;
            }
            if ((g_InputStates[0].triggerMask & 0x10) != 0 && help->cursor.value > 0)
            {
                g_SoundManager.PlaySfx(10);
                help->cursor.Move(-1);
                g_AnmManager->StopVmById(help->vmIds[9], 1);
                goto BeginPageLoad;
            }
            if ((g_InputStates[0].triggerMask & 0x80303) != 0)
            {
                g_SoundManager.PlaySfx(9);
                help->pageState = 1;
                SetUpdateTimerCurrent(&help->timer, 0);
                g_AnmManager->StopVmById(help->vmIds[9], 1);
                for (int i = 0; i < 9; ++i)
                {
                    help->vmIds[i] = help->resource->CreateUiVm(&position, i);
                    g_AnmManager->SetInterruptAndUpdate(help->vmIds[i],
                        static_cast<short>(help->cursor.value == i ? 2 : 3));
                }
            }
            break;
        }
        break;
    case 2:
        if (help->timer.current >= 30)
            help->exitRequested = 1;
        break;
    }
    goto AdvanceTimer;

BeginPageLoad:
    g_SoundManager.PlaySfx(7);
    help->pageState = 2;
    ResetHelpTimer(help->timer);
    sprintf(help->pageFilename, "help_%.2d.png", help->cursor.value);
    QueueHelpWorker(HelpInf::LoadPageWorker);
    for (int i = 0; i < 9; ++i)
        g_AnmManager->StopVmById(help->vmIds[i], 1);

AdvanceTimer:
    int nextCurrent = help->timer.current;
    float *const speed = help->timer.gameSpeed;
    help->timer.previous = nextCurrent;
    if (*speed > 0.99 && *speed < 1.01f)
    {
        float nextCurrentF = help->timer.currentF;
        ++nextCurrent;
        nextCurrentF += 1.0f;
        help->timer.current = nextCurrent;
        help->timer.currentF = nextCurrentF;
    }
    else
    {
        const float nextCurrentF = *speed + help->timer.currentF;
        help->timer.currentF = nextCurrentF;
        help->timer.current = static_cast<int>(nextCurrentF);
    }
    return 1;
}

int __fastcall HelpInf::OnTick(void *help)
{
    return Update(static_cast<HelpInf *>(help));
}

int __fastcall HelpInf::OnDraw(void *help)
{
    return static_cast<HelpInf *>(help)->Draw();
}
