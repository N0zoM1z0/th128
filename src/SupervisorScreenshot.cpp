#include "SupervisorScreenshot.hpp"
#include "SupervisorFrame.hpp"
#include "SupervisorSurfaceCopies.hpp"
#include "Supervisor.hpp"
#include "Input.hpp"
#include "FileSystem.hpp"
#include "GameErrorContext.hpp"

#include <d3d9.h>
#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Distinct reviewed release logger at 0x004360F0.
void ScreenshotDebugLog(const char *format, ...);

namespace
{
    // Partial screenshot state rooted at Supervisor + 0x6D8 (0x004D3538).
    struct SupervisorScreenshotState
    {
        uintptr_t worker;
        BITMAPFILEHEADER fileHeader;
        BITMAPINFO *info;
        unsigned char *pixels;
        char filename[MAX_PATH];
    };
    typedef char ScreenshotHeaderOffset[(offsetof(SupervisorScreenshotState, fileHeader) == 4) ? 1 : -1];
    typedef char ScreenshotInfoOffset[(offsetof(SupervisorScreenshotState, info) == 0x14) ? 1 : -1];
    typedef char ScreenshotFilenameOffset[(offsetof(SupervisorScreenshotState, filename) == 0x1C) ? 1 : -1];

    inline SupervisorScreenshotState &Screenshot()
    {
        return *reinterpret_cast<SupervisorScreenshotState *>(
            reinterpret_cast<unsigned char *>(&g_Supervisor) + 0x6D8);
    }
}

void __cdecl WriteSupervisorScreenshot(void *)
{
    FileSystem::OpenWriteStream(Screenshot().filename);
    FileSystem::WriteStream(&Screenshot().fileHeader, sizeof(BITMAPFILEHEADER));
    FileSystem::WriteStream(Screenshot().info, sizeof(BITMAPINFOHEADER));
    FileSystem::WriteStream(Screenshot().pixels, 640 * 480 * 3);
    FileSystem::CloseStream();
    if (Screenshot().info != 0)
    {
        free(Screenshot().info);
        Screenshot().info = 0;
    }
    if (Screenshot().pixels != 0)
    {
        free(Screenshot().pixels);
        Screenshot().pixels = 0;
    }
    Screenshot().worker = 0;
}

int CaptureSupervisorScreenshot(const char *filename)
{
    while (Screenshot().worker != 0)
        Sleep(10);

    IDirect3DSurface9 *surface = 0;
    ScreenshotDebugLog("SnapShot! %s\n", filename);
    g_Supervisor.d3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);

    memset(&Screenshot().fileHeader, 0, sizeof(BITMAPFILEHEADER));
    Screenshot().fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    Screenshot().fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    Screenshot().fileHeader.bfType = 0x4D42;
    strcpy(Screenshot().filename, filename);

    switch (g_Supervisor.backBufferFormat)
    {
    case D3DFMT_R5G6B5:
        g_GameErrorContext.Log("16bit \x82\xcd\x8e\xe6\x82\xe8\x8d\x9e\x82\xdf\x82\xc8\x82\xa2\r\n");
        break;
    case D3DFMT_X8R8G8B8:
        Screenshot().info = static_cast<BITMAPINFO *>(malloc(sizeof(BITMAPINFO)));
        if (Screenshot().info == 0)
            goto allocationFailure;
        memset(Screenshot().info, 0, sizeof(BITMAPINFO));
        Screenshot().pixels = static_cast<unsigned char *>(malloc(640 * 480 * 3));
        if (Screenshot().pixels == 0)
        {
        allocationFailure:
            g_GameErrorContext.Log("snapShotScreen : \x8a\x6d\x95\xdb\x82\xb5\x82\xad\x82\xe8\r\n");
            break;
        }

        Screenshot().fileHeader.bfSize += 640 * 480 * 3;
        Screenshot().info->bmiHeader.biBitCount = 24;
        Screenshot().info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        Screenshot().info->bmiHeader.biWidth = 640;
        Screenshot().info->bmiHeader.biHeight = 480;
        Screenshot().info->bmiHeader.biPlanes = 1;
        Screenshot().info->bmiHeader.biCompression = BI_RGB;
        {
            D3DLOCKED_RECT locked;
            surface->LockRect(&locked, 0, 0);
            int y = 479;
            for (int offset = 0; y > -1; --y, offset += 640 * 3)
            {
                unsigned char *destination = Screenshot().pixels + offset;
                unsigned char *source = static_cast<unsigned char *>(locked.pBits) + locked.Pitch * y;
                for (int remaining = 640; remaining != 0; --remaining)
                {
                    *reinterpret_cast<RGBTRIPLE *>(destination) = *reinterpret_cast<RGBTRIPLE *>(source);
                    source += 4;
                    destination += 3;
                }
            }
            surface->UnlockRect();
        }
        Screenshot().worker = _beginthread(WriteSupervisorScreenshot, 0, 0);
        break;
    default:
        // The original returns here before the common surface release.
        g_GameErrorContext.Log("error ? src\\game\\mother.cpp\r\n");
        return 1;
    }
    if (surface != 0)
        surface->Release();
    return 0;
}

void SupervisorFrame::HandleScreenshot()
{
    FlushAnmSurfaceCopies();
    if ((g_InputStates[0].triggerMask & 0x40000) != 0)
    {
        _chdir(reinterpret_cast<char *>(&g_Supervisor) + 0xB39);
        _mkdir("snapshot");
        char filename[256];
        int index;
        for (index = 0; index < 1000; ++index)
        {
            sprintf(filename, "%ssnapshot\\th%.3d.bmp", reinterpret_cast<char *>(this) + 0x29, index);
            if (!FileSystem::FileExists(filename))
                break;
        }
        if (index < 1000)
            CaptureSupervisorScreenshot(filename);
        _chdir(reinterpret_cast<char *>(&g_Supervisor) + 0x1B39);
    }
}
