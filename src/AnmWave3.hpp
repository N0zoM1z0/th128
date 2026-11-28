#ifndef TH128_ANM_WAVE3_HPP
#define TH128_ANM_WAVE3_HPP

namespace th128 {

struct Aw3Vec2 {
    float x;
    float y;
};

struct Aw3Vec3 {
    float x;
    float y;
    float z;

    Aw3Vec3() {}
    Aw3Vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
};

struct Aw3Vertex {
    float x;
    float y;
    float z;
    float rhw;
    unsigned int diffuse;

    void SetPolar(float angle, float radius);
};

struct Aw3Effect10Vertex {
    float x;
    float y;
    float z;
    float rhw;
    unsigned int diffuse;
    float u;
    float v;

    void SetPolar(float angle, float radius);
};

struct Aw3Effect10Data {
    Aw3Effect10Vertex verts[33];
    float radii[33];
    float speeds[33];
    float scrollU;
    float scrollV;
    int pad4ac;
};

struct Aw3D3dDevice {
    virtual long __stdcall QueryInterface(const void *, void **) = 0;
    virtual unsigned long __stdcall AddRef() = 0;
    virtual unsigned long __stdcall Release() = 0;
    virtual void __stdcall M03() = 0;
    virtual void __stdcall M04() = 0;
    virtual void __stdcall M05() = 0;
    virtual void __stdcall M06() = 0;
    virtual void __stdcall M07() = 0;
    virtual void __stdcall M08() = 0;
    virtual void __stdcall M09() = 0;
    virtual void __stdcall M10() = 0;
    virtual void __stdcall M11() = 0;
    virtual void __stdcall M12() = 0;
    virtual void __stdcall M13() = 0;
    virtual void __stdcall M14() = 0;
    virtual void __stdcall M15() = 0;
    virtual void __stdcall M16() = 0;
    virtual void __stdcall M17() = 0;
    virtual void __stdcall M18() = 0;
    virtual void __stdcall M19() = 0;
    virtual void __stdcall M20() = 0;
    virtual void __stdcall M21() = 0;
    virtual void __stdcall M22() = 0;
    virtual void __stdcall M23() = 0;
    virtual void __stdcall M24() = 0;
    virtual void __stdcall M25() = 0;
    virtual void __stdcall M26() = 0;
    virtual void __stdcall M27() = 0;
    virtual void __stdcall M28() = 0;
    virtual void __stdcall M29() = 0;
    virtual void __stdcall M30() = 0;
    virtual void __stdcall M31() = 0;
    virtual void __stdcall M32() = 0;
    virtual void __stdcall M33() = 0;
    virtual void __stdcall M34() = 0;
    virtual void __stdcall M35() = 0;
    virtual void __stdcall M36() = 0;
    virtual void __stdcall M37() = 0;
    virtual void __stdcall M38() = 0;
    virtual void __stdcall M39() = 0;
    virtual void __stdcall M40() = 0;
    virtual void __stdcall M41() = 0;
    virtual void __stdcall M42() = 0;
    virtual void __stdcall M43() = 0;
    virtual void __stdcall M44() = 0;
    virtual void __stdcall M45() = 0;
    virtual void __stdcall M46() = 0;
    virtual void __stdcall M47() = 0;
    virtual void __stdcall M48() = 0;
    virtual void __stdcall M49() = 0;
    virtual void __stdcall M50() = 0;
    virtual void __stdcall M51() = 0;
    virtual void __stdcall M52() = 0;
    virtual void __stdcall M53() = 0;
    virtual void __stdcall M54() = 0;
    virtual void __stdcall M55() = 0;
    virtual void __stdcall M56() = 0;
    virtual long __stdcall SetRenderState(unsigned int state, unsigned int value) = 0;
    virtual void __stdcall M58() = 0;
    virtual void __stdcall M59() = 0;
    virtual void __stdcall M60() = 0;
    virtual void __stdcall M61() = 0;
    virtual void __stdcall M62() = 0;
    virtual void __stdcall M63() = 0;
    virtual void __stdcall M64() = 0;
    virtual void __stdcall M65() = 0;
    virtual void __stdcall M66() = 0;
    virtual long __stdcall SetTextureStageState(unsigned int stage, unsigned int type, unsigned int value) = 0;
    virtual void __stdcall M68() = 0;
    virtual void __stdcall M69() = 0;
    virtual void __stdcall M70() = 0;
    virtual void __stdcall M71() = 0;
    virtual void __stdcall M72() = 0;
    virtual void __stdcall M73() = 0;
    virtual void __stdcall M74() = 0;
    virtual void __stdcall M75() = 0;
    virtual void __stdcall M76() = 0;
    virtual void __stdcall M77() = 0;
    virtual void __stdcall M78() = 0;
    virtual void __stdcall M79() = 0;
    virtual void __stdcall M80() = 0;
    virtual void __stdcall M81() = 0;
    virtual void __stdcall M82() = 0;
    virtual long __stdcall DrawPrimitiveUP(unsigned int primType, unsigned int primCount, const void *data, unsigned int stride) = 0;
    virtual void __stdcall M84() = 0;
    virtual void __stdcall M85() = 0;
    virtual void __stdcall M86() = 0;
    virtual void __stdcall M87() = 0;
    virtual void __stdcall M88() = 0;
    virtual long __stdcall SetFVF(unsigned int fvf) = 0;
};

struct Aw3AnmManager {
    unsigned char pad00[0xac];
    int drawCallCount;
    unsigned char padb0[0x4e95de];
    unsigned char colorOpState;
    unsigned char pad4e968f[4];
    unsigned char alphaOpState;
    unsigned char pad4e9694[0x3d0068];
    Aw3Vertex *vertexCursor;

    void FlushAnmSpriteBatch();
    int DrawFilledCircle(float cx, float cy, float radius, float startAngle, int segments, unsigned int innerColor, unsigned int outerColor);
    int DrawPolyLine(const Aw3Vec2 *points, const unsigned int *colors, int count);
    int DrawCircleOutline(float cx, float cy, float radius, float startAngle, int segments, unsigned int color);
    int DrawRing(float cx, float cy, float radius, float thickness, float startAngle, int segments, unsigned int color);
};

struct Aw3AnmVm {
    unsigned char pad00[0x3f0];
    unsigned int color;
    unsigned char pad3f4[0x64];
    Aw3Vec3 pos;
    Aw3Vec3 offset;
    Aw3Vec3 shake;
    unsigned char pad47c[0x30];
    Aw3Effect10Data *effectData;
    unsigned char pad4b0[0x0c];
    int (__fastcall *onTick)(Aw3AnmVm *vm);
    void (__fastcall *onDraw)(Aw3AnmVm *vm);

    int StartEffect10();
};

void GetAnmVmDrawPosition(const Aw3AnmVm *vm, Aw3Vec3 *outPos);

struct Aw3Rng {
    unsigned int GetU32();
    float GetFloatSigned() {
        return (float)((double)GetU32() * 4.656612873077393e-10 - 1.0);
    }
};

void PolarToVec2(Aw3Vertex *out, float angle, float radius);
float __stdcall AddAngles(float a, float b);
int __fastcall UpdateAnmVmEffect10(Aw3AnmVm *vm);
void __fastcall DrawAnmVmEffect10(Aw3AnmVm *vm);

extern Aw3D3dDevice *g_Aw3D3dDevice;
extern Aw3AnmManager *g_Aw3AnmManager;
extern Aw3Rng g_Aw3Rng;

} // namespace th128

#endif
