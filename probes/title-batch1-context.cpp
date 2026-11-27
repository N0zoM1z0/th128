#include "TitleBatch1.hpp"
#include <string.h>
#include <windows.h>

static CRITICAL_SECTION g_SoundCs;
static int g_SoundFlags;
static unsigned char g_SoundLockCount;

Tb1AnmVm *__stdcall Tb1GetVmById(Tb1AnmMgr *mgr, Tb1AnmId id) {
    if (id.id == 0) {
        return 0;
    }
    for (Tb1AnmNode *n = mgr->list0.next; n != 0; n = n->next) {
        if (n->vm->id.id == id.id) {
            return n->vm;
        }
    }
    for (Tb1AnmNode *n = mgr->list1.next; n != 0; n = n->next) {
        if (n->vm->id.id == id.id) {
            return n->vm;
        }
    }
    return 0;
}

int Tb1AnmVm::BindAnmFile(Tb1AnmFile *file, int spriteId) {
    if (file->f108 == 0 || file->f124 != 0) {
        return -1;
    }
    scriptId = (unsigned short)spriteId;
    const float *entry = (const float *)(file->sprites + spriteId * 72);
    float *dst = (float *)pad01c;
    dst[0] = entry[0];
    dst[1] = entry[0];
    dst[2] = entry[1];
    dst[3] = entry[1];
    dst[4] = entry[2];
    dst[5] = entry[2];
    dst[6] = entry[3];
    dst[7] = entry[3];
    dst[8] = 0.0f;
    dst[9] = 0.0f;
    dst[10] = 1.0f;
    dst[11] = 1.0f;
    return 0;
}

void Tb1SoundMgr::PostCommand(const char *cmd, int cmdId, int arg) {
    if (g_SoundFlags & 0x8000) {
        EnterCriticalSection(&g_SoundCs);
        ++g_SoundLockCount;
    }
    for (int i = 0; i < 31; ++i) {
        if (slots[i].cmdId == 0) {
            slots[i].cmdId = cmdId;
            slots[i].arg = arg;
            strcpy(slots[i].name, cmd);
            slots[i].f08 = 0;
            break;
        }
    }
    if (g_SoundFlags & 0x8000) {
        LeaveCriticalSection(&g_SoundCs);
        --g_SoundLockCount;
    }
}

static Tb1SoundMgr g_OtherSoundMgr;

extern "C" int __stdcall TitleBatch1Probe(Tb1AnmMgr *mgr, Tb1MenuObj *obj, int mode) {
    int sum = 0;
    if (mode & 1) {
        g_Tb1AnmMgr = mgr;
        g_Tb1Cfg.bgmVol = (signed char)mode;
        g_Tb1Cfg.seVol = (signed char)(mode >> 8);
        g_OtherSoundMgr.PostCommand((const char *)&g_Tb1Cfg, mode, 1);
    }
    if (mode & 2) {
        Tb1UpdateOptionsVolume(obj);
    }
    if (mode & 4) {
        Tb1UpdateOptionsVolume(obj);
    }
    if (mode & 8) {
        Tb1AnmVm *v = Tb1GetVmById(mgr, obj->rootId);
        if (v != 0) {
            sum += v->BindAnmFile(v->anmFile, mode);
            sum += v->scriptId;
        }
        g_Tb1SoundMgr.PostCommand("Play", mode, 1);
    }
    if (mode & 16) {
        g_Tb1SoundMgr.PostCommand("Stop", mode + 1, 2);
        g_OtherSoundMgr.PostCommand("Fade", mode + 2, 3);
    }
    return sum + g_Tb1BgmVol + g_Tb1SeVol + g_Tb1SeDb;
}
