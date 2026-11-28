#pragma once

#include <windows.h>
#include <mmsystem.h>

namespace th128 {

struct CcwJoyKeyCfg {
    short shot0;
    short bomb2;
    short slow4;
    short skip6;
    short pause8;
};

struct CcwJoyCaps {
    unsigned int xMin0;
    unsigned int xMax4;
    unsigned int yMin8;
    unsigned int yMaxC;
};

struct CcwDiJoyState2 {
    LONG lX;
    LONG lY;
    LONG lZ;
    LONG lRx;
    LONG lRy;
    LONG lRz;
    LONG rglSlider[2];
    DWORD rgdwPOV[4];
    BYTE rgbButtons[128];
    LONG lVX;
    LONG lVY;
    LONG lVZ;
    LONG lVRx;
    LONG lVRy;
    LONG lVRz;
    LONG rglVSlider[2];
    LONG lAX;
    LONG lAY;
    LONG lAZ;
    LONG lARx;
    LONG lARy;
    LONG lARz;
    LONG rglASlider[2];
    LONG lFX;
    LONG lFY;
    LONG lFZ;
    LONG lFRx;
    LONG lFRy;
    LONG lFRz;
    LONG rglFSlider[2];
};

struct CcwIDirectInputDevice8 {
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetCapabilities(void *) = 0;
    virtual HRESULT __stdcall EnumObjects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall GetProperty(const void *, void *) = 0;
    virtual HRESULT __stdcall SetProperty(const void *, const void *) = 0;
    virtual HRESULT __stdcall Acquire() = 0;
    virtual HRESULT __stdcall Unacquire() = 0;
    virtual HRESULT __stdcall GetDeviceState(DWORD cbData, void *lpvData) = 0;
    virtual HRESULT __stdcall GetDeviceData(DWORD, void *, DWORD *, DWORD) = 0;
    virtual HRESULT __stdcall SetDataFormat(const void *) = 0;
    virtual HRESULT __stdcall SetEventNotification(HANDLE) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND, DWORD) = 0;
    virtual HRESULT __stdcall GetObjectInfo(void *, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall GetDeviceInfo(void *) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND, DWORD) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE, DWORD, const void *) = 0;
    virtual HRESULT __stdcall CreateEffect(const void *, const void *, void **, void *) = 0;
    virtual HRESULT __stdcall EnumEffects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall GetEffectInfo(void *, const void *) = 0;
    virtual HRESULT __stdcall GetForceFeedbackState(DWORD *) = 0;
    virtual HRESULT __stdcall SendForceFeedbackCommand(DWORD) = 0;
    virtual HRESULT __stdcall EnumCreatedEffectObjects(void *, void *, DWORD) = 0;
    virtual HRESULT __stdcall Escape(void *) = 0;
    virtual HRESULT __stdcall Poll() = 0;
};

struct CcwDSBufferDesc {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX *lpwfxFormat;
    GUID guid3DAlgorithm;
};

