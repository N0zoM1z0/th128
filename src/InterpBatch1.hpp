#ifndef TH128_INTERP_BATCH1_HPP
#define TH128_INTERP_BATCH1_HPP

namespace th128 {

struct IbVec2 {
    float x;
    float y;

    IbVec2() {}
    IbVec2(float fx, float fy) : x(fx), y(fy) {}

    inline IbVec2 operator+(const IbVec2& rhs) const {
        return IbVec2(x + rhs.x, y + rhs.y);
    }
    inline IbVec2 operator-(const IbVec2& rhs) const {
        return IbVec2(x - rhs.x, y - rhs.y);
    }
    inline IbVec2 operator*(float s) const {
        return IbVec2(x * s, y * s);
    }
};

inline IbVec2 operator*(float s, const IbVec2& v) {
    return IbVec2(s * v.x, s * v.y);
}

struct IbVec3 {
    float x;
    float y;
    float z;

    IbVec3() {}
    IbVec3(float fx, float fy, float fz) : x(fx), y(fy), z(fz) {}

    inline IbVec3 operator+(const IbVec3& rhs) const {
        return IbVec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    inline IbVec3 operator-(const IbVec3& rhs) const {
        return IbVec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    inline IbVec3 operator*(float s) const {
        return IbVec3(x * s, y * s, z * s);
    }
};

inline IbVec3 operator*(float s, const IbVec3& v) {
    return IbVec3(s * v.x, s * v.y, s * v.z);
}

struct IbInt3 {
    int x;
    int y;
    int z;

    IbInt3() {}
    IbInt3(int ix, int iy, int iz) : x(ix), y(iy), z(iz) {}

    inline IbInt3 operator+(const IbInt3& rhs) const {
        return IbInt3(x + rhs.x, y + rhs.y, z + rhs.z);
    }
    inline IbInt3& operator+=(const IbInt3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    inline IbInt3 operator-(const IbInt3& rhs) const {
        return IbInt3(x - rhs.x, y - rhs.y, z - rhs.z);
    }
    inline IbInt3 operator*(float s) const {
        return IbInt3((int)((float)x * s), (int)((float)y * s), (int)((float)z * s));
    }
};

struct IbTimer {
    int prev;
    int cur;
    float curF;
    float* scale;
    unsigned int flags;

    void Increment();

    inline void SetInt(int v);
};

extern float g_IbDefaultTimeScale;

float __stdcall IbEaseTransition(unsigned int mode, float t);

template <typename T>
struct IbInterp {
    T start;
    T end;
    T vel0;
    T vel1;
    IbTimer timer;
    int duration;
    int mode;

    T Step();
};

} // namespace th128

#endif
