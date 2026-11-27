#include "ZeroCallMedium2.hpp"

#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace th128 {

void Zc2StageCamOwner::InitGameCameras() {
    g_Zc2Cameras[1].pos = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[1].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[1].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[1].fovOrFar = 0.52359879f;
    g_Zc2Cameras[1].vpX = 0;
    g_Zc2Cameras[1].vpY = 0;
    g_Zc2Cameras[1].vpW = 640;
    g_Zc2Cameras[1].vpH = 480;
    g_Zc2Cameras[1].vpMinZ = 0.0f;
    g_Zc2Cameras[1].vpMaxZ = 1.0f;
    g_Zc2Cameras[1].mode = 1;
    g_Zc2Cameras[1].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);

    g_Zc2Cameras[0].pos = Zc2Vec3(0.0f, 0.0f, 1000.0f);
    g_Zc2Cameras[0].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[0].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[0].fovOrFar = 0.52359879f;
    g_Zc2Cameras[0].vpMinZ = 0.0f;
    g_Zc2Cameras[0].vpMaxZ = 1.0f;
    g_Zc2Cameras[0].mode = 0;
    g_Zc2Cameras[0].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);
    if (flags & 0x4000) {
        g_Zc2Cameras[0].vpX = 128;
        g_Zc2Cameras[0].vpY = 16;
        g_Zc2Cameras[0].vpW = 384;
        g_Zc2Cameras[0].vpH = 448;
    } else {
        g_Zc2Cameras[0].vpX = 0;
        g_Zc2Cameras[0].vpY = 0;
        g_Zc2Cameras[0].vpW = 640;
        g_Zc2Cameras[0].vpH = 480;
    }

    g_Zc2Cameras[2].pos = Zc2Vec3(0.0f, 0.0f, 1000.0f);
    g_Zc2Cameras[2].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[2].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[2].fovOrFar = 0.52359879f;
    g_Zc2Cameras[2].vpMinZ = 0.0f;
    g_Zc2Cameras[2].vpMaxZ = 1.0f;
    g_Zc2Cameras[2].mode = 0;
    g_Zc2Cameras[2].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);
    if (flags & 0x4000) {
        g_Zc2Cameras[2].vpX = 109;
        g_Zc2Cameras[2].vpY = 0;
        g_Zc2Cameras[2].vpW = 422;
        g_Zc2Cameras[2].vpH = 480;
    } else {
        g_Zc2Cameras[2].vpX = 0;
        g_Zc2Cameras[2].vpY = 0;
        g_Zc2Cameras[2].vpW = 512;
        g_Zc2Cameras[2].vpH = 512;
    }
}

void Zc2InitMenuCameras() {
    g_Zc2Cameras[1].pos = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[1].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[1].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[1].fovOrFar = 0.52359879f;
    g_Zc2Cameras[1].vpX = 0;
    g_Zc2Cameras[1].vpY = 0;
    g_Zc2Cameras[1].vpW = 640;
    g_Zc2Cameras[1].vpH = 480;
    g_Zc2Cameras[1].vpMinZ = 0.0f;
    g_Zc2Cameras[1].vpMaxZ = 1.0f;
    g_Zc2Cameras[1].mode = 1;
    g_Zc2Cameras[1].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);

    g_Zc2Cameras[0].pos = Zc2Vec3(0.0f, 0.0f, 1000.0f);
    g_Zc2Cameras[0].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[0].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[0].fovOrFar = 0.52359879f;
    g_Zc2Cameras[0].vpX = 0;
    g_Zc2Cameras[0].vpY = 0;
    g_Zc2Cameras[0].vpW = 640;
    g_Zc2Cameras[0].vpH = 480;
    g_Zc2Cameras[0].vpMinZ = 0.0f;
    g_Zc2Cameras[0].vpMaxZ = 1.0f;
    g_Zc2Cameras[0].mode = 0;
    g_Zc2Cameras[0].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);

    g_Zc2Cameras[2].pos = Zc2Vec3(0.0f, 0.0f, 1000.0f);
    g_Zc2Cameras[2].at = Zc2Vec3(0.0f, 0.0f, 0.0f);
    g_Zc2Cameras[2].up = Zc2Vec3(0.0f, 1.0f, 0.0f);
    g_Zc2Cameras[2].fovOrFar = 0.52359879f;
    g_Zc2Cameras[2].vpX = 0;
    g_Zc2Cameras[2].vpY = 0;
    g_Zc2Cameras[2].vpW = 640;
    g_Zc2Cameras[2].vpH = 480;
    g_Zc2Cameras[2].vpMinZ = 0.0f;
    g_Zc2Cameras[2].vpMaxZ = 1.0f;
    g_Zc2Cameras[2].mode = 0;
    g_Zc2Cameras[2].offset = Zc2Vec3(0.0f, 0.0f, 0.0f);
}

