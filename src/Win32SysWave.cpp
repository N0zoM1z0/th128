#include "Win32SysWave.hpp"

#include <direct.h>
#include <objbase.h>
#include <stdlib.h>
#include <string.h>

extern "C" __declspec(dllimport) HRESULT __stdcall DirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, const IID &riidltf, LPVOID *ppvOut, void *punkOuter);

namespace th128 {

typedef HRESULT (WINAPI *WswDwmEnableCompositionFn)(UINT);

HRESULT WswSoundBuffer::Stop() {
    typedef HRESULT (__stdcall *Fn)(WswSoundBuffer *);
    return ((Fn)vtable[18])(this);
}

ULONG WswDirectInput::Release() {
    typedef ULONG (__stdcall *Fn)(WswDirectInput *);
    return ((Fn)vtable[2])(this);
}

HRESULT WswDirectInput::CreateDevice(const GUID &guid, WswDirectInputDevice **dev, void *outer) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInput *, const GUID &, WswDirectInputDevice **, void *);
    return ((Fn)vtable[3])(this, guid, dev, outer);
}

HRESULT WswDirectInput::EnumDevices(DWORD devType, void *cb, void *ref, DWORD flags) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInput *, DWORD, void *, void *, DWORD);
    return ((Fn)vtable[4])(this, devType, cb, ref, flags);
}

ULONG WswDirectInputDevice::Release() {
    typedef ULONG (__stdcall *Fn)(WswDirectInputDevice *);
    return ((Fn)vtable[2])(this);
}

HRESULT WswDirectInputDevice::GetCapabilities(void *caps) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInputDevice *, void *);
    return ((Fn)vtable[3])(this, caps);
}

HRESULT WswDirectInputDevice::EnumObjects(void *cb, void *ref, DWORD flags) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInputDevice *, void *, void *, DWORD);
    return ((Fn)vtable[4])(this, cb, ref, flags);
}

HRESULT WswDirectInputDevice::Acquire() {
    typedef HRESULT (__stdcall *Fn)(WswDirectInputDevice *);
    return ((Fn)vtable[7])(this);
}

HRESULT WswDirectInputDevice::SetDataFormat(const void *fmt) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInputDevice *, const void *);
    return ((Fn)vtable[11])(this, fmt);
}

HRESULT WswDirectInputDevice::SetCooperativeLevel(HWND hwnd, DWORD flags) {
    typedef HRESULT (__stdcall *Fn)(WswDirectInputDevice *, HWND, DWORD);
    return ((Fn)vtable[13])(this, hwnd, flags);
}

void WswBgmFadeUpdate() {
    if (g_WswBgmView == NULL) {
        return;
    }
    if (g_WswBgmView->fadeMode == 1) {
        if (--g_WswBgmView->fadeCur <= 0) {
            g_WswBgmView->fadeMode = 0;
            g_WswBgmView->buffers[0]->Stop();
        } else {
            g_WswBgmView->SetVolume(g_WswBgmView->fadeCur * 5000 / g_WswBgmView->fadeTotal - 5000);
        }
    }
    if (g_WswBgmView->fadeMode == 2) {
        if (--g_WswBgmView->fadeCur <= 0) {
            g_WswBgmView->fadeMode = 0;
        } else {
            g_WswBgmView->SetVolume(g_WswBgmView->fadeCur * -5000 / g_WswBgmView->fadeTotal);
        }
    }
    if (g_WswBgmView->fadeMode == 4) {
        if (--g_WswBgmView->fadeCur <= 0) {
            g_WswBgmView->fadeMode = 0;
        } else {
            g_WswBgmView->SetVolume(g_WswBgmView->fadeCur * 1000 / g_WswBgmView->fadeTotal - 1000);
        }
    }
    if (g_WswBgmView->fadeMode == 3) {
        if (--g_WswBgmView->fadeCur <= 0) {
            g_WswBgmView->fadeMode = 0;
        } else {
            g_WswBgmView->SetVolume(g_WswBgmView->fadeCur * -1000 / g_WswBgmView->fadeTotal);
        }
    }
}

unsigned char *__stdcall WswEncryptBlocks(unsigned char *data, int size, unsigned char key,
                                          unsigned char step, int block, int limit) {
    int tail = size % block < block / 4 ? size % block : 0;
    int length = size;
    if (limit <= size) {
        length = limit;
    }
    unsigned char *out = data;
    unsigned char *temp = (unsigned char *)malloc(length);
    unsigned char *in = temp;
    if (temp == NULL) {
        return data;
    }
    size -= (size & 1) + tail;
    memcpy(temp, data, length);
    while (size > 0 && limit > 0) {
        if (size < block) {
            block = size;
        }
        unsigned char *end = in + block;
        unsigned char *p = end - 1;
        for (int i = (block + 1) / 2; i > 0; --i) {
            *out = *p ^ key;
            key += step;
            p -= 2;
            ++out;
        }
        p = end - 2;
        for (int i = block / 2; i > 0; --i) {
            *out = *p ^ key;
            key += step;
            p -= 2;
            ++out;
        }
        limit -= block;
        size -= block;
        in = end;
    }
    free(temp);
    return data;
}

