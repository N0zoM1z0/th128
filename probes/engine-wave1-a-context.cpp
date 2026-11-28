#include "EngineWave1A.hpp"

namespace th128 {

Ew1aAnmManager *g_Ew1aAnmManager = NULL;

static volatile int g_Sink = 0;

__declspec(noinline) Ew1aAnmVm::Ew1aAnmVm() {
    int x = g_Sink;
    g_Sink = x + *(int *)this;
}

__declspec(noinline) void Ew1aAnmVm::ResetForReuse() {
    int a = *(int *)this;
    Sleep(1);
    g_Sink += a + (int)this;
}

__declspec(noinline) unsigned short Ew1aRng::GetU16() {
    int a = *(int *)this;
    Sleep(1);
    g_Sink += a + (int)this;
    return (unsigned short)g_Sink;
}

static void *volatile g_Keep[] = {
    (void *)&Ew1aSumBytes,
};

} // namespace th128

extern "C" int __stdcall EngineWave1AProbe(int a, int b) {
    using namespace th128;
    int r = (int)g_Keep[0];

    Ew1aAnmVm *vmArr = new Ew1aAnmVm[a & 3];
    r += (int)vmArr;
    r += (int)Ew1aAllocateAnmVm();
    r += (int)Ew1aAllocateAnmVm();

    Ew1aChain *c1 = new Ew1aChain();
    Ew1aChain *c2 = new Ew1aChain();
    r += (int)c1 + (int)c2;

    r += (int)Ew1aDecryptBlocks((unsigned char *)(int)a, b, (unsigned char)a, (unsigned char)b, a + 4, b + 8);
    r += (int)Ew1aDecryptBlocks((unsigned char *)(int)(a + 1), b + 1, (unsigned char)(a + 2), (unsigned char)(b + 2), a + 8, b + 16);

    r += (int)Ew1aEncryptBlocks((unsigned char *)(int)a, b, (unsigned char)a, (unsigned char)b, a + 4, b + 8);
    r += (int)Ew1aEncryptBlocks((unsigned char *)(int)(a + 1), b + 1, (unsigned char)(a + 2), (unsigned char)(b + 2), a + 8, b + 16);

    Ew1aRng *r1 = (Ew1aRng *)(int)a;
    Ew1aRng *r2 = (Ew1aRng *)(int)(a + 4);
    r += (int)r1->GetU32AndSkip2();
    r += (int)r2->GetU32AndSkip2();

    r += (int)Ew1aReadShiftJisLine((const char *)(int)a, (char *)(int)b, a + 16);
    r += (int)Ew1aReadShiftJisLine((const char *)(int)(a + 1), (char *)(int)(b + 1), b + 16);

    r += Ew1aSumBytes(b, (const unsigned char *)(int)a);
    r += Ew1aSumBytes(b + 1, (const unsigned char *)(int)(a + 1));
    return r;
}
