#include "ZeroCallBatchA.hpp"
#include <new>

volatile int g_ZcaSink = 0;

typedef void (__stdcall *ZcaDecodeFn)(int *, int *);
volatile ZcaDecodeFn g_ZcaDecodePtr = &ZcaDecodeGauge;
typedef void (__stdcall *ZcaAdvFn)(int);
volatile ZcaAdvFn g_ZcaAdvPtr = &ZcaAdvanceStage;
typedef int (ZcaReplayBuf::*ZcaChkFn)(int);
volatile ZcaChkFn g_ZcaChkPtr = &ZcaReplayBuf::ChecksumBytes;
typedef int (__fastcall *ZcaB16Fn)(ZcaBullet *, int);
volatile ZcaB16Fn g_ZcaB16Ptr = &ZcaGetBulletParam16;
typedef int (__fastcall *ZcaB32Fn)(ZcaLaser *, int);
volatile ZcaB32Fn g_ZcaB32Ptr = &ZcaGetBulletParam32;

extern "C" int __stdcall ZeroCallBatchAProbe(void **args, int mode) {
    int a = mode, b = mode + 1;
    int r = 0;
    g_ZcaTextPixels = (unsigned short *)args[0];
    OutputDebugStringA((const char *)&g_ZcaTextBytes);
    if (mode & 1) g_ZcaDecodePtr(&a, &b);
    if (mode & 2) ZcaLzssReplaceNode(a, b);
    if (mode & 4) ZcaCopyMeshStrip((ZcaMesh *)args[0]);
    if (mode & 8) g_ZcaAdvPtr(a);
    if (mode & 16) ZcaResetEnemySub((ZcaEnemySub *)args[1]);
    if (mode & 32) r += ZcaChecksumDwords((const int *)args[2], a);
    if (mode & 64) ZcaUnlinkEnemy((ZcaEnemy *)args[3]);
    if (mode & 128) r += (int)ZcaReadLine((char *)args[4], &a, (char *)args[5]);
    if (mode & 256) r += (((ZcaReplayBuf *)args[6])->*g_ZcaChkPtr)(a);
    if (mode & 512) ZcaFillText4444((unsigned int)a);
    if (mode & 1024) ZcaResetTwoLists((ZcaTwoLists *)args[7]);
    if (mode & 2048) ((ZcaDropOwner *)args[8])->SetDrop(a, b, a + 1, a + 2, a + 3);
    if (mode & 4096) r += g_ZcaB16Ptr((ZcaBullet *)args[9], a);
    if (mode & 8192) r += ZcaIsAnmLoaded((ZcaAnmVm *)args[10]);
    if (mode & 16384) r += g_ZcaB32Ptr((ZcaLaser *)args[11], a);
    if (mode & 32768) r += (int)(new ZcaReplaySlot[4]);
    return r + a + b + g_ZcaSink;
}
