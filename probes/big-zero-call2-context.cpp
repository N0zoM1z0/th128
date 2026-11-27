#include "BigZeroCall2.hpp"

extern "C" int __stdcall BigZeroCall2Probe(int newNode, int *matchPos, int mode) {
    if (mode & 1) {
        g_Bz2LzssTree[8192].right = newNode;
        g_Bz2LzssWindow[newNode & 0x1fff] = (unsigned char)mode;
    }
    int r = Bz2LzssAddString(newNode, matchPos);
    if (mode & 2) {
        r += Bz2LzssAddString(newNode + 1, matchPos);
    }
    return r + *matchPos;
}
