#include "CoreInputCollision.hpp"

#include <stdlib.h>
#include <string.h>

extern "C" {
__declspec(dllimport) void __stdcall EnterCriticalSection(void *lpCriticalSection);
__declspec(dllimport) void __stdcall LeaveCriticalSection(void *lpCriticalSection);
__declspec(dllimport) void *__stdcall CreateFileA(
    const char *lpFileName,
    unsigned long dwDesiredAccess,
    unsigned long dwShareMode,
    void *lpSecurityAttributes,
    unsigned long dwCreationDisposition,
    unsigned long dwFlagsAndAttributes,
    void *hTemplateFile);
__declspec(dllimport) unsigned long __stdcall GetFileSize(void *hFile, unsigned long *lpFileSizeHigh);
__declspec(dllimport) int __stdcall ReadFile(
    void *hFile,
    void *lpBuffer,
    unsigned long nNumberOfBytesToRead,
    unsigned long *lpNumberOfBytesRead,
    void *lpOverlapped);
__declspec(dllimport) int __stdcall CloseHandle(void *hObject);
__declspec(dllimport) unsigned int __stdcall joyGetPosEx(unsigned int uJoyID, void *pji);
__declspec(dllimport) int __stdcall GetKeyboardState(unsigned char *lpKeyState);
}

