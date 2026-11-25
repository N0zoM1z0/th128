#include "SoundManager.hpp"

#include <stdlib.h>
#include <string.h>

#include "Supervisor.hpp"

// Minimal COM vtable interface for IDirectSoundBuffer matching DirectSound8
// vtable slot indices (+0x08 Release, +0x24 GetStatus, +0x40 SetPan, +0x48 Stop).
struct IDirectSoundBuffer
{
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                // +0x04
    virtual ULONG __stdcall Release() = 0;                               // +0x08
    virtual HRESULT __stdcall Slot03() = 0;                              // +0x0C
    virtual HRESULT __stdcall Slot04() = 0;                              // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                              // +0x14
    virtual HRESULT __stdcall Slot06() = 0;                              // +0x18
    virtual HRESULT __stdcall Slot07() = 0;                              // +0x1C
    virtual HRESULT __stdcall Slot08() = 0;                              // +0x20
    virtual HRESULT __stdcall GetStatus(LPDWORD status) = 0;             // +0x24
    virtual HRESULT __stdcall Slot10() = 0;                              // +0x28
    virtual HRESULT __stdcall Slot11() = 0;                              // +0x2C
    virtual HRESULT __stdcall Slot12() = 0;                              // +0x30
    virtual HRESULT __stdcall Slot13() = 0;                              // +0x34
    virtual HRESULT __stdcall Slot14() = 0;                              // +0x38
    virtual HRESULT __stdcall Slot15() = 0;                              // +0x3C
    virtual HRESULT __stdcall SetPan(LONG pan) = 0;                      // +0x40
    virtual HRESULT __stdcall Slot17() = 0;                              // +0x44
    virtual HRESULT __stdcall Stop() = 0;                                // +0x48
};

void __cdecl SoundDebugLog(const char *, ...)
{
}

void SfxChannel::Init(int index)
{
    priority = -1;
    SfxDesc *entry = g_SfxTable;
    while (entry != NULL)
    {
        if (entry->id == index)
        {
            break;
        }
        ++entry;
    }
    desc = entry;
    id = index;
}

void SfxChannel::SetPriority(int p)
{
    priority = p;
}

int SfxChannel::GetId()
{
    return id;
}

IDirectSoundBuffer *SfxChannel::GetBuffer()
{
    return buffer;
}

void SfxChannel::ReleaseBuffer()
{
    if (buffer != NULL)
    {
        buffer->Release();
        buffer = NULL;
    }
}

void SfxChannel::SetPanDirect(int pan)
{
    buffer->SetPan(pan);
}

void BgmStreamView::SetFadeOut(float seconds)
{
    fadeMode = 1;
    int frames = (int)(seconds * 60.0);
    fadeRemaining = frames;
    fadeTotal = frames;
}

int BgmStreamView::HasField10()
{
    return field10 != 0;
}

SoundManager::SoundManager()
{
    memset(this, 0, sizeof(SoundManager));
    for (int i = 0; i < 76; ++i)
    {
        channels[i].Init(i);
    }
}

void SoundManager::PlaySfx(int id)
{
    int priority = g_SfxTable[id].priority;
    int slot = 0;
    for (; slot < 12; ++slot)
    {
        if (queuedIds[slot] < 0)
        {
            break;
        }
        if (queuedIds[slot] == id)
        {
            if (queuedCounts[slot] < 60)
            {
                int count = queuedCounts[slot];
                if (count >= 0)
                {
                    queuedPans[slot][count] = 0;
                    ++queuedCounts[slot];
                }
            }
            return;
        }
    }
    if (slot < 12)
    {
        queuedIds[slot] = id;
        queuedPans[slot][0] = 0;
        ++queuedCounts[slot];
        channels[id].priority = priority;
    }
}

void SoundManager::PlaySfxWithPan(int id, float x)
{
    int priority = g_SfxTable[id].priority;
    int pan = (int)((x * 1000.0) / 192.0);
    int slot = 0;
    for (; slot < 12; ++slot)
    {
        if (queuedIds[slot] < 0)
        {
            break;
        }
        if (queuedIds[slot] == id)
        {
            if (queuedCounts[slot] < 60)
            {
                int count = queuedCounts[slot];
                if (count >= 0)
                {
                    queuedPans[slot][count] = pan;
                    ++queuedCounts[slot];
                }
            }
            return;
        }
    }
    if (slot < 12)
    {
        queuedIds[slot] = id;
        queuedPans[slot][0] = pan;
        ++queuedCounts[slot];
        channels[id].priority = priority;
    }
}

void SoundManager::SetSfxPan(int id, float x)
{
    channels[id].SetPanDirect((int)((x * 1000.0) / 192.0));
}

void SoundManager::StopSfx(int id)
{
    if (id < 0)
    {
        for (int i = 0; i < 76; ++i)
        {
            channels[i].wasPlaying = 0;
            if (channels[i].buffer != NULL)
            {
                DWORD status;
                channels[i].buffer->GetStatus(&status);
                channels[i].wasPlaying = (status & 1) != 0;
                channels[i].buffer->Stop();
            }
        }
        return;
    }
    int slot = 0;
    for (; slot < 12; ++slot)
    {
        if (queuedIds[slot] < 0)
        {
            break;
        }
        if (queuedIds[slot] == id)
        {
            queuedCounts[slot] = -1;
            return;
        }
    }
    if (slot < 12)
    {
        queuedIds[slot] = id;
        queuedCounts[slot] = -1;
    }
}

void SoundManager::EnqueueBgmCommand(int type, int param, const char *filename)
{
    g_Supervisor.EnterCriticalSection(11);
    for (int i = 0; i < 31; ++i)
    {
        if (bgmQueue[i].type == 0)
        {
            bgmQueue[i].type = type;
            bgmQueue[i].param = param;
            strcpy(bgmQueue[i].filename, filename);
            bgmQueue[i].state = 0;
            break;
        }
    }
    g_Supervisor.LeaveCriticalSection(11);
}

void SoundManager::SetBgmFadeOut(float seconds)
{
    if (bgmStream != NULL)
    {
        bgmStream->SetFadeOut(seconds);
    }
}

void SoundManager::FreeWaveBuffer18C0(int index)
{
    if (waveBuffers18C0[index] != NULL)
    {
        free(waveBuffers18C0[index]);
        waveBuffers18C0[index] = NULL;
    }
}
