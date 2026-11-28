#pragma once

#include <windows.h>

namespace th128 {

struct SfwRectF {
    float left;
    float top;
    float right;
    float bottom;
};

struct SfwVec3 {
    float x;
    float y;
    float z;

    SfwVec3() {}
    SfwVec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct SfwVertex {
    SfwVec3 pos;
    float rhw;
    DWORD color;
};

struct SfwViewport {
    DWORD X;
    DWORD Y;
    DWORD Width;
    DWORD Height;
    float MinZ;
    float MaxZ;
};

struct SfwD3dDevice {
    virtual HRESULT __stdcall M00() = 0;
    virtual HRESULT __stdcall M01() = 0;
    virtual HRESULT __stdcall M02() = 0;
    virtual HRESULT __stdcall M03() = 0;
    virtual HRESULT __stdcall M04() = 0;
    virtual HRESULT __stdcall M05() = 0;
    virtual HRESULT __stdcall M06() = 0;
    virtual HRESULT __stdcall M07() = 0;
    virtual HRESULT __stdcall M08() = 0;
    virtual HRESULT __stdcall M09() = 0;
    virtual HRESULT __stdcall M10() = 0;
    virtual HRESULT __stdcall M11() = 0;
    virtual HRESULT __stdcall M12() = 0;
    virtual HRESULT __stdcall M13() = 0;
    virtual HRESULT __stdcall M14() = 0;
    virtual HRESULT __stdcall M15() = 0;
    virtual HRESULT __stdcall M16() = 0;
    virtual HRESULT __stdcall M17() = 0;
    virtual HRESULT __stdcall M18() = 0;
    virtual HRESULT __stdcall M19() = 0;
    virtual HRESULT __stdcall M20() = 0;
    virtual HRESULT __stdcall M21() = 0;
    virtual HRESULT __stdcall M22() = 0;
    virtual HRESULT __stdcall M23() = 0;
    virtual HRESULT __stdcall M24() = 0;
    virtual HRESULT __stdcall M25() = 0;
    virtual HRESULT __stdcall M26() = 0;
    virtual HRESULT __stdcall M27() = 0;
    virtual HRESULT __stdcall M28() = 0;
    virtual HRESULT __stdcall M29() = 0;
    virtual HRESULT __stdcall M30() = 0;
    virtual HRESULT __stdcall M31() = 0;
    virtual HRESULT __stdcall M32() = 0;
    virtual HRESULT __stdcall M33() = 0;
    virtual HRESULT __stdcall M34() = 0;
    virtual HRESULT __stdcall M35() = 0;
    virtual HRESULT __stdcall M36() = 0;
    virtual HRESULT __stdcall M37() = 0;
    virtual HRESULT __stdcall M38() = 0;
    virtual HRESULT __stdcall M39() = 0;
    virtual HRESULT __stdcall M40() = 0;
    virtual HRESULT __stdcall M41() = 0;
    virtual HRESULT __stdcall M42() = 0;
    virtual HRESULT __stdcall M43() = 0;
    virtual HRESULT __stdcall M44() = 0;
    virtual HRESULT __stdcall M45() = 0;
    virtual HRESULT __stdcall M46() = 0;
    virtual HRESULT __stdcall SetViewport(const SfwViewport *vp) = 0;
    virtual HRESULT __stdcall M48() = 0;
    virtual HRESULT __stdcall M49() = 0;
    virtual HRESULT __stdcall M50() = 0;
    virtual HRESULT __stdcall M51() = 0;
    virtual HRESULT __stdcall M52() = 0;
    virtual HRESULT __stdcall M53() = 0;
    virtual HRESULT __stdcall M54() = 0;
    virtual HRESULT __stdcall M55() = 0;
    virtual HRESULT __stdcall M56() = 0;
    virtual HRESULT __stdcall SetRenderState(DWORD state, DWORD value) = 0;
    virtual HRESULT __stdcall M58() = 0;
    virtual HRESULT __stdcall M59() = 0;
    virtual HRESULT __stdcall M60() = 0;
    virtual HRESULT __stdcall M61() = 0;
    virtual HRESULT __stdcall M62() = 0;
    virtual HRESULT __stdcall M63() = 0;
    virtual HRESULT __stdcall M64() = 0;
    virtual HRESULT __stdcall M65() = 0;
    virtual HRESULT __stdcall M66() = 0;
    virtual HRESULT __stdcall SetTextureStageState(DWORD stage, DWORD type, DWORD value) = 0;
    virtual HRESULT __stdcall M68() = 0;
    virtual HRESULT __stdcall M69() = 0;
    virtual HRESULT __stdcall M70() = 0;
    virtual HRESULT __stdcall M71() = 0;
    virtual HRESULT __stdcall M72() = 0;
    virtual HRESULT __stdcall M73() = 0;
    virtual HRESULT __stdcall M74() = 0;
    virtual HRESULT __stdcall M75() = 0;
    virtual HRESULT __stdcall M76() = 0;
    virtual HRESULT __stdcall M77() = 0;
    virtual HRESULT __stdcall M78() = 0;
    virtual HRESULT __stdcall M79() = 0;
    virtual HRESULT __stdcall M80() = 0;
    virtual HRESULT __stdcall M81() = 0;
    virtual HRESULT __stdcall M82() = 0;
    virtual HRESULT __stdcall DrawPrimitiveUP(DWORD type, UINT count, const void *data, UINT stride) = 0;
    virtual HRESULT __stdcall M84() = 0;
    virtual HRESULT __stdcall M85() = 0;
    virtual HRESULT __stdcall M86() = 0;
    virtual HRESULT __stdcall M87() = 0;
    virtual HRESULT __stdcall M88() = 0;
    virtual HRESULT __stdcall SetFVF(DWORD fvf) = 0;
};

struct SfwAnmManager {
    unsigned char pad00[0x4e9688];
    DWORD f4e9688;
    unsigned char f4e968c;
    unsigned char f4e968d;
    unsigned char f4e968e;
    unsigned char f4e968f;
    unsigned char pad4e9690[4];
    DWORD f4e9694;
};

struct SfwTimer {
    int previous;
    int current;
    float currentF;
    const float *gameSpeed;
    unsigned int control;