int __stdcall Zc2InsertHighScore(Zc2ScoreFile *sf) {
    int rank = 0;
    for (; rank < 10; ++rank) {
        if (sf->entries[g_Zc2Difficulty][rank].score <= g_Zc2Score) {
            break;
        }
    }
    if (rank >= 10) {
        return -1;
    }
    for (int i = 9; i > rank; --i) {
        sf->entries[g_Zc2Difficulty][i] = sf->entries[g_Zc2Difficulty][i - 1];
    }
    sf->entries[g_Zc2Difficulty][rank].score = g_Zc2Score;
    sf->entries[g_Zc2Difficulty][rank].cont = g_Zc2Continues;
    sf->entries[g_Zc2Difficulty][rank].stage = g_Zc2StageNum;
    _time64((__time64_t *)&sf->entries[g_Zc2Difficulty][rank].timeSec);
    strcpy(sf->entries[g_Zc2Difficulty][rank].name, "        ");
    float ratio = (float)(g_Zc2Fps->totalTime / g_Zc2Fps->frameCount);
    sf->entries[g_Zc2Difficulty][rank].slowRate = (float)(100.0 - ratio * 100.0);
    return rank;
}

Zc2IceShard *Zc2Player::SpawnIceShard(const Zc2Vec3 *pos, float a, float b, int timerVal, int c, int d) {
    Zc2IceShard *shard = &shards[nextShardIdx];
    nextShardIdx = (nextShardIdx + 1) % 2000;
    for (int i = 0; i < 2000; ++i) {
        if ((shard->flags & 1) == 0) {
            memset(shard, 0, sizeof(*shard));
            shard->flags |= 3;
            memset(&shard->pos, 0, 0x34);
            shard->pos = *pos;
            shard->f00 = a;
            shard->f04 = b;
            if ((shard->timer.flags & 1) == 0) {
                shard->timer.curF = 0.0f;
                shard->timer.cur = 0;
                shard->timer.prev = -999999;
                shard->timer.speedPtr = &g_Zc2GameSpeed;
                shard->timer.flags |= 1;
            }
            shard->timer.cur = timerVal;
            shard->timer.curF = (float)timerVal;
            shard->timer.prev = timerVal - 1;
            shard->f64 = 0;
            shard->f60 = c;
            shard->f68 = 999999;
            shard->f6c = 4;
            shard->f70 = d;
            return shard;
        }
        if (nextShardIdx == 0) {
            shard = &shards[0];
        } else {
            ++shard;
        }
        nextShardIdx = (nextShardIdx + 1) % 2000;
    }
    return shard;
}

