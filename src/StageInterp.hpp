#pragma once

#include <stddef.h>

struct StageTimerMini
{
    int previous;                               // +0x00
    int current;                                // +0x04
    float currentF;                             // +0x08
    float *gameSpeed;                           // +0x0C
    unsigned int flags;                         // +0x10

    void Increment();                           // 0x00402AC0 (external)

    inline void EnsureInit();
    inline void SetInt(int v);
    inline void ResetToMinus1();
};

struct StageVec3
{
    float x;
    float y;
    float z;

    StageVec3 operator+(const StageVec3 &rhs) const
    {
        StageVec3 r;
        r.x = x + rhs.x;
        r.y = y + rhs.y;
        r.z = z + rhs.z;
        return r;
    }

    StageVec3 operator-(const StageVec3 &rhs) const
    {
        StageVec3 r;
        r.x = x - rhs.x;
        r.y = y - rhs.y;
        r.z = z - rhs.z;
        return r;
    }

    StageVec3 operator*(float s) const;
    StageVec3 operator/(float s) const;
    float LengthSq() const;
};

struct StageFog
{
    float f0;                                   // +0x00
    float f1;                                   // +0x04
    float r;                                    // +0x08
    float g;                                    // +0x0C
    float b;                                    // +0x10
    float a;                                    // +0x14
    unsigned char cr;                           // +0x18
    unsigned char cg;                           // +0x19
    unsigned char cb;                           // +0x1A
    unsigned char ca;                           // +0x1B

    StageFog() {}
    StageFog(float p0, float p1, float pr, float pg, float pb, float pa);

    void PackRgba();
    StageFog operator-(const StageFog &rhs) const;
    StageFog operator*(float s) const;
    StageFog operator+(const StageFog &rhs) const;
};
typedef char StageFogSizeCheck[(sizeof(StageFog) == 0x1C) ? 1 : -1];

template <typename T>
struct StageInterp
{
    T start;
    T end;
    T vel0;
    T vel1;
    StageTimerMini timer;
    int duration;
    int mode;

    T Step();
    float EvalT();
    void SetMode(int m) { mode = m; }
    void SetStart(const T &v) { start = v; }
    void SetEnd(const T &v) { end = v; }
    void SetVel0(const T &v) { vel0 = v; }
    void SetVel1(const T &v) { vel1 = v; }
    void SetDuration(int d) { duration = d; }
    int GetDuration() const { return duration; }
    void ClearDuration() { duration = 0; }
    void ResetTimer();
};

struct StageScriptCmd
{
    int time;                                   // +0x00
    short opcode;                               // +0x04
    short size;                                 // +0x06
    int arg0;                                   // +0x08
};

struct StageAnmNodeMini
{
    struct StageAnmVmMini *vm;                  // +0x00
    StageAnmNodeMini *next;                     // +0x04
};

struct StageAnmVmMini
{
    unsigned char pad00[0x14];
    StageAnmNodeMini *childHead;                // +0x14
    void *parentPtr;                            // +0x18
    unsigned char pad1C[0x424 - 0x1C];
    int field424;                               // +0x424
    unsigned char pad428[0x4B0 - 0x428];
    unsigned int flags4B0;                      // +0x4B0

    void ClearFlag0();
    bool HasField424();
    int UpdateFrame();                          // 0x00459D90 (external)
};

struct StageAnmManagerMini
{
    unsigned char pad00[0x4E9690];
    unsigned char dirtyFlag;                    // +0x4E9690

    StageAnmVmMini *GetVmById(int id);          // 0x00467420 (external)
};

struct StageBgStateMini
{
    unsigned char pad00[0x1C];
    StageScriptCmd *scriptStart;                // +0x1C
    unsigned char pad20[0x38 - 0x20];
    StageTimerMini scriptTimer;                 // +0x38..+0x4B
    StageScriptCmd *scriptCur;                  // +0x4C
    unsigned char pad50[0x2910 - 0x50];
    int flag2910;                               // +0x2910
    int pad2914;
    int flag2918;                               // +0x2918
    StageVec3 vec291C;                          // +0x291C
    StageVec3 vec2928;                          // +0x2928
    unsigned char pad2934[0x37F8 - 0x2934];
    unsigned int flags37F8;                     // +0x37F8
    StageTimerMini fadeTimer;                   // +0x37FC..+0x380F
    unsigned char pad3810[0x3834 - 0x3810];
    int vmId3834;                               // +0x3834
    int vmId3838;                               // +0x3838

    static void JumpToLabel(int labelId);
    void SetVecPair(const StageVec3 *a, const StageVec3 *b);
    void SetFlag2910();
    void StopBgVms();
    static void StartFadeOut60();
};

extern StageBgStateMini *g_StageBgState;        // 0x004B8928
extern StageAnmManagerMini *g_StageAnmManager;  // 0x004D2E50
extern float g_StageDefaultTimeScale;           // 0x004B6F58

float __stdcall StageEaseTransition(float t, int mode);   // 0x0046B190 (external)
float __stdcall StageNormalizeAngle(float a);             // 0x0046A700 (external)

float __stdcall StageCosf(float a);
float * __stdcall StageNormalizeAngleInto(float *out, float a);
void StageSetAnmDirty();
int __stdcall StageUpdateVm(StageAnmVmMini *vm);
void *StageMalloc(size_t n);