bool WswTextImage::DrawOutlinedText(int x, int y, COLORREF color, COLORREF outlineColor,
                                    HFONT font, const char *text) {
    if (text == NULL) {
        return false;
    }
    int w = this->width;
    int h = this->height;
    HDC dc = this->hdc;
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    if (outlineColor != (COLORREF)-1) {
        SetTextColor(dc, outlineColor);
        RECT r0 = { x, y, x + w - 2, y + h - 2 };
        DrawTextA(dc, text, -1, &r0, 0);
        RECT rects[5] = {
            { x + 2, y,     x + w + 2, y + h     },
            { x + 1, y + 2, x + w + 1, y + h + 2 },
            { x + 2, y,     x + w + 2, y + h     },
            { x,     y + 2, x + w,     y + h + 2 },
            { x + 2, y + 2, x + w + 2, y + h + 2 },
        };
        for (int i = 0; i < 5; ++i) {
            DrawTextA(dc, text, -1, &rects[i], 0);
        }
    }
    SetTextColor(dc, color);
    RECT rMain = { x + 1, y + 1, x + w + 1, y + h + 1 };
    DrawTextA(dc, text, -1, &rMain, 0);
    SelectObject(dc, oldFont);
    return true;
}

void WswSupervisor::InitSystemPathsAndDwm() {
    SystemParametersInfoA(SPI_GETSCREENSAVEACTIVE, 0, &g_WswSavedScreenSave, 0);
    SystemParametersInfoA(SPI_GETLOWPOWERACTIVE, 0, &g_WswSavedLowPower, 0);
    SystemParametersInfoA(SPI_GETPOWEROFFACTIVE, 0, &g_WswSavedPowerOff, 0);
    SystemParametersInfoA(SPI_SETSCREENSAVEACTIVE, 0, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETLOWPOWERACTIVE, 0, NULL, SPIF_SENDCHANGE);
    SystemParametersInfoA(SPI_SETPOWEROFFACTIVE, 0, NULL, SPIF_SENDCHANGE);
    QueryPerformanceFrequency(&g_WswPerfFreq);
    QueryPerformanceCounter(&g_WswPerfCounter);
    GetEnvironmentVariableA("APPDATA", this->appDataPath, 0x1000);
    if (this->appDataPath[0] != '\0') {
        strcat(this->appDataPath, "\\ShanghaiAlice");
        _mkdir(g_WswSupervisor.appDataPath);
        strcat(this->appDataPath, "\\th128");
        _mkdir(g_WswSupervisor.appDataPath);
        strcat(this->appDataPath, "\\");
        WswLog("%d\n", this->appDataPath);
    }
    if (GetModuleFileNameA(NULL, this->modulePath, 0x1000) != 0) {
        char *p = strrchr(this->modulePath, '\\');
        if (p != NULL) {
            *p = '\0';
        }
        WswLog("%d\n", this->modulePath);
    }
    _chdir(this->appDataPath);
    _mkdir("replay");
    _chdir(this->modulePath);
    HMODULE hDwm = LoadLibraryA("dwmapi.dll");
    if (hDwm != NULL) {
        WswDwmEnableCompositionFn fn =
            (WswDwmEnableCompositionFn)GetProcAddress(hDwm, "DwmEnableComposition");
        if (fn != NULL) {
            fn(0);
        }
    }
}

int WswResolveShortcut(const char *lnkPath, char *outPath, int maxLen) {
    if (outPath == NULL) {
        return 0;
    }
    int result = 0;
    CoInitialize(NULL);
    WswShellLink *shellLink;
    if (SUCCEEDED(CoCreateInstance(g_WswCLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                   g_WswIID_IShellLinkA, (void **)&shellLink))) {
        WswPersistFile *persistFile;
        if (SUCCEEDED(shellLink->QueryInterface(g_WswIID_IPersistFile, (void **)&persistFile))) {
            wchar_t *widePath = new wchar_t[maxLen];
            MultiByteToWideChar(CP_ACP, 0, lnkPath, -1, widePath, maxLen);
            if (SUCCEEDED(persistFile->Load(widePath, STGM_READ))) {
                WIN32_FIND_DATAA findData;
                if (SUCCEEDED(shellLink->GetPath(outPath, maxLen, &findData, 0))) {
                    result = 1;
                }
            }
            delete widePath;
            persistFile->Release();
        }
        shellLink->Release();
    }
    CoUninitialize();
    return result;
}