int __stdcall Zc2CreateMainWindow(HINSTANCE hInst) {
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.hCursor = LoadCursorA(0, IDC_ARROW);
    wc.hInstance = hInst;
    wc.lpfnWndProc = Zc2MainWndProc;
    g_Zc2WindowActive = 1;
    g_Zc2WindowClosing = 0;
    wc.lpszClassName = "BASE";
    RegisterClassA(&wc);

    unsigned int flags = g_Zc2SupFlags;
    flags = (flags & ~0xcu) | (((unsigned int)g_Zc2WinMode << 2) & 0xcu);
    g_Zc2IsWindowed = ((flags & 0xcu) != 0) ? 1 : 0;
    if (g_Zc2CfgByte4e == 0 && g_Zc2CfgByte53 == 2) {
        flags |= 0x10u;
    } else {
        flags &= ~0x10u;
    }
    g_Zc2SupFlags = flags;
    g_Zc2InputCfg[1] = 15;
    g_Zc2InputCfg[2] = 15;
    g_Zc2InputCfg[3] = 0;
    g_Zc2InputCfg[4] = 12;
    g_Zc2InputCfg[5] = 12;
    g_Zc2InputCfg[6] = 0;
    g_Zc2InputCfg[7] = 12;
    g_Zc2InputCfg[8] = 12;
    g_Zc2InputCfg[9] = 0;
    g_Zc2InputCfg[10] = 8;
    g_Zc2InputCfg[11] = 8;
    g_Zc2InputCfg[12] = 0;
    g_Zc2InputCfg[0] = 0;

    HWND hwnd;
    if (g_Zc2IsWindowed == 0) {
        hwnd = CreateWindowExA(
            0, "BASE",
            "\x97\x64\x90\xb8\x91\xe5\x90\xed\x91\x88\x81\x40\x81\x60 \x93\x8c\x95\xfb\x8e\x4f\x8c\x8e\x90\xb8. ver 1.00a",
            WS_POPUP | WS_VISIBLE,
            0, 0, 640, 480,
            0, 0, hInst, 0
        );
    } else {
        int w, h;
        unsigned int mode = (flags >> 2) & 3;
        if (mode == 3) {
            w = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 1280;
            h = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 960;
        } else if (mode == 2) {
            w = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 960;
            h = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 720;
        } else {
            w = GetSystemMetrics(SM_CXFIXEDFRAME) * 2 + 640;
            h = GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + 480;
        }
        h += GetSystemMetrics(SM_CYCAPTION);
        hwnd = CreateWindowExA(
            0, "BASE",
            "\x97\x64\x90\xb8\x91\xe5\x90\xed\x91\x88\x81\x40\x81\x60 \x93\x8c\x95\xfb\x8e\x4f\x8c\x8e\x90\xb8. ver 1.00a",
            0x100b0000u,
            g_Zc2WinX, g_Zc2WinY, w, h,
            0, 0, hInst, 0
        );
    }
    g_Zc2MainHwnd = hwnd;
    GetWindowRect(hwnd, &g_Zc2WinRect);
    g_Zc2SupHwnd = g_Zc2MainHwnd;
    if (g_Zc2MainHwnd == 0) {
        return 1;
    }
    SendMessageA(g_Zc2MainHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
    Sleep(16);
    SendMessageA(g_Zc2MainHwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    return 0;
}

INT_PTR CALLBACK Zc2ConfigDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    (void)lParam;
    switch (msg) {
    case WM_INITDIALOG:
        if (g_Zc2CfgFlags & 0x100) {
            SendMessageA(GetDlgItem(hDlg, 0xca), BM_SETCHECK, 1, 0);
        }
        switch (g_Zc2WinMode) {
        case 0:
            SendMessageA(GetDlgItem(hDlg, 0xcc), BM_SETCHECK, 1, 0);
            break;
        case 1:
            SendMessageA(GetDlgItem(hDlg, 0xcd), BM_SETCHECK, 1, 0);
            break;
        case 2:
            SendMessageA(GetDlgItem(hDlg, 0xce), BM_SETCHECK, 1, 0);
            break;
        case 3:
            SendMessageA(GetDlgItem(hDlg, 0xcf), BM_SETCHECK, 1, 0);
            break;
        }
        g_Zc2SupFlags = (g_Zc2SupFlags & ~0x20u) | 0x40u;
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == 0xd0) {
            if (IsDlgButtonChecked(hDlg, 0xca) == 1) {
                g_Zc2CfgFlags |= 0x100;
            } else {
                g_Zc2CfgFlags &= ~0x100u;
            }
            if (IsDlgButtonChecked(hDlg, 0xcc) == 1) {
                g_Zc2WinMode = 0;
            } else if (IsDlgButtonChecked(hDlg, 0xcd) == 1) {
                g_Zc2WinMode = 1;
            } else if (IsDlgButtonChecked(hDlg, 0xce) == 1) {
                g_Zc2WinMode = 2;
            } else if (IsDlgButtonChecked(hDlg, 0xcf) == 1) {
                g_Zc2WinMode = 3;
            } else {
                g_Zc2WinMode = 0;
            }
            g_Zc2SupFlags &= ~0x60u;
            EndDialog(hDlg, 6);
            goto close_dialog;
        }
        return 0;
    case WM_CLOSE:
    close_dialog:
        if ((g_Zc2SupFlags & 0x60u) == 0x40u) {
            g_Zc2SupFlags = (g_Zc2SupFlags & ~0x40u) | 0x20u;
        }
        EndDialog(hDlg, 6);
        return 1;
    }
    return 0;
}

