#include "GuiDialogHelpers.hpp"

int GdGuiFlags::GetFlag4() { return (flags5A04 >> 4) & 1U; }
int GdEnemyFlags::IsNotFlag5() { return (~(flags50AC >> 5)) & 1U; }
int GdEnemyFlags::IsNotFlag0() { return (~flags50AC) & 1U; }

int GdEnemyFlags::IsFlag0Or4OrPos506C()
{
    if ((flags50AC & 0x11U) != 0 || field506C > 0)
    {
        return 1;
    }
    return 0;
}

int GdEnemyMgrMini::GetFlag0() { return g_GdEnemyMgr->flags7C & 1U; }
int Gd4B8934::GetField94() { return g_Gd4B8934->field94; }

void __stdcall Gd4B8934::DecodeTime(int *outSec, int *outCs)
{
    Gd4B8934 *m = g_Gd4B8934;
    int v = m->encodedTime;
    int cs = (v % 100 + 67) % 100;
    int sec = ((v / 100) % 1000 + 934) % 1000;
    if (v / 100000 - 22 != sec + cs)
    {
        *outSec = 999;
        *outCs = 99;
        return;
    }
    int v2 = m->encodedTime;
    *outSec = ((v2 / 100) % 1000 + 934) % 1000;
    *outCs = (v2 % 100 + 67) % 100;
}

void *GdTable66C::GetEntry(int idx)
{
    return entries[idx];
}

int GdGuiObj::CalcHook() { return GdGuiCalc(this); }

GdGuiObj *GdGuiObj::DestroySelf()
{
    GdGuiDtor(this);
    operator delete(this);
    return this;
}

void GdGuiObj::DestroyGlobal()
{
    GdGuiObj *g = g_GdGuiObj;
    if (g != NULL)
    {
        GdGuiDtor(g);
        operator delete(g);
    }
}

char *GdDecodeDialogString(const char *src)
{
    char *dst = g_GdDecodeBuf;
    unsigned char key = 0x77;
    unsigned char step = 7;
    unsigned char c;
    do
    {
        c = (unsigned char)(*src++ ^ key);
        key += step;
        *dst++ = (char)c;
        step += 0x10;
    } while (c != 0);
    return g_GdDecodeBuf;
}

void GdStageOwner::Set7C(int v) { field7C = v; }
int GdGet4B4D3C() { return g_Gd4B4D3C; }
int GdGet4B4D50Bit2() { return (g_Gd4B4D50 >> 2) & 1U; }
void GdSet4B4D50Bit2() { g_Gd4B4D50 |= 4U; }
void GdStageOwner::Set6C(int v) { field6C = v; }

void GdStageOwner::SetStageIdx(int idx)
{
    g_GdCurStageInfo = &g_GdStageTable[idx];
    field54 = idx;
    field58 = idx;
}

int GdGetPowerFloor10()
{
    int p = g_GdPower / 100;
    return p - p % 10;
}

int GdReplayMgr::GetSubByte0ABit0()
{
    return g_GdReplayMgr->sub1C->byte0A & 1U;
}

int GdStageOwner::GetFlag4() { return (flags90 >> 4) & 1U; }
int GdCall473090(const char *a, const char *b) { return GdStrCmp473090(a, b); }
unsigned int __stdcall GdTestInput210(unsigned int mask) { return g_GdInput210 & mask; }

int GdGetHold18C(int bit)
{
    if ((g_GdInput210 & (1U << bit)) != 0)
    {
        return g_GdHold18C[bit];
    }
    return 0;
}

int GdPair::Diff() { return b != a; }
unsigned int __stdcall GdTestInput21C(unsigned int mask) { return g_GdInput21C & mask; }
int *GdOff40::Get() { return &f; }
int GdGet3F0::Get() { return v; }
float GdGetFltA0::Get() { return v; }
int GdGetB0::Get() { return v; }
int GdGetA4::Get() { return v; }
int GdGet4B4D2C() { return g_Gd4B4D2C; }
int *GdOff464::Get() { return &f; }
int GdStageOwner::GetFlag3() { return (flags90 >> 3) & 1U; }
int GdGet4D33CC() { return g_Gd4D33CC; }
int GdStageOwner::GetFlag5() { return (flags90 >> 5) & 1U; }
int GdGet4B4D04() { return g_Gd4B4D04; }
int GdGet4B4D08() { return g_Gd4B4D08; }
int GdGet4B4CFC() { return g_Gd4B4CFC; }
int GdGet4B4D00() { return g_Gd4B4D00; }
void GdNoOp425BD0() {}
void GdSet0::Set(int x) { v = x; }
int GdGet4B4CC0() { return g_Gd4B4CC0; }
int GdGet04::Get() { return v; }

void GdVmState::SetState3()
{
    if (onStateChange != NULL)
    {
        onStateChange(this, 3);
    }
    state3F8 = 3;
}

void GdVmState::SetStatePlus7(int s)
{
    short ns = (short)(s + 7);
    if (onStateChange != NULL)
    {
        onStateChange(this, ns);
    }
    state3F8 = ns;
}

int Gd4B8924::GetFlag3() { return (g_Gd4B8924->flags37F8 >> 3) & 1U; }
