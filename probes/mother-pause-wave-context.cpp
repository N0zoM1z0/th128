#include "../src/MotherPauseWave.hpp"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

namespace th128 {

MpwChain *g_MpwChain = NULL;
int g_MpwAudioMode = 0;
MpwThreadInf g_MpwSupThread;
void *g_MpwCfgAlloc = NULL;
MpwSmallObj *g_MpwGlobalObj = NULL;
MpwAnmManager *g_MpwAnmMgr = NULL;
void *g_MpwSoundMgrPtr = NULL;
HGDIOBJ g_MpwGdiObjs[8];
MpwArchive g_MpwArchive;
MpwBufObj *g_MpwBufObjs[3];
int g_MpwSceneCur = 0;
int g_MpwSceneNext = 0;
int g_MpwSceneFlag = 0;
MpwSupervisor g_MpwSupervisor;
MpwConfig g_MpwConfig;
char g_MpwAppDataDir[0x1000];
unsigned int g_MpwInputKeys[4];
unsigned short g_MpwInputKey10 = 0;
int g_MpwSupFlag3e4 = 0;
MpwErrorCtx g_MpwErrorCtx;
MpwAnmId g_MpwLoadVmIds[3];
MpwStreamSound *g_MpwStreamSound = NULL;
int g_MpwStageNum = 0;
int g_MpwStageNumCopy = 0;
int g_MpwRouteIdx = 0;
void *g_MpwStageTablePtr = NULL;
unsigned char g_MpwStageTableA[0x118];
unsigned char g_MpwStageTableB[0x118];
MpwScoreFile *g_MpwScoreFile = NULL;
static volatile int g_Sink = 0;

__declspec(noinline) void MpwThreadInf::Stop() {
    if (handle4 == NULL) return;
    DWORD t = GetTickCount();
    g_Sink += (int)handle4 + (int)t;
    handle4 = NULL;
}

__declspec(noinline) MpwSmallObj::~MpwSmallObj() {
    g_Sink += dummy;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

__declspec(noinline) void MpwConfig::ResetDefaults() {
    if (version0 == 0) return;
    DWORD t = GetTickCount();
    g_Sink += (int)version0 + (int)t;
    version0 = 0;
}

__declspec(noinline) void MpwArchive::Close() {
    if (dummy == 0) return;
    DWORD t = GetTickCount();
    g_Sink += dummy + (int)t;
    dummy = 0;
}

__declspec(noinline) HRESULT MpwStreamSound::SetVolume(int vol) {
    g_Sink += (int)this + vol;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
    if (buffers4 != NULL && buffers4[0] != NULL) {
        return buffers4[0]->Stop();
    }
    return 0;
}

__declspec(noinline) MpwAnmId MpwAnmResource::CreateVm(const void *pos, int script, int layer, unsigned int flags) {
    int a = dummy;
    int b = pos ? *(const int *)pos : 0;
    DWORD t1 = GetTickCount();
    int c = script + layer + (int)flags;
    DWORD t2 = GetTickCount();
    int d = (a ^ b) + (int)t1 + (int)t2 + c;
    g_Sink += d + a + b + c;
    MpwAnmId r;
    r.id = (unsigned int)d;
    return r;
}

struct MpwAnmNode {
    MpwAnmVm *vm;
    MpwAnmNode *next;
    MpwAnmNode *prev;
};

struct MpwListMgr {
    unsigned char pad00[0x8b9704];
    MpwAnmNode *list0Head;
    MpwAnmNode *list0Tail;
    MpwAnmNode *list1Head;
};

__declspec(noinline) MpwAnmVm *MpwAnmManager::GetVmById(MpwAnmId id) {
    if (id.id == 0) {
        return NULL;
    }
    MpwListMgr *m = (MpwListMgr *)this;
    for (MpwAnmNode *n = m->list0Head; n != NULL; n = n->next) {
        if (*(unsigned int *)n->vm == id.id) {
            return n->vm;
        }
    }
    for (MpwAnmNode *n = m->list1Head; n != NULL; n = n->next) {
        if (*(unsigned int *)n->vm == id.id) {
            return n->vm;
        }
    }
    return NULL;
}

__declspec(noinline) void MpwMenuCursor::Move(int delta) {
    g_Sink += cur + delta;
    DWORD t = GetTickCount();
    g_Sink += (int)t;
}

static __declspec(noinline) void MpwLockCs(int idx);
static __declspec(noinline) void MpwUnlockCs(int idx);

__declspec(noinline) const char *__cdecl MpwErrorCtx::Log(const char *fmt, ...) {
    char local[0x2000];
    va_list args;
    MpwLockCs(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    MpwUnlockCs(3);
    return fmt;
}

__declspec(noinline) const char *__cdecl MpwErrorCtx::Fatal(const char *fmt, ...) {
    char local[0x200];
    va_list args;
    MpwLockCs(3);
    va_start(args, fmt);
    vsprintf(local, fmt, args);
    va_end(args);
    size_t len = strlen(local);
    if (cursor + len < buffer + 0x1fff) {
        strcpy(cursor, local);
        cursor += len;
        *cursor = '\0';
    }
    fatal = 1;
    MpwUnlockCs(3);
    return fmt;
}

static __declspec(noinline) void MpwLockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx + (int)t;
}

static __declspec(noinline) void MpwUnlockCs(int idx) {
    DWORD t = GetTickCount();
    g_Sink += idx + (int)t + 7;
}

__declspec(noinline) int MpwChain::RegisterCalc(MpwChainElem *elem, int priority) {
    int result = 0;
    MpwChain *chain = g_MpwChain;
    if (elem->onRegC != NULL) {
        result = elem->onRegC(elem->arg20);
        elem->onRegC = NULL;
    }
    MpwLockCs(0);
    MpwChainNode *cursor = &chain->calcChain.node14;
    elem->priority0 = priority;
    while (cursor->next != NULL) {
        if (cursor->next->owner->priority0 >= priority) {
            break;
        }
        cursor = cursor->next;
    }
    MpwChainNode *node = &elem->node14;
    if (cursor->next != NULL) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    MpwUnlockCs(0);
    return result;
}

__declspec(noinline) int MpwChain::RegisterDraw(MpwChainElem *elem, int priority) {
    int result = 0;
    MpwChain *chain = g_MpwChain;
    if (elem->onRegC != NULL) {
        result = elem->onRegC(elem->arg20);
        elem->onRegC = NULL;
    }
    MpwLockCs(0);
    MpwChainNode *cursor = &chain->drawChain.node14;
    elem->priority0 = priority;
    while (cursor->next != NULL) {
        if (cursor->next->owner->priority0 >= priority) {
            break;
        }
        cursor = cursor->next;
    }
    MpwChainNode *node = &elem->node14;
    if (cursor->next != NULL) {
        node->next = cursor->next;
        cursor->next->prev = node;
    }
    cursor->next = node;
    node->prev = cursor;
    MpwUnlockCs(0);
    return result;
}

__declspec(noinline) int MpwWaitSoundReady() {
    DWORD t = GetTickCount();
    g_Sink += (int)t;
    return g_Sink & 1;
}

__declspec(noinline) void MpwCleanupSub() {
    DWORD t = GetTickCount();
    g_Sink += (int)t + 2;
}

__declspec(noinline) void __stdcall MpwEnqueueBgmCommand(void *mgr, int cmd, int arg, const char *label) {
    DWORD t1 = GetTickCount();
    g_Sink += (int)mgr + cmd + arg + (int)label + (int)t1;
    DWORD t2 = GetTickCount();
    g_Sink += (int)t1 * 3 + (int)t2 + (int)label;
}

__declspec(noinline) void MpwDestroyTextImage() {
    DWORD t = GetTickCount();
    g_Sink += (int)t + 3;
}

static __declspec(noinline) void __cdecl MpwDebugPrint(const char *fmt, ...) {
    DWORD t = GetTickCount();
    g_Sink += (int)fmt + (int)t;
}

__declspec(noinline) int __stdcall MpwWriteDataToFile(const char *path, const void *data, unsigned int size) {
    MpwLockCs(2);
    HANDLE handle = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        LPSTR message;
        FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPSTR)&message, 0, NULL);
        MpwDebugPrint("error : %s write error %s\r\n", path, message);
        LocalFree(message);
        MpwUnlockCs(2);
        return -1;
    }
    DWORD written;
    WriteFile(handle, data, size, &written, NULL);
    if (size != written) {
        CloseHandle(handle);
        MpwDebugPrint("error : %s write error\r\n", path);
        MpwUnlockCs(2);
        return -2;
    }
    CloseHandle(handle);
    MpwDebugPrint("%s write ...\r\n", path);
    MpwUnlockCs(2);
    return 0;
}

} // namespace th128