struct CcwIDirectSoundBuffer {
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetCaps(void *) = 0;
    virtual HRESULT __stdcall GetCurrentPosition(DWORD *, DWORD *) = 0;
    virtual HRESULT __stdcall GetFormat(WAVEFORMATEX *, DWORD, DWORD *) = 0;
    virtual HRESULT __stdcall GetVolume(LONG *) = 0;
    virtual HRESULT __stdcall GetPan(LONG *) = 0;
    virtual HRESULT __stdcall GetFrequency(DWORD *) = 0;
    virtual HRESULT __stdcall GetStatus(DWORD *pdwStatus) = 0;
    virtual HRESULT __stdcall Initialize(void *, const void *) = 0;
    virtual HRESULT __stdcall Lock(DWORD dwOffset, DWORD dwBytes, void **ppvAudioPtr1, DWORD *pdwAudioBytes1,
        void **ppvAudioPtr2, DWORD *pdwAudioBytes2, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Play(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall SetCurrentPosition(DWORD) = 0;
    virtual HRESULT __stdcall SetFormat(const WAVEFORMATEX *) = 0;
    virtual HRESULT __stdcall SetVolume(LONG) = 0;
    virtual HRESULT __stdcall SetPan(LONG) = 0;
    virtual HRESULT __stdcall SetFrequency(DWORD) = 0;
    virtual HRESULT __stdcall Stop() = 0;
    virtual HRESULT __stdcall Unlock(void *pvAudioPtr1, DWORD dwAudioBytes1, void *pvAudioPtr2, DWORD dwAudioBytes2) = 0;
    virtual HRESULT __stdcall Restore() = 0;
};

struct CcwIDirectSound8 {
    virtual HRESULT __stdcall QueryInterface(const void *, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall CreateSoundBuffer(const CcwDSBufferDesc *pcDSBufferDesc, CcwIDirectSoundBuffer **ppDSBuffer, void *pUnkOuter) = 0;
    virtual HRESULT __stdcall GetCaps(void *) = 0;
    virtual HRESULT __stdcall DuplicateSoundBuffer(CcwIDirectSoundBuffer *pDSBufferOriginal, CcwIDirectSoundBuffer **ppDSBufferDuplicate) = 0;
};

struct CcwSfxInfo {
    int pad0;
    int waveIdx4;
};

struct CcwSoundBuffer {
    CcwIDirectSoundBuffer *buf0;
    int pad4;
    CcwSfxInfo *info8;
    int countC;
    int pad10;
    int pad14;

    int LoadWave(const char *name);
};

struct CcwWaveHeader {
    unsigned char pad0[0x10];
    int dataOffset10;
    unsigned char pad14[8];
    int dataSize1c;
    unsigned char pad20[0xe];
    unsigned short bitsPerSample2e;
};

struct CcwWaveFile {
    unsigned char pad0[8];
    int remaining8;
    unsigned char padC[0x7c - 0xc];
    int isMemory7c;
    unsigned char *memStart80;
    unsigned char *memCur84;
    int memRemaining88;
    HANDLE fileHandle8c;
    CcwWaveHeader *hdr90;

    HRESULT Read(unsigned char *dst, DWORD size, DWORD *outRead);

    inline HRESULT ResetFile() {
        if (isMemory7c != 0) {
            memCur84 = memStart80;
            if (hdr90->dataSize1c > 0) {
                memRemaining88 = hdr90->dataSize1c;
            }
        } else {
            if (fileHandle8c == INVALID_HANDLE_VALUE || fileHandle8c == NULL) {
                return (HRESULT)0x800401f0;
            }
            SetFilePointer(fileHandle8c, hdr90->dataOffset10 + g_CcwBgmBaseOffset, NULL, FILE_BEGIN);
            remaining8 = hdr90->dataSize1c;
        }
        return 0;
    }

    static int g_CcwBgmBaseOffset;
};

struct CcwDSound {
    void *vtable0;
    CcwIDirectSoundBuffer **buffers4;
    DWORD bufferSize8;
    CcwWaveFile *waveFileC;

    HRESULT FillBufferWithSound(CcwIDirectSoundBuffer *pDSB, BOOL bRepeatWavIfBufferLarger);
};

struct CcwReplayHeader {
    unsigned int magic0;
    unsigned short version4;
    unsigned short pad6;
    unsigned char pad8[0x14];
    unsigned int compSize1c;
    unsigned int uncompSize20;
};

struct CcwReplayData {
    unsigned char pad0[0x58];
    int stageCount58;
    unsigned char pad5c[0x70 - 0x5c];
};

struct CcwReplayStageHdr {
    short stageId0;
    short pad2;
    int frameCount4;
    int dataSize8;
    unsigned char padC[0x90 - 0xc];
};

struct CcwReplayStageSlot {
    unsigned char *frameData0;
    int pad4;
    unsigned char *frameEnd8;
    int padC;
    CcwReplayStageHdr *hdr10;
    unsigned char pad14[0x24 - 0x14];
};

struct CcwReplayMgr {
    unsigned char pad0[0x18];
    CcwReplayHeader *header18;
    CcwReplayData *data1c;
    unsigned char pad20[0x138 - 0x20];
    CcwReplayStageSlot stages138[10];
    unsigned char pad2a0[0x39c - 0x2a0];
    void *uncompBuf39c;
    unsigned char pad3a0[0x3b4 - 0x3a0];
    char filename3b4[256];

    int LoadReplay(const char *filename);
};

struct CcwErrorCtx {
    char buffer[0x2000];
    char *cursor;
    int fatal;
    const char *__cdecl Log(const char *fmt, ...);
};

extern unsigned int g_CcwSupFlags;
extern CcwJoyKeyCfg g_CcwJoyKeyCfg;
extern CcwJoyCaps g_CcwJoyCaps;
extern CcwIDirectInputDevice8 *g_CcwDiJoy;
extern short g_CcwPadDeadX;
extern short g_CcwPadDeadY;

extern int g_CcwSoundEnabled;
extern CcwIDirectSound8 *g_CcwDSound;
extern CcwSoundBuffer g_CcwSfxEntries[64];
extern void *g_CcwWaveBufs[128];
extern int g_CcwLoadThreadState;
extern CcwErrorCtx g_CcwErrorCtx;

extern unsigned char g_CcwStageFlags;
extern char g_CcwAppDataDir[260];
extern char g_CcwOrigDir[260];

void __cdecl CcwDebugLog(const char *fmt, ...);
void __cdecl CcwSoundDebugLog(const char *fmt, ...);
unsigned char *__stdcall CcwFindRiffChunk(unsigned char *data, unsigned int size, unsigned int *outSize, const char *tag);

int __stdcall CcwFileExists(const char *path);
int CcwOpenReadStream(const char *path);
void *CcwReadStream(unsigned int size);
void CcwCloseStream();
void *__stdcall CcwLoadPackedFile(const char *path, unsigned int *outSize, int isExternal);
unsigned char *__stdcall CcwDecryptBlocks(unsigned char *data, int size, unsigned char key, unsigned char step, int blockSize, int limit);
void __stdcall CcwDecompressLzss(const unsigned char *src, unsigned int srcSize, void *dst, unsigned int dstSize);

unsigned int __fastcall CcwMergeJoyInput(unsigned int flags);

} // namespace th128
