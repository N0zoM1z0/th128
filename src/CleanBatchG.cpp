#include "CleanBatchG.hpp"
#include <stdio.h>
#include <string.h>

CbgGuiObj *g_CbgGui;
int g_CbgMotivation;
int g_CbgMotivationLock;
int g_CbgFreezeGauge;

// 1. 0x00434bc0 (233B)
int __stdcall CbgCheckReplayVersion(CbgReplayMgr *mgr, const char *ver, int expA, int expB) {
    char *p = mgr->text9a4;
    if (p == 0) {
        return 0;
    }
    int rem = mgr->textLen9a0;
    if (strncmp(ver, "debug", 5) == 0) {
        return 0;
    }
    if (strcmp("0100a", "debug") == 0) {
        return 0;
    }
    while (rem != 0) {
        if (strncmp(ver, p, 5) == 0) {
            int a, b;
            p += 6;
            sscanf(p, "%d %d", &a, &b);
            if (a == expA && b == expB) {
                return 0;
            }
        }
        char *old = p;
        p = strchr(p, '\n') + 1;
        rem += (int)(old - p);
    }
    return -1;
}

// 2. 0x00457c40 (223B)
int __stdcall CbgFindBgmEntry(const char *path, CbgSoundMgr *mgr) {
    char buf[0x80];
    int i = 0;
    const char *p = strrchr(path, '/');
    if (p == 0) {
        p = strrchr(path, '\\');
    }
    if (p == 0) {
        strcpy(buf, path);
    } else {
        strcpy(buf, p + 1);
    }
    CbgBgmEntry *entries = mgr->entries;
    while (entries[i].name[0] != 0) {
        if (strcmp(entries[i].name, buf) == 0) {
            break;
        }
        ++i;
    }
    if (entries[i].name[0] != 0) {
        return i;
    }
    return 0;
}

// 3. 0x00459820 (212B)
void __stdcall CbgLoadMapFile(CbgMapOwner *owner, const char *path) {
    char buf[0x104];
    int sz;
    sprintf(buf, "%s", path);
    int len = strlen(buf);
    buf[len - 3] = 'm';
    buf[len - 2] = 'a';
    buf[len - 1] = 'p';
    if (CbgFileExists(buf)) {
        owner->map134 = (unsigned int *)CbgOpenFile(buf, &sz, 1);
        for (int i = 0; i < owner->count114 + owner->count110; ++i) {
            owner->map134[i] += (unsigned int)owner->map134;
        }
    } else {
        owner->map134 = 0;
    }
}

// 4. 0x004033d0 (185B)
CbgStageObj::CbgStageObj() {
    memset(this, 0, sizeof(CbgStageObj));
    flags |= 2;
}

// 5. 0x004200e0 (184B)
CbgGuiObj::CbgGuiObj() {
    memset(this, 0, sizeof(CbgGuiObj));
    flags |= 2;
    g_CbgGui = this;
}

// 6. 0x00427260 (152B)
void __stdcall CbgAddMotivation(int delta) {
    int old = g_CbgMotivation;
    if (g_CbgMotivationLock != 0) {
        return;
    }
    int cur = old + delta;
    g_CbgMotivation = cur;
    if (cur / 10000 > old / 10000) {
        CbgPlaySound(0x12);
        CbgShowPopup(g_CbgGui, 4, 4, 0);
        cur = g_CbgMotivation;
    }
    if (cur > 100000) {
        g_CbgMotivation = 100000;
    } else if (cur < 0) {
        g_CbgMotivationLock = 1;
    }
}

// 7. 0x00427300 (150B)
void __stdcall CbgAddFreezeGauge(int delta) {
    int old = g_CbgFreezeGauge;
    int cur = old + delta;
    g_CbgFreezeGauge = cur;
    if (cur > 30000) {
        cur = 30000;
        g_CbgFreezeGauge = 30000;
    }
    if (cur / 10000 > old / 10000) {
        CbgPlaySound(0x48);
        CbgShowPopup(g_CbgGui, 2, 2, 0);
        CbgUpdateFreezeUi(g_CbgGui);
    } else if (cur < 0) {
        g_CbgFreezeGauge = 0;
    }
}