    void Increment();
    void Reset() {
        if ((control & 1u) == 0) {
            currentF = 0.0f;
            current = 0;
            previous = -999999;
            gameSpeed = &g_SfwGameSpeed;
            control |= 1u;
        }
        currentF = 0.0f;
        current = 0;
        previous = -1;
    }

    static float g_SfwGameSpeed;
};

struct SfwScreenEffect {
    unsigned char pad00[0x18];
    int alpha18;
    int duration1c;
    int param20;
    int param24;
    int param28;
    unsigned char pad2c[0x04];
    SfwTimer timer;

    int OnDrawFade();
    int OnCalcFlash();
    int OnCalcShakeA();
    int OnCalcShakeB();
};

struct SfwRng {
    unsigned int GetU32();
};

struct SfwStage {
    unsigned char pad00[0x60];
    union {
        unsigned int flags60;
        struct {
            unsigned int b0 : 1;
            unsigned int b1 : 1;
            unsigned int b2 : 1;
        } bits;
    };
};

extern SfwAnmManager *g_SfwAnmManager;
extern SfwD3dDevice *g_SfwD3dDevice;
extern SfwViewport g_SfwViewport;
extern int g_SfwPaused;
extern SfwRng g_SfwRng;
extern float g_SfwShakeX;
extern float g_SfwShakeY;
extern SfwStage *g_SfwStage;

void SfwFlushAnmSpriteBatch(SfwAnmManager *mgr);
void SfwDrawSolidRect(const SfwRectF *rect, DWORD color);
void __stdcall SfwDrawGradientRect(const SfwRectF *rect, DWORD c0, DWORD c1, DWORD c2, DWORD c3);

} // namespace th128
