#pragma once

#include <windows.h>
#include <mmsystem.h>

namespace th128 {

struct SiwDiPad {
    void **vtable;
    inline HRESULT Acquire() {
        typedef HRESULT (__stdcall *Fn)(SiwDiPad *);
        return ((Fn)vtable[7])(this);
    }
    inline HRESULT GetDeviceState(DWORD cbData, void *lpvData) {
        typedef HRESULT (__stdcall *Fn)(SiwDiPad *, DWORD, void *);
        return ((Fn)vtable[9])(this, cbData, lpvData);
    }
    inline HRESULT Poll() {
        typedef HRESULT (__stdcall *Fn)(SiwDiPad *);
        return ((Fn)vtable[25])(this);
    }
};

struct SiwDiJoyState2 {
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

struct SiwDsBuffer {
    virtual HRESULT __stdcall QueryInterface(const IID &, void **) = 0;
    virtual ULONG __stdcall AddRef() = 0;
    virtual ULONG __stdcall Release() = 0;
    virtual HRESULT __stdcall GetCaps(void *) = 0;
    virtual HRESULT __stdcall GetCurrentPosition(DWORD *playCursor, DWORD *writeCursor) = 0;
    virtual HRESULT __stdcall GetFormat(void *, DWORD, DWORD *) = 0;
    virtual HRESULT __stdcall GetVolume(LONG *) = 0;
    virtual HRESULT __stdcall GetPan(LONG *) = 0;
    virtual HRESULT __stdcall GetFrequency(DWORD *) = 0;
    virtual HRESULT __stdcall GetStatus(DWORD *status) = 0;
    virtual HRESULT __stdcall Initialize(void *, void *) = 0;
    virtual HRESULT __stdcall Lock(DWORD offset, DWORD bytes, void **audioPtr1, DWORD *audioBytes1,
                                   void **audioPtr2, DWORD *audioBytes2, DWORD flags) = 0;
    virtual HRESULT __stdcall Play(DWORD, DWORD, DWORD) = 0;
    virtual HRESULT __stdcall SetCurrentPosition(DWORD newPos) = 0;
    virtual HRESULT __stdcall SetFormat(const void *) = 0;
    virtual HRESULT __stdcall SetVolume(LONG) = 0;
    virtual HRESULT __stdcall SetPan(LONG) = 0;
    virtual HRESULT __stdcall SetFrequency(DWORD) = 0;
    virtual HRESULT __stdcall Stop() = 0;
    virtual HRESULT __stdcall Unlock(void *audioPtr1, DWORD audioBytes1, void *audioPtr2, DWORD audioBytes2) = 0;
    virtual HRESULT __stdcall Restore() = 0;
};

struct SiwWaveDesc {
    unsigned char pad00[0x10];
    int dataOffset;
    unsigned char pad14[0x08];
    int dataSize;
    unsigned char pad20[0x0e];
    unsigned short bitsPerSample;
};

struct SiwWaveFile {
    unsigned char pad00[0x08];
    int remaining;
    unsigned char pad0c[0x20];
    unsigned int totalSize;
    unsigned char pad30[0x4c];
    int isMemory;
    unsigned char *memBase;
    unsigned char *memCur;
    int memRemaining;
    HANDLE fileHandle;
    SiwWaveDesc *desc;

    HRESULT Read(unsigned char *buf, DWORD sizeToRead, DWORD *sizeRead);
    HRESULT ResetFile(bool loop, DWORD seekPos);
};

struct SiwSound {
    void **vtable;
    SiwDsBuffer **buffers;
    unsigned int bufSize;
    SiwWaveFile *waveFile;
    unsigned int numBuffers;
    unsigned char pad14[0x4c];
    unsigned int lastPlayPos;
    unsigned int playProgress;
    unsigned int nextWriteOffset;
    int fillNextNotificationWithSilence;
    unsigned int notifySize;

    HRESULT RestoreBuffer(SiwDsBuffer *pDSB, int *pbWasRestored);
    HRESULT FillBufferWithSound(SiwDsBuffer *pDSB, int bRepeatWavIfBufferLarger);
    HRESULT HandleWaveStreamNotification(int bLoopedPlay);
};

extern unsigned int g_SiwFlags;
extern SiwDiPad *g_SiwPad;
extern short g_SiwBtn0;
extern short g_SiwBtn1;
extern short g_SiwBtn4;
extern short g_SiwBtn3;
extern short g_SiwBtn2;
extern unsigned int g_SiwJoyMinX;
extern unsigned int g_SiwJoyMaxX;
extern unsigned int g_SiwJoyMinY;
extern unsigned int g_SiwJoyMaxY;
extern short g_SiwDeadzoneX;
extern short g_SiwDeadzoneY;
extern int g_SiwBgmBaseOffset;

void SiwDebugLog(const char *fmt, ...);
unsigned int SiwPollGamepad(unsigned int mask);

} // namespace th128
