#ifndef TH128_CSOUND_MANAGER_HPP
#define TH128_CSOUND_MANAGER_HPP

#include <windows.h>

struct IDirectSound8View;

struct CSoundManager
{
    IDirectSound8View *ds; // +0x00

    CSoundManager();
    ~CSoundManager();
    HRESULT Initialize(HWND hwnd);
    HRESULT SetPrimaryBufferFormat();
};

struct CWaveFileView
{
    void *hmmio;               // +0x00
    unsigned char pad04[0x04];
    DWORD remainingBytes;      // +0x08
    unsigned char pad0C[0x20];
    DWORD size;                // +0x2C
    unsigned char pad30[0x48];
    int mode;                  // +0x78
    int fromMemory;            // +0x7C
    unsigned char *memBase;    // +0x80
    unsigned char *memCur;     // +0x84
    DWORD memSize;             // +0x88
    HANDLE handle;             // +0x8C
    void *trackInfo;           // +0x90
    const char *filename;      // +0x94
    DWORD savedFilePos;        // +0x98

    CWaveFileView();
    ~CWaveFileView();
    void CloseAndDelete();
    HRESULT OpenFromMemory(unsigned char *data, DWORD dataSize, void *info);
    DWORD GetSize();
    HRESULT ResetFile(bool loop, DWORD seekPos);
    HRESULT Read(unsigned char *dst, DWORD sizeToRead, DWORD *outRead);
    HRESULT Close();
    DWORD GetSavedPos();
    void SaveFilePos();
};

struct BgmTrackInfoView
{
    unsigned char pad00[0x10];
    DWORD dataOffset; // +0x10
    DWORD field14;    // +0x14
    DWORD loopStart;  // +0x18
    DWORD dataSize;   // +0x1C
};

extern DWORD g_BgmBaseOffset;

struct IDirectSoundBufferFullView;

struct CSoundView
{
    void *vtable;                         // +0x00
    IDirectSoundBufferFullView **buffers; // +0x04
    DWORD bufferSize;                     // +0x08
    CWaveFileView *waveFile;              // +0x0C
    DWORD numBuffers;                     // +0x10
    int fadeRemaining;                    // +0x14
    int fadeTotal;                        // +0x18
    int fadeMode;                         // +0x1C
    DWORD field20;                        // +0x20
    DWORD field24;                        // +0x24
    DWORD field28;                        // +0x28
    DWORD field2C;                        // +0x2C
    int isPlayingFlag;                    // +0x30
    int isPausedFlag;                     // +0x34

    HRESULT RestoreBuffer(IDirectSoundBufferFullView *buf, int *wasRestored);
    IDirectSoundBufferFullView *GetFreeBuffer();
    IDirectSoundBufferFullView *GetBuffer0();
    HRESULT Get3DBufferInterface(DWORD index, void **out3d);
    HRESULT SetVolume(int volume);
    HRESULT Stop(int closeFile);
    HRESULT Pause();
    HRESULT Reset();
    int IsSoundPlaying();
    int UpdateFadeOut();
    int UpdateFadeIn();
    int UpdateFadeLow();
    int UpdateFadeHigh();
};

extern int g_BgmVolume;
extern const GUID g_IID_IDirectSound3DBuffer;

void __cdecl WaveDebugLog(const char *fmt, ...);
void __cdecl EclDebugLog(const char *fmt, ...);

#endif
