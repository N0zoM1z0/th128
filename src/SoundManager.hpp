#ifndef TH128_SOUND_MANAGER_HPP
#define TH128_SOUND_MANAGER_HPP

#include <windows.h>

struct IDirectSoundBuffer;

struct SfxDesc
{
    int id;             // +0x00
    const char *name;   // +0x04
    short field08;      // +0x08
    short priority;     // +0x0A
    int field0C;        // +0x0C
    int field10;        // +0x10
};

struct SfxChannel
{
    IDirectSoundBuffer *buffer; // +0x00
    int priority;               // +0x04
    SfxDesc *desc;              // +0x08
    int id;                     // +0x0C
    int field10;                // +0x10
    int wasPlaying;             // +0x14

    void Init(int index);
    void ReleaseBuffer();
    void SetPanDirect(int pan);
    void SetPriority(int p);
    int GetId();
    IDirectSoundBuffer *GetBuffer();
};

struct BgmCommand
{
    int type;            // +0x00
    int param;           // +0x04
    int state;           // +0x08
    char filename[0x100]; // +0x0C
};

struct BgmStreamView
{
    void *vtable;        // +0x00
    void *field04;       // +0x04
    void *field08;       // +0x08
    void *waveFile;      // +0x0C
    int field10;         // +0x10
    int fadeRemaining;   // +0x14
    int fadeTotal;       // +0x18
    int fadeMode;        // +0x1C

    void SetFadeOut(float seconds);
    int HasField10();
};

struct SoundManager
{
    void *directSound;              // +0x0000 (0x4D5A68)
    unsigned char pad0004[0x0C];
    int initialized;                // +0x0010 (0x4D5A78)
    DWORD bgmThreadId;              // +0x0014 (0x4D5A7C)
    HANDLE bgmThreadHandle;         // +0x0018 (0x4D5A80)
    int field001C;                  // +0x001C
    int queuedIds[12];              // +0x0020 (0x4D5A88)
    int queuedCounts[12];           // +0x0050 (0x4D5AB8)
    int queuedPans[12][128];        // +0x0080 (0x4D5AE8)
    unsigned char pad1880[0x40];
    void *waveBuffers18C0[42];      // +0x18C0 (0x4D7328)
    unsigned char pad1968[0x20];
    SfxChannel channels[76];        // +0x1988 (0x4D73F0)
    void *waveBuffers[65];          // +0x20A8 (0x4D7B10)
    BgmCommand bgmQueue[32];        // +0x21AC (0x4D7C14)
    unsigned char pad432C[0x1100];
    BgmStreamView *bgmStream;       // +0x542C (0x4DAE94)
    HANDLE bgmNotifyEvent;          // +0x5430 (0x4DAE98)
    unsigned char pad5434[0x2C];

    SoundManager();

    void PlaySfx(int id);
    void PlaySfxWithPan(int id, float x);
    void SetSfxPan(int id, float x);
    void StopSfx(int id);
    void EnqueueBgmCommand(int type, int param, const char *filename);
    void SetBgmFadeOut(float seconds);
    void FreeWaveBuffer18C0(int index);
};

extern SoundManager g_SoundManager;
extern SfxDesc g_SfxTable[];

void __cdecl SoundDebugLog(const char *fmt, ...);

#endif
