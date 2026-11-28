#ifndef TH128_ITEM_WAVE1_HPP
#define TH128_ITEM_WAVE1_HPP

#include <windows.h>

namespace th128 {

struct Iw1Vec3 {
    float x;
    float y;
    float z;
};

struct Iw1Timer {
    int prev;
    int current;
    float currentF;
    float *timeScalePtr;
    unsigned int control;

    void Reset() {
        if ((control & 1u) == 0) {
            currentF = 0.0f;
            current = 0;
            prev = -999999;
            timeScalePtr = &g_Iw1TimeScale;
            control |= 1u;
        }
        currentF = 0.0f;
        current = 0;
        prev = -1;
    }

    static float g_Iw1TimeScale;
};

struct Iw1ChainElem {
    void *prev;
    unsigned int flags;
    void *callback;
    void *onRegister;
    void *onCut;
    Iw1ChainElem *self;
    void *next;
    void *listPtr;
    void *arg;

    Iw1ChainElem() {
        flags &= ~1u;
        callback = 0;
        onRegister = 0;
        onCut = 0;
        prev = 0;
        self = this;
        next = 0;
        listPtr = 0;
    }
};

struct Iw1ChainManager {
    void __fastcall Cut(void *elem);
};

struct Iw1Globals {
    int pad00;
    int score;
    int itemCount;
    unsigned char pad0c[0x40];
    int modeFlag;
    unsigned char pad50[0x20];
    int powerMeter;
    int maxItemCount;
    int itemDivisor;
    unsigned char pad7c[0x24];
    float freezeRate;
    unsigned char pada4[0x04];
    int motivation;
    int gameOverFlag;
    int padb0;
    int perfectFreeze;
};

struct Iw1Gui {
    void ShowPopup(int kind, int a, int b);
    void UpdateMeterAnms();
};

struct Iw1HelpInf {
    unsigned char pad00[0x08];
    Iw1ChainElem *calcChain;
    Iw1ChainElem *drawChain;
    int state;
    Iw1Timer timer;

    static int Initialize(Iw1HelpInf *self);
};

struct Iw1Item {
    unsigned char pad00[0x9d4];
    Iw1Vec3 pos;
    unsigned char pad9e0[0x60];

    Iw1Item();
    ~Iw1Item();
    void CollectPoint();
};

struct Iw1ItemManager {
    unsigned char pad00[0x08];
    void *calcChain;
    void *drawChain;
    int pad10;
    Iw1Item items[0x2cc];

    ~Iw1ItemManager();
};

int GetFreezeLevel(const Iw1Globals *g);
void __stdcall AddMotivation(int delta);
void __stdcall AddPerfectFreeze(int delta);
void PlaySfx(int id);
void PlaySfxWithPan(int id, float pan);
void AddScorePopup(const Iw1Vec3 *pos, int value, int color);
void TriggerItemBonus(void *player);
void RegisterCalcChain(Iw1ChainElem *elem, int prio);
void RegisterDrawChain(Iw1ChainElem *elem, int prio);
void StartLoaderThread(void (*fn)());
void HelpLoaderEntry();
int __cdecl HelpCalcCb(void *arg);
int __cdecl HelpDrawCb(void *arg);

extern const int g_Iw1FreezeThresholds[2][15];
extern Iw1Globals g_Iw1Globals;
extern Iw1Gui *g_Iw1Gui;
extern void *g_Iw1Player;
extern Iw1ItemManager *g_Iw1ItemManager;
extern Iw1ChainManager *g_Iw1ChainManager;
extern unsigned int g_Iw1SupervisorFlags;
extern CRITICAL_SECTION g_Iw1ChainCritSec;
extern unsigned char g_Iw1ChainCritSecDepth;

} // namespace th128

#endif
