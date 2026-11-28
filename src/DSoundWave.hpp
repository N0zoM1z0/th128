#pragma once

#include <windows.h>
#include <mmsystem.h>
#include <stddef.h>

namespace th128 {

struct DswPositionNotify {
    DWORD dwOffset;
    HANDLE hEventNotify;
};

struct IDswNotify {
    virtual HRESULT __stdcall QueryInterface(const GUID &iid, void **out) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                       // +0x04
    virtual ULONG __stdcall Release() = 0;                                      // +0x08
    virtual HRESULT __stdcall SetNotificationPositions(DWORD count, const DswPositionNotify *notifies) = 0; // +0x0C
};

struct IDswBuffer {
    virtual HRESULT __stdcall QueryInterface(const GUID &iid, void **out) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                       // +0x04
    virtual ULONG __stdcall Release() = 0;                                      // +0x08
    virtual HRESULT __stdcall Slot03() = 0;                                     // +0x0C
    virtual HRESULT __stdcall GetCurrentPosition(LPDWORD playCursor, LPDWORD writeCursor) = 0; // +0x10
    virtual HRESULT __stdcall Slot05() = 0;                                     // +0x14
    virtual HRESULT __stdcall Slot06() = 0;                                     // +0x18
    virtual HRESULT __stdcall Slot07() = 0;                                     // +0x1C
    virtual HRESULT __stdcall Slot08() = 0;                                     // +0x20
    virtual HRESULT __stdcall GetStatus(LPDWORD status) = 0;                    // +0x24
    virtual HRESULT __stdcall Slot10() = 0;                                     // +0x28
    virtual HRESULT __stdcall Lock(DWORD offset, DWORD bytes, void **ptr1, LPDWORD bytes1, void **ptr2, LPDWORD bytes2, DWORD flags) = 0; // +0x2C
    virtual HRESULT __stdcall Play(DWORD r1, DWORD prio, DWORD flags) = 0;      // +0x30
    virtual HRESULT __stdcall SetCurrentPosition(DWORD pos) = 0;                // +0x34
    virtual HRESULT __stdcall SetFormat(const WAVEFORMATEX *fmt) = 0;           // +0x38
    virtual HRESULT __stdcall SetVolume(LONG vol) = 0;                          // +0x3C
    virtual HRESULT __stdcall SetPan(LONG pan) = 0;                             // +0x40
    virtual HRESULT __stdcall Slot17() = 0;                                     // +0x44
    virtual HRESULT __stdcall Stop() = 0;                                       // +0x48
    virtual HRESULT __stdcall Unlock(void *ptr1, DWORD bytes1, void *ptr2, DWORD bytes2) = 0; // +0x4C
    virtual HRESULT __stdcall Restore() = 0;                                    // +0x50
};

struct DswBufferDesc {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX *lpwfxFormat;
    GUID guid3DAlgorithm;
};

struct IDswDirectSound8 {
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0; // +0x00
    virtual ULONG __stdcall AddRef() = 0;                                // +0x04
    virtual ULONG __stdcall Release() = 0;                               // +0x08
    virtual HRESULT __stdcall CreateSoundBuffer(const DswBufferDesc *desc, IDswBuffer **out, void *outer) = 0; // +0x0C
};

struct DswTrackInfo {
    char name[16];          // +0x00
    DWORD dataOffset;       // +0x10
    DWORD field14;          // +0x14
    DWORD loopStart;        // +0x18
    DWORD dataSize;         // +0x1C
    WAVEFORMATEX format;    // +0x20 (+0x2E = wBitsPerSample)
};

struct DswWaveFile {
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
    DswTrackInfo *trackInfo;   // +0x90
    const char *filename;      // +0x94
    DWORD savedFilePos;        // +0x98

    DswWaveFile() {
        trackInfo = NULL;
        hmmio = NULL;
        size = 0;
        fromMemory = 0;
    }
    ~DswWaveFile() {
        if (mode == 1) {
            CloseHandle(handle);
            handle = INVALID_HANDLE_VALUE;
        }
    }
    HRESULT OpenFile(const char *path, DswTrackInfo *info);
    HRESULT ReopenTrack(DswTrackInfo *info, DWORD seekPos);
    HRESULT ResetFile(bool loop, DWORD seekPos);
    HRESULT Read(unsigned char *dst, DWORD sizeToRead, DWORD *outRead);
    HRESULT Close();
};

struct DswSoundManager;
struct DswStreamingSound;

struct DswSound {
    IDswBuffer **buffers;       // +0x04
    DWORD bufferSize;           // +0x08
    DswWaveFile *waveFile;      // +0x0C
    DWORD numBuffers;           // +0x10
    int fadeRemaining;          // +0x14
    int fadeTotal;              // +0x18
    int fadeMode;               // +0x1C
    DWORD playPriority;         // +0x20
    DWORD playFlags;            // +0x24
    DWORD field28;              // +0x28
    DWORD field2C;              // +0x2C
    int isPlayingFlag;          // +0x30
    int isPausedFlag;           // +0x34
    DswBufferDesc dsbd;         // +0x38
    DswSoundManager *soundMgr;  // +0x5C

    DswSound(IDswBuffer **apDSBuffer, DWORD dwDSBufferSize, DWORD dwNumBuffers, DswWaveFile *pWaveFile);
    virtual ~DswSound();

    HRESULT FillBufferWithSound(IDswBuffer *pDSB, BOOL bRepeatWavIfBufferLarger);
    HRESULT RestoreBuffer(IDswBuffer *buf, int *wasRestored);
    IDswBuffer *GetFreeBuffer();
    HRESULT Play();
    HRESULT SetVolume(int volume);
    HRESULT Resume();
    HRESULT Reset();
};

struct DswStreamingSound : public DswSound {
    DWORD lastPlayPos;                      // +0x60
    DWORD playProgress;                     // +0x64
    DWORD nextWriteOffset;                  // +0x68
    BOOL fillNextNotificationWithSilence;   // +0x6C
    DWORD notifySize;                       // +0x70
    HANDLE notifyEvent;                     // +0x74
    DWORD unused78;                         // +0x78

    DswStreamingSound(IDswBuffer *pDSBuffer, DWORD dwDSBufferSize, DswWaveFile *pWaveFile, DWORD dwNotifySize);
    virtual ~DswStreamingSound();

    HRESULT HandleWaveStreamNotification(BOOL bLoopedPlay);
    HRESULT ResetStream();
};

struct DswSoundManager {
    IDswDirectSound8 *ds; // +0x00

    HRESULT CreateStreamingFromFile(
        DswStreamingSound **ppStreamingSound,
        const char *strWaveFileName,
        DWORD dwCreationFlags,
        GUID guid3DAlgorithm,
        DWORD dwNotifyCount,
        DWORD dwNotifySize,
        HANDLE hNotifyEvent,
        DswTrackInfo *info);

    HRESULT CreateStreamingFromMemory(
        DswStreamingSound **ppStreamingSound,
        unsigned char *data,
        DWORD dataSize,
        DswTrackInfo *info,
        DWORD dwCreationFlags,
        GUID guid3DAlgorithm,
        DWORD dwNotifyCount,
        DWORD dwNotifySize,
        HANDLE hNotifyEvent);
};

extern DWORD g_DswBgmBaseOffset;
extern int g_DswBgmVolume;
extern const GUID g_DswIID_IDirectSoundNotify;

void __cdecl DswWaveDebugLog(const char *fmt, ...);

} // namespace th128