namespace th128 {

void *__stdcall CiLoadFileFromArchiveOrDisk(const char *path, unsigned int *outSize, int fromDisk) {
    if (g_CiFlags & 0x8000) {
        EnterCriticalSection(&g_CiFileCs);
        g_CiFileLockCount++;
    }

    void *buf = 0;
    unsigned long size;
    if (fromDisk == 0) {
        const char *p = strrchr(path, '\\');
        p = (p == 0) ? path : (p + 1);
        const char *q = strrchr(p, '/');
        q = (q == 0) ? path : (q + 1);

        size = 0;
        if (g_CiArchive.entries != 0) {
            CiArchiveEntry *entry = g_CiArchive.entries;
            int cnt = g_CiArchive.entryCount;
            while (cnt > 0) {
                if (_stricmp(q, entry->name) == 0) {
                    size = entry->size;
                    break;
                }
                cnt--;
                entry++;
            }
        }
        if (outSize != 0) {
            *outSize = size;
        }
        if (size == 0) {
            if (g_CiFlags & 0x8000) {
                LeaveCriticalSection(&g_CiFileCs);
                g_CiFileLockCount--;
            }
            return 0;
        }
        CiLogPrintf("data %s\n", q);
        buf = malloc(size);
        if (buf == 0) {
            if (g_CiFlags & 0x8000) {
                LeaveCriticalSection(&g_CiFileCs);
                g_CiFileLockCount--;
            }
            return 0;
        }
        CiArchiveExtract(&g_CiArchive, q, buf);
    } else {
        CiLogPrintf("file %s\n", path);
        void *h = CreateFileA(path, 0x80000000u, 1u, 0, 3u, 0x08000080u, 0);
        if (h == (void *)-1) {
            CiLogPrintf("file open error %s\n", path);
            if (g_CiFlags & 0x8000) {
                LeaveCriticalSection(&g_CiFileCs);
                g_CiFileLockCount--;
            }
            return 0;
        }
        size = GetFileSize(h, 0);
        buf = malloc(size);
        if (buf == 0) {
            CiLogPrintf("file alloc error %s\n", path);
            CloseHandle(h);
            if (g_CiFlags & 0x8000) {
                LeaveCriticalSection(&g_CiFileCs);
                g_CiFileLockCount--;
            }
            return 0;
        }
        ReadFile(h, buf, size, &size, 0);
        if (outSize != 0) {
            *outSize = size;
        }
        CloseHandle(h);
    }

    if (g_CiFlags & 0x8000) {
        LeaveCriticalSection(&g_CiFileCs);
        g_CiFileLockCount--;
    }
    return buf;
}

unsigned int CiPollJoystick(unsigned int mask) {
    if ((g_CiFlags & 0x800) == 0) {
        CiJoyInfoEx ji;
        memset(&ji, 0, sizeof(ji));
        ji.dwSize = sizeof(ji);
        ji.dwFlags = 0xff;
        if (joyGetPosEx(0, &ji) != 0) {
            return mask;
        }
        if (g_CiJoyConfig.btnShot >= 0) {
            mask |= (ji.dwButtons & (1u << g_CiJoyConfig.btnShot)) ? 0x0001u : 0u;
        }
        if (g_CiJoyConfig.btnBomb >= 0) {
            mask |= (ji.dwButtons & (1u << g_CiJoyConfig.btnBomb)) ? 0x0002u : 0u;
        }
        if (g_CiJoyConfig.btnSkip >= 0) {
            mask |= (ji.dwButtons & (1u << g_CiJoyConfig.btnSkip)) ? 0x0100u : 0u;
        }
        if (g_CiJoyConfig.btnSlow >= 0) {
            mask |= (ji.dwButtons & (1u << g_CiJoyConfig.btnSlow)) ? 0x0008u : 0u;
        }
        if (g_CiJoyConfig.btnPause >= 0) {
            mask |= (ji.dwButtons & (1u << g_CiJoyConfig.btnPause)) ? 0x0200u : 0u;
        }
        unsigned int halfRangeX = (g_CiJoyMaxX - g_CiJoyMinX) >> 2;
        unsigned int centerX = (g_CiJoyMaxX + g_CiJoyMinX) >> 1;
        if (ji.dwXpos > centerX + halfRangeX) mask |= 0x0080u;
        if (ji.dwXpos < centerX - halfRangeX) mask |= 0x0040u;
        unsigned int halfRangeY = (g_CiJoyMaxY - g_CiJoyMinY) >> 2;
        unsigned int centerY = (g_CiJoyMinY + g_CiJoyMaxY) >> 1;
        if (ji.dwYpos > centerY + halfRangeY) mask |= 0x0020u;
        if (ji.dwYpos < centerY - halfRangeY) mask |= 0x0010u;
        return mask;
    }

    if (g_CiJoystickDev->vtbl->Poll(g_CiJoystickDev) < 0) {
        long hr = g_CiJoystickDev->vtbl->Acquire(g_CiJoystickDev);
        int tries = 0;
        while (hr == (long)0x8007001e) {
            hr = g_CiJoystickDev->vtbl->Acquire(g_CiJoystickDev);
            CiLogPrintf("acquire %d\n", tries);
            tries++;
            if (tries >= 400) {
                return mask;
            }
        }
        return mask;
    }

    CiDIJoyState2 js;
    memset(&js, 0, sizeof(js));
    if (g_CiJoystickDev->vtbl->GetDeviceState(g_CiJoystickDev, sizeof(js), &js) < 0) {
        return mask;
    }

    if (g_CiJoyConfig.btnShot >= 0) {
        mask |= (js.rgbButtons[g_CiJoyConfig.btnShot] & 0x80) ? 0x0001u : 0u;
    }
    if (g_CiJoyConfig.btnBomb >= 0) {
        mask |= (js.rgbButtons[g_CiJoyConfig.btnBomb] & 0x80) ? 0x0002u : 0u;
    }
    if (g_CiJoyConfig.btnSkip >= 0) {
        mask |= (js.rgbButtons[g_CiJoyConfig.btnSkip] & 0x80) ? 0x0100u : 0u;
    }
    if (g_CiJoyConfig.btnSlow >= 0) {
        mask |= (js.rgbButtons[g_CiJoyConfig.btnSlow] & 0x80) ? 0x0008u : 0u;
    }
    if (g_CiJoyConfig.btnPause >= 0) {
        mask |= (js.rgbButtons[g_CiJoyConfig.btnPause] & 0x80) ? 0x0200u : 0u;
    }
    if (js.lX < -g_CiJoyDeadzoneX) mask |= 0x0040u;
    if (js.lY < -g_CiJoyDeadzoneY) mask |= 0x0010u;
    if (js.lX > g_CiJoyDeadzoneX) mask |= 0x0080u;
    if (js.lX > g_CiJoyDeadzoneY) mask |= 0x0020u;
    return mask;
}

unsigned char *CiPollJoystickRaw() {
    memset(g_CiJoyRawButtons, 0, sizeof(g_CiJoyRawButtons));
    if ((g_CiFlags & 0x800) == 0) {
        CiJoyInfoEx ji;
        memset(&ji, 0, sizeof(ji));
        ji.dwSize = sizeof(ji);
        ji.dwFlags = 0xff;
        if (joyGetPosEx(0, &ji) == 0) {
            unsigned int btns = ji.dwButtons;
            for (unsigned int i = 0; i < 32; ++i) {
                if (btns & 1u) {
                    g_CiJoyRawButtons[i] = 0x80;
                }
                btns >>= 1;
            }
        }
    } else {
        if (g_CiJoystickDev->vtbl->Poll(g_CiJoystickDev) < 0) {
            long hr = g_CiJoystickDev->vtbl->Acquire(g_CiJoystickDev);
            int tries = 0;
            while (hr == (long)0x8007001e) {
                hr = g_CiJoystickDev->vtbl->Acquire(g_CiJoystickDev);
                tries++;
                if (tries >= 400) {
                    break;
                }
            }
        } else {
            CiDIJoyState2 js;
            g_CiJoystickDev->vtbl->GetDeviceState(g_CiJoystickDev, sizeof(js), &js);
            memcpy(g_CiJoyRawButtons, js.rgbButtons, sizeof(g_CiJoyRawButtons));
        }
    }
    return g_CiJoyRawButtons;
}

unsigned int CiPollInput() {
    unsigned int mask = 0;
    if (g_CiWindowActive != 0) {
        unsigned char ks[256];
        if ((g_CiFlags & 0x400) == 0) {
            GetKeyboardState(ks);
            if (ks[0x5a] & 0x80) mask |= 0x0001u;
            if (ks[0x58] & 0x80) mask |= 0x0002u;
            if (ks[0x10] & 0x80) mask |= 0x0008u;
            if ((ks[0x26] & 0x80) || (ks[0x68] & 0x80)) mask |= 0x0010u;
            if ((ks[0x28] & 0x80) || (ks[0x62] & 0x80)) mask |= 0x0020u;
            if ((ks[0x25] & 0x80) || (ks[0x64] & 0x80)) mask |= 0x0040u;
            if ((ks[0x27] & 0x80) || (ks[0x66] & 0x80)) mask |= 0x0080u;
            if (ks[0x11] & 0x80) mask |= 0x0100u;
            if (ks[0x1b] & 0x80) mask |= 0x0200u;
            if (ks[0x51] & 0x80) mask |= 0x0400u;
            if (ks[0x53] & 0x80) mask |= 0x0800u;
            if ((ks[0x50] & 0x80) || (ks[0x24] & 0x80)) mask |= 0x1000u;
            if (ks[0x0d] & 0x80) mask |= 0x2000u;
            if (ks[0x44] & 0x80) mask |= 0x4000u;
            if (ks[0x52] & 0x80) mask |= 0x8000u;
            if (ks[0x63] & 0x80) mask |= 0x00a0u;
            if (ks[0x69] & 0x80) mask |= 0x0090u;
            if (ks[0x61] & 0x80) mask |= 0x0060u;
            if (ks[0x67] & 0x80) mask |= 0x0050u;
        } else {
            long hr = g_CiKeyboardDev->vtbl->GetDeviceState(g_CiKeyboardDev, 256, ks);
            if (hr == (long)0x8007001e || hr != 0) {
                g_CiKeyboardDev->vtbl->Acquire(g_CiKeyboardDev);
            } else {
                if (ks[0x2c] & 0x80) mask |= 0x0001u;
                if (ks[0x2d] & 0x80) mask |= 0x0002u;
                if (ks[0x2a] & 0x80) mask |= 0x0008u;
                if ((ks[0xc8] & 0x80) || (ks[0x48] & 0x80)) mask |= 0x0010u;
                if ((ks[0xd0] & 0x80) || (ks[0x50] & 0x80)) mask |= 0x0020u;
                if ((ks[0xcb] & 0x80) || (ks[0x4b] & 0x80)) mask |= 0x0040u;
                if ((ks[0xcd] & 0x80) || (ks[0x4d] & 0x80)) mask |= 0x0080u;
                if (ks[0x1d] & 0x80) mask |= 0x0100u;
                if (ks[0x01] & 0x80) mask |= 0x0200u;
                if (ks[0x10] & 0x80) mask |= 0x0400u;
                if (ks[0x1f] & 0x80) mask |= 0x0800u;
                if ((ks[0x19] & 0x80) || (ks[0xc7] & 0x80)) mask |= 0x1000u;
                if (ks[0x1c] & 0x80) mask |= 0x2000u;
                if (ks[0x20] & 0x80) mask |= 0x4000u;
                if (ks[0x13] & 0x80) mask |= 0x8000u;
                if (ks[0x49] & 0x80) mask |= 0x0090u;
                if (ks[0x4f] & 0x80) mask |= 0x0060u;
                if (ks[0x47] & 0x80) mask |= 0x0050u;
                if (ks[0x51] & 0x80) mask |= 0x00a0u;
            }
        }
    }
    return CiPollJoystick(mask);
}

int __stdcall CiHitTestSegments(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy) {
    float t2, t1;
    t1 = (ax - bx) * (cy - ay) + (ay - by) * (ax - cx);
    t2 = (ax - bx) * (dy - ay) + (ay - by) * (ax - dx);
    if (t2 * t1 > 0.0f) {
        return 0;
    }
    if (t1 == 0.0f && t2 == 0.0f) {
        float tmp;
        if (bx < ax) {
            tmp = bx; bx = ax; ax = tmp;
            tmp = by; by = ay; ay = tmp;
        }
        if (dx < cx) {
            tmp = dx; dx = cx; cx = tmp;
            tmp = dy; dy = cy; cy = tmp;
        }
        if (dx >= ax && dy >= ay && bx >= cx && by >= cy) {
            return 1;
        }
        return 0;
    }
    float u1 = (cx - dx) * (ay - cy) + (cy - dy) * (cx - ax);
    float u2 = (cx - dx) * (by - cy) + (cy - dy) * (cx - bx);
    if (u1 * u2 > 0.0f) {
        return 0;
    }
    return 1;
}

} // namespace th128