typedef int (th128::MpwSupervisor::*MpwLoadConfigFn)();
volatile MpwLoadConfigFn g_MpwLoadConfigPtr = &th128::MpwSupervisor::LoadConfig;

extern "C" int __stdcall MotherPauseWaveProbe(int selector, int arg) {
    using namespace th128;
    if (selector == 1) return ((MpwMother *)arg)->Init();
    if (selector == 2) return ((MpwSupervisor *)arg)->Shutdown();
    if (selector == 3) return MpwRegisterSupervisorChains();
    if (selector == 4) return ((MpwSupervisor *)arg)->LoadConfig();
    if (selector == 5) { ((MpwSupervisor *)arg)->SpawnLoadingVmsA((const MpwVec3 *)(arg + 4)); return g_Sink; }
    if (selector == 6) { ((MpwSupervisor *)arg)->SpawnLoadingVmsB((const MpwVec3 *)(arg + 4)); return g_Sink; }
    if (selector == 7) { MpwUpdateBgmFade(); return g_Sink; }
    if (selector == 8) { ((MpwPauseMenu *)arg)->InitScoreNameEntry(); return g_Sink; }
    if (selector == 9) {
        MpwAnmId id = ((MpwAnmResource *)arg)->CreateVm((const void *)arg, arg, arg + 1, (unsigned int)(arg + 2));
        MpwWriteDataToFile((const char *)arg, (const void *)(arg + 4), (unsigned int)arg);
        return (int)id.id + g_MpwChain->RegisterCalc((MpwChainElem *)arg, arg) + g_MpwChain->RegisterDraw((MpwChainElem *)arg, arg);
    }
    if (selector == 10) return g_MpwSupervisor.LoadConfig();
    return 0;
}
