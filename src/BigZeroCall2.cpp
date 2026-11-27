#include "BigZeroCall2.hpp"

Bz2LzssNode g_Bz2LzssTree[8193];
unsigned char g_Bz2LzssWindow[8192];

int __stdcall Bz2LzssAddString(int newNode, int *matchPos) {
    if (newNode == 0) {
        return 0;
    }
    int testNode = g_Bz2LzssTree[8192].right;
    int matchLen = 0;
    for (;;) {
        int i;
        int delta;
        for (i = 0; i < 18; ++i) {
            delta = (int)g_Bz2LzssWindow[(newNode + i) & 0x1fff] -
                    (int)g_Bz2LzssWindow[(testNode + i) & 0x1fff];
            if (delta != 0) {
                break;
            }
        }
        if (i >= matchLen) {
            matchLen = i;
            *matchPos = testNode;
            if (matchLen >= 18) {
                int p = g_Bz2LzssTree[testNode].parent;
                if (g_Bz2LzssTree[p].left == testNode) {
                    g_Bz2LzssTree[p].left = newNode;
                } else {
                    g_Bz2LzssTree[p].right = newNode;
                }
                g_Bz2LzssTree[newNode] = g_Bz2LzssTree[testNode];
                g_Bz2LzssTree[g_Bz2LzssTree[newNode].left].parent = newNode;
                g_Bz2LzssTree[g_Bz2LzssTree[newNode].right].parent = newNode;
                g_Bz2LzssTree[testNode].parent = 0;
                return matchLen;
            }
        }
        int *child;
        if (delta >= 0) {
            child = &g_Bz2LzssTree[testNode].right;
        } else {
            child = &g_Bz2LzssTree[testNode].left;
        }
        if (*child == 0) {
            *child = newNode;
            g_Bz2LzssTree[newNode].parent = testNode;
            g_Bz2LzssTree[newNode].right = 0;
            g_Bz2LzssTree[newNode].left = 0;
            return matchLen;
        }
        testNode = *child;
    }
}
