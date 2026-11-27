#pragma once

#include <stddef.h>

namespace th128 {

struct CiArchiveEntry {
    const char *name;
    unsigned int offset;
    unsigned int size;
    unsigned int extra;
};

struct CiArchive {
    CiArchiveEntry *entries;
    int entryCount;
};

struct CiCriticalSection {
    unsigned char raw[0x18];
};

struct CiDInputDeviceVtbl {
    void *pad00[7];
    long (__stdcall *Acquire)(void *self);
    void *pad20;
    long (__stdcall *GetDeviceState)(void *self, unsigned long cbData, void *lpvData);
    void *pad28[15];
    long (__stdcall *Poll)(void *self);
};

struct CiDInputDevice {
    CiDInputDeviceVtbl *vtbl;
};

struct CiJoyInfoEx {
    unsigned long dwSize;
    unsigned long dwFlags;
    unsigned long dwXpos;
    unsigned long dwYpos;
    unsigned long dwZpos;
    unsigned long dwRpos;
    unsigned long dwUpos;
    unsigned long dwVpos;
    unsigned long dwButtons;
    unsigned long dwButtonNumber;
    unsigned long dwPOV;
    unsigned long dwReserved1;
    unsigned long dwReserved2;
};

struct CiDIJoyState2 {
    long lX;
    long lY;
    long lZ;
    long lRx;
    long lRy;
    long lRz;
    long rglSlider[2];
    unsigned long rgdwPOV[4];
    unsigned char rgbButtons[128];
    unsigned char padB0[0x60];
};

struct CiJoyConfig {
    short btnShot;
    short btnBomb;
    short btnPause;
    short btnSlow;
    short btnSkip;
};

extern unsigned int g_CiFlags;
extern CiCriticalSection g_CiFileCs;
extern unsigned char g_CiFileLockCount;
extern CiArchive g_CiArchive;
extern int g_CiWindowActive;
extern CiDInputDevice *g_CiKeyboardDev;
extern CiDInputDevice *g_CiJoystickDev;
extern unsigned int g_CiJoyMinX;
extern unsigned int g_CiJoyMaxX;
extern unsigned int g_CiJoyMinY;
extern unsigned int g_CiJoyMaxY;
extern short g_CiJoyDeadzoneX;
extern short g_CiJoyDeadzoneY;
extern CiJoyConfig g_CiJoyConfig;
extern unsigned char g_CiJoyRawButtons[0x80];

void __cdecl CiLogPrintf(const char *fmt, ...);
void __stdcall CiArchiveExtract(CiArchive *arc, const char *name, void *dst);

void *__stdcall CiLoadFileFromArchiveOrDisk(const char *path, unsigned int *outSize, int fromDisk);
unsigned int CiPollJoystick(unsigned int mask);
unsigned char *CiPollJoystickRaw();
unsigned int CiPollInput();
int __stdcall CiHitTestSegments(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy);

} // namespace th128
