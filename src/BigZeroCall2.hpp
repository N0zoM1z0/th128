#ifndef TH128_BIG_ZERO_CALL2_HPP
#define TH128_BIG_ZERO_CALL2_HPP

struct Bz2LzssNode {
    int parent;
    int left;
    int right;
};

extern Bz2LzssNode g_Bz2LzssTree[8193];
extern unsigned char g_Bz2LzssWindow[8192];

int __stdcall Bz2LzssAddString(int newNode, int *matchPos);

#endif