int Zc2Fade::OnCalc() {
    if (mode == 0) {
        if (duration != 0 && timerCur <= duration) {
            alpha = (int)((timerCurF * 128.0) / duration);
        }
    } else {
        if (timerCur <= 8) {
            alpha = 128 - (int)(timerCurF * 128.0 * 0.125);
        } else {
            return 7;
        }
    }
    timerPrev = timerCur;
    if (*speedPtr > 0.99 && *speedPtr < 1.01f) {
        timerCur++;
        timerCurF = (float)(timerCurF + 1.0);
    } else {
        timerCurF += *speedPtr;
        timerCur = (int)timerCurF;
    }
    return 1;
}

void Zc2InitQuadVertexBuffer() {
    Zc2AnmMgrVB *mgr = g_Zc2AnmMgr;
    mgr->quadVerts[0].pos.x = -128.0f;
    mgr->quadVerts[1].pos.x = 128.0f;
    mgr->quadVerts[2].pos.x = -128.0f;
    mgr->quadVerts[3].pos.x = 128.0f;
    mgr->quadVerts[0].pos.y = -128.0f;
    mgr->quadVerts[1].pos.y = -128.0f;
    mgr->quadVerts[2].pos.y = 128.0f;
    mgr->quadVerts[3].pos.y = 128.0f;
    mgr->quadVerts[0].pos.z = 0.0f;
    mgr->quadVerts[1].pos.z = 0.0f;
    mgr->quadVerts[2].pos.z = 0.0f;
    mgr->quadVerts[3].pos.z = 0.0f;
    mgr->quadVerts[0].u = 0.0f;
    mgr->quadVerts[1].u = 1.0f;
    mgr->quadVerts[2].u = 0.0f;
    mgr->quadVerts[3].u = 1.0f;
    mgr->quadVerts[0].v = 0.0f;
    mgr->quadVerts[1].v = 0.0f;
    mgr->quadVerts[2].v = 1.0f;
    mgr->quadVerts[3].v = 1.0f;

    for (int i = 0; i < 4; ++i) {
        g_Zc2QuadVertsGlobal[i].pos = mgr->quadVerts[i].pos;
    }
    for (int i = 0; i < 4; ++i) {
        g_Zc2QuadVertsGlobal[i].u = mgr->quadVerts[i].u;
        g_Zc2QuadVertsGlobal[i].v = mgr->quadVerts[i].v;
    }

    g_Zc2D3DDev->vtbl->CreateVertexBuffer(g_Zc2D3DDev, 0x50, 0, 0x102, 1, &mgr->quadVb, 0);
    void *locked = 0;
    mgr->quadVb->vtbl->Lock(mgr->quadVb, 0, 0, &locked, 0);
    memcpy(locked, mgr->quadVerts, sizeof(mgr->quadVerts));
    mgr->quadVb->vtbl->Unlock(mgr->quadVb);
    g_Zc2D3DDev->vtbl->SetStreamSource(g_Zc2D3DDev, 0, mgr->quadVb, 0, 0x14);
}