int WswCheckShortcutLaunch() {
    STARTUPINFOA si;
    si.cb = sizeof(si);
    memset(&si.lpReserved, 0, sizeof(si) - 4);
    char exePath[261];
    char titleBuf[261];
    GetModuleFileNameA(NULL, exePath, 261);
    GetConsoleTitleA(titleBuf, 261);
    GetStartupInfoA(&si);
    if (si.lpTitle != NULL) {
        char *dot = strrchr(si.lpTitle, '.');
        if (WswFileExists(si.lpTitle) && dot != NULL) {
            if (_stricmp(dot, ".lnk") == 0) {
                do {
                    WswResolveShortcut(si.lpTitle, titleBuf, MAX_PATH);
                } while (_stricmp(strrchr(titleBuf, '.'), ".lnk") == 0);
            } else {
                strcpy(titleBuf, si.lpTitle);
            }
            if (strcmp(exePath, titleBuf) != 0) {
                g_WswFlags &= ~0x40;
                g_WswLaunchedFromShortcut = 1;
                return g_WswGlobal4db7e8 != 0 ? 0 : -1;
            }
        }
        g_WswFlags |= 0x40;
    }
    return g_WswGlobal4db7e8 != 0 ? 0 : -1;
}

static __forceinline void WswCutChainElem(WswChainElem *elem) {
    if (elem != NULL) {
        WswChain *chain = g_WswChain;
        if (g_WswFlags & 0x8000) {
            EnterCriticalSection(&g_WswCritSec);
            ++g_WswCritDepth;
        }
        chain->Cut(elem);
        if (g_WswFlags & 0x8000) {
            LeaveCriticalSection(&g_WswCritSec);
            --g_WswCritDepth;
        }
    }
}

WswReplayManager::~WswReplayManager() {
    operator delete(this->f18);
    for (int i = 0; i < 17; ++i) {
        this->FreeStageChunks(i);
    }
    operator delete(this->f1c);
    this->f1c = NULL;
    for (int i = 0; i < 17; ++i) {
        operator delete(this->f20[i]);
        this->f20[i] = NULL;
    }
    WswCutChainElem(this->calcElem);
    WswCutChainElem(this->extraElem);
    WswCutChainElem(this->drawElem);
    if (this == g_WswReplayMgr) {
        g_WswReplayMgr = NULL;
    }
}

int WswInputSupervisor::InitDirectInput() {
    if (this->flags200 & 8) {
        return -1;
    }
    if (FAILED(DirectInput8Create(g_WswInstance, 0x800, g_WswIID_IDirectInput8A,
                                  (void **)&this->dinput, NULL))) {
        this->dinput = NULL;
        g_WswErrorCtx.Log("DirectInput \x82\xaa\x8eg\x97p\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\r\n");
        return -1;
    }
    if (FAILED(this->dinput->CreateDevice(g_WswGUID_SysKeyboard, &this->keyboard, NULL))) {
        if (this->dinput != NULL) {
            this->dinput->Release();
            this->dinput = NULL;
        }
        g_WswErrorCtx.Log("DirectInput \x82\xaa\x8eg\x97p\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\r\n");
        return -1;
    }
    if (FAILED(this->keyboard->SetDataFormat(g_WswDfDIKeyboard))) {
        if (this->keyboard != NULL) {
            this->keyboard->Release();
            this->keyboard = NULL;
        }
        if (this->dinput != NULL) {
            this->dinput->Release();
            this->dinput = NULL;
        }
        g_WswErrorCtx.Log("DirectInput SetDataFormat \x82\xaa\x8eg\x97p\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\r\n");
        return -1;
    }
    if (FAILED(this->keyboard->SetCooperativeLevel(g_WswHwnd, 0x16))) {
        if (this->keyboard != NULL) {
            this->keyboard->Release();
            this->keyboard = NULL;
        }
        this->ReleaseDirectInput();
        g_WswErrorCtx.Log("DirectInput SetCooperativeLevel \x82\xaa\x8eg\x97p\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\r\n");
        return -1;
    }
    this->keyboard->Acquire();
    g_WswErrorCtx.Log("DirectInput \x82\xcd\x90\xb3\x8f\xed\x82\xc9\x8f\x89\x8a\xfa\x89\xbb\x82\xb3\x82\xea\x82\xdc\x82\xb5\x82\xbd\r\n");
    this->dinput->EnumDevices(4, (void *)WswEnumJoysticksCb, NULL, 1);
    if (this->pad != NULL) {
        this->pad->SetDataFormat(g_WswDfDIJoystick2);
        this->pad->SetCooperativeLevel(g_WswHwnd, 0x0a);
        g_WswDiDevCaps[0] = 0x2c;
        this->pad->GetCapabilities(g_WswDiDevCaps);
        g_WswPadAxisCount = 0;
        this->pad->EnumObjects((void *)WswEnumObjectsCb, NULL, 0);
        g_WswErrorCtx.Log("\x97L\x8c\xf8\x82\xc8\x83p\x83" "b\x83h\x82\xf0\x94\xad\x8c\xa9\x82\xb5\x82\xdc\x82\xb5\x82\xbd\r\n");
    }
    return 0;
}

} // namespace th128
