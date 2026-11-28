#pragma once

#include <windows.h>

namespace th128 {

struct WswSoundBuffer {
    void **vtable;
    HRESULT Stop();
};

struct WswSoundView {
    unsigned char pad00[0x04];
    WswSoundBuffer **buffers;
    unsigned char pad08[0x0c];
    int fadeCur;
    int fadeTotal;
    int fadeMode;

    void SetVolume(int volume);
};

struct WswTextImage {
    unsigned char pad00[0x104];
    int width;
    int height;
    unsigned char pad10c[0x08];
    HDC hdc;

    bool DrawOutlinedText(int x, int y, COLORREF color, COLORREF outlineColor,
                          HFONT font, const char *text);
};

struct WswChainElem {
    unsigned char pad00[0x04];
};

struct WswChain {
    void Cut(WswChainElem *elem);
};

struct WswStageChunk {
    unsigned char data[0x24];
    ~WswStageChunk();
};

struct WswReplayManager {
    unsigned char pad00[0x08];
    WswChainElem *calcElem;
    WswChainElem *drawElem;
    unsigned char pad10[0x08];
    void *f18;
    void *f1c;
    void *f20[17];
    unsigned char pad64[0xd4];
    WswStageChunk chunks[17];
    unsigned char pad39c[0x0c];
    WswChainElem *extraElem;

    ~WswReplayManager();
    void FreeStageChunks(int index);
};

struct WswComObject {
    void **vtable;
    ULONG Release();
};

struct WswDirectInputDevice {
    void **vtable;
    ULONG Release();
    HRESULT GetCapabilities(void *caps);
    HRESULT EnumObjects(void *cb, void *ref, DWORD flags);
    HRESULT Acquire();
    HRESULT SetDataFormat(const void *fmt);
    HRESULT SetCooperativeLevel(HWND hwnd, DWORD flags);
};

struct WswDirectInput {
    void **vtable;
    ULONG Release();
    HRESULT CreateDevice(const GUID &guid, WswDirectInputDevice **dev, void *outer);
    HRESULT EnumDevices(DWORD devType, void *cb, void *ref, DWORD flags);
};

struct WswSupervisor {
    unsigned char pad00[0x0c];
    WswDirectInput *dinput;
    unsigned char pad10[0x10];
    WswDirectInputDevice *keyboard;
    WswDirectInputDevice *pad;
    unsigned char pad28[0x01];
    char appDataPath[0x1000];
    char modulePath[0x1000];
    unsigned char pad2029[0x03];

    void ReleaseDirectInput();
    void InitSystemPathsAndDwm();
};

struct WswInputSupervisor {
    unsigned char pad00[0x0c];
    WswDirectInput *dinput;
    unsigned char pad10[0x10];
    WswDirectInputDevice *keyboard;
    WswDirectInputDevice *pad;
    unsigned char pad28[0x1d8];
    unsigned int flags200;

    void ReleaseDirectInput();
    int InitDirectInput();
};

struct WswErrorContext {
    void Log(const char *fmt, ...);
};

struct WswShellLink {
    void **vtable;
    inline HRESULT QueryInterface(const IID &riid, void **ppv) {
        typedef HRESULT (__stdcall *Fn)(WswShellLink *, const IID &, void **);
        return ((Fn)vtable[0])(this, riid, ppv);
    }
    inline ULONG Release() {
        typedef ULONG (__stdcall *Fn)(WswShellLink *);
        return ((Fn)vtable[2])(this);
    }
    inline HRESULT GetPath(char *pszFile, int cch, WIN32_FIND_DATAA *pfd, DWORD fFlags) {
        typedef HRESULT (__stdcall *Fn)(WswShellLink *, char *, int, WIN32_FIND_DATAA *, DWORD);
        return ((Fn)vtable[3])(this, pszFile, cch, pfd, fFlags);
    }
};

struct WswPersistFile {
    void **vtable;
    inline ULONG Release() {
        typedef ULONG (__stdcall *Fn)(WswPersistFile *);
        return ((Fn)vtable[2])(this);
    }
    inline HRESULT Load(const wchar_t *pszFileName, DWORD dwMode) {
        typedef HRESULT (__stdcall *Fn)(WswPersistFile *, const wchar_t *, DWORD);
        return ((Fn)vtable[5])(this, pszFileName, dwMode);
    }
};

extern WswSoundView *g_WswBgmView;
extern int g_WswSavedScreenSave;
extern int g_WswSavedLowPower;
extern int g_WswSavedPowerOff;
extern LARGE_INTEGER g_WswPerfFreq;
extern LARGE_INTEGER g_WswPerfCounter;
extern WswSupervisor g_WswSupervisor;
extern unsigned int g_WswFlags;
extern CRITICAL_SECTION g_WswCritSec;
extern unsigned char g_WswCritDepth;
extern WswChain *g_WswChain;
extern WswReplayManager *g_WswReplayMgr;
extern unsigned char g_WswLaunchedFromShortcut;
extern int g_WswGlobal4db7e8;
extern HINSTANCE g_WswInstance;
extern HWND g_WswHwnd;
extern WswErrorContext g_WswErrorCtx;
extern unsigned int g_WswDiDevCaps[11];
extern int g_WswPadAxisCount;

extern const GUID g_WswIID_IDirectInput8A;
extern const GUID g_WswGUID_SysKeyboard;
extern const void *g_WswDfDIKeyboard;
extern const void *g_WswDfDIJoystick2;
extern const GUID g_WswCLSID_ShellLink;
extern const GUID g_WswIID_IShellLinkA;
extern const GUID g_WswIID_IPersistFile;

void WswLog(const char *fmt, ...);
int __stdcall WswFileExists(const char *path);
void WswEnumJoysticksCb();
void WswEnumObjectsCb();

void WswBgmFadeUpdate();
unsigned char *__stdcall WswEncryptBlocks(unsigned char *data, int size, unsigned char key,
                                          unsigned char step, int block, int limit);
int WswCheckShortcutLaunch();
int WswResolveShortcut(const char *lnkPath, char *outPath, int maxLen);

} // namespace th128