void __stdcall Zc2ClearTextureSurface(Zc2D3DTex **ppTex) {
    Zc2D3DSurf *surf = 0;
    Zc2SurfDesc desc;
    Zc2LockedRect locked;
    (*ppTex)->vtbl->GetSurfaceLevel(*ppTex, 0, &surf);
    surf->vtbl->GetDesc(surf, &desc);
    surf->vtbl->LockRect(surf, &locked, 0, 0);
    switch (desc.format) {
    case 20:
    case 22:
    case 23:
        memset(locked.pBits, 0, desc.height * locked.pitch);
        break;
    case 21: {
        unsigned char *p = (unsigned char *)locked.pBits;
        for (unsigned int i = 0; i < desc.width * desc.height; ++i) {
            p[3] = 0xff;
            p[2] = 0;
            p[1] = 0;
            p[0] = 0;
            p += 4;
        }
        break;
    }
    case 26: {
        unsigned short *p = (unsigned short *)locked.pBits;
        for (unsigned int i = 0; i < desc.width * desc.height; ++i) {
            *p = 0xf000;
            ++p;
        }
        break;
    }
    }
    surf->vtbl->UnlockRect(surf);
    if (surf != 0) {
        surf->vtbl->Release(surf);
    }
}

int Zc2EclManager::LoadFileData(Zc2EclHeader *hdr) {
    files[fileCount] = hdr;
    if (files[fileCount]->magic != 0x54504353u || files[fileCount]->version != 1) {
        files[fileCount] = 0;
        return -1;
    }
    unsigned char *base = (unsigned char *)files[fileCount];
    unsigned int *offsets = (unsigned int *)(base + files[fileCount]->includeCount + 0x24);
    unsigned short count = files[fileCount]->subCount;
    totalSubs += count;
    char *names = (char *)(offsets + count);
    Zc2EclSubEntry *oldSubs = subs;
    if (oldSubs == 0) {
        subs = (Zc2EclSubEntry *)malloc(totalSubs * sizeof(Zc2EclSubEntry));
        for (int i = 0; i < totalSubs; ++i) {
            subs[i].code = (unsigned char *)files[fileCount] + *offsets;
            subs[i].name = names;
            names += strlen(names) + 1;
            ++offsets;
        }
    } else {
        subs = (Zc2EclSubEntry *)malloc(totalSubs * sizeof(Zc2EclSubEntry));
        int curCount = totalSubs - files[fileCount]->subCount;
        memcpy(subs, oldSubs, curCount * sizeof(Zc2EclSubEntry));
        if (oldSubs != 0) {
            free(oldSubs);
        }
        for (int i = 0; i < files[fileCount]->subCount; ++i) {
            int pos = 0;
            for (; pos < curCount; ++pos) {
                if (strcmp(names, subs[pos].name) <= 0) {
                    break;
                }
            }
            for (int j = totalSubs - 1; j > pos; --j) {
                subs[j] = subs[j - 1];
            }
            subs[pos].code = (unsigned char *)files[fileCount] + *offsets;
            subs[pos].name = names;
            names += strlen(names) + 1;
            ++curCount;
            ++offsets;
        }
    }
    int idx = fileCount;
    fileCount = idx + 1;
    if (files[idx]->includeCount != 0) {
        LoadIncludes((unsigned char *)files[idx] + 0x24);
    }
    return idx;
}

} // namespace th128
