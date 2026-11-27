#include "InterpBatch1.hpp"

namespace th128 {

inline void IbTimer::SetInt(int v) {
    if ((flags & 1u) == 0) {
        curF = 0.0f;
        cur = 0;
        prev = -999999;
        scale = &g_IbDefaultTimeScale;
        flags |= 1u;
    }
    cur = v;
    prev = v - 1;
    curF = (float)v;
}

template <typename T>
T IbInterp<T>::Step() {
    if (duration > 0) {
        timer.Increment();
        if (timer.cur >= duration) {
            timer.SetInt(duration);
            duration = 0;
            if (mode != 7) {
                return end;
            }
            return start;
        }
    }

    if (mode == 7) {
        T cur = start;
        start = end + cur;
        return start;
    }
    if (mode == 17) {
        T cur = start;
        start = vel1 + cur;
        vel1 = end + vel1;
        return start;
    }

    float t = timer.curF / (float)duration;
    if (mode == 8) {
        float w0 = (float)((t - 1.0) * (t - 1.0) * (2.0 * t + 1.0));
        float w1 = (float)(t * t * (3.0 - 2.0 * t));
        float w2 = (float)((1.0 - t) * (1.0 - t) * t);
        float w3 = (float)((t - 1.0) * t * t);
        return start * w0 + end * w1 + vel0 * w2 + vel1 * w3;
    }

    unsigned int m = (unsigned int)(mode - 1);
    float eased = (m <= 15u) ? IbEaseTransition(m, t) : t;
    T delta = (end - start) * eased;
    delta = delta + start;
    return delta;
}

template struct IbInterp<IbVec3>;
template struct IbInterp<IbVec2>;
template struct IbInterp<IbInt3>;
template struct IbInterp<float>;
template struct IbInterp<int>;

} // namespace th128
