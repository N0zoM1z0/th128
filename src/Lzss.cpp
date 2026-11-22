#include "PbgArchive.hpp"

#include <stdlib.h>

namespace Pbg
{
// Shared 8 KB history; each call restarts its write cursor at 1 and keeps
// the old contents.
unsigned char g_LzssHistory[0x2000];

static __forceinline unsigned int ReadBits(unsigned int bit, const unsigned char *packed, int packedSize,
                                           const unsigned char *&cursor, int &cached,
                                           unsigned char &mask)
{
    unsigned int result = 0;
    while (bit != 0)
    {
        if (mask == 0x80)
        {
            cached = *cursor;
            if (cursor - packed >= packedSize)
            {
                cached = 0;
            }
            else
            {
                ++cursor;
            }
        }
        if (cached & mask)
        {
            result |= bit;
        }
        bit >>= 1;
        mask >>= 1;
        if (mask == 0)
        {
            mask = 0x80;
        }
    }
    return result;
}

unsigned char *__stdcall DecompressLzss(const unsigned char *packed, int packedSize,
                                        unsigned char *destination, unsigned int size)
{
    unsigned char mask = 0x80;
    int cached = 0;
    if (destination == NULL)
    {
        destination = (unsigned char *)malloc(size);
        if (destination == NULL)
        {
            return NULL;
        }
    }
    const unsigned char *cursor = packed;
    unsigned char *output = destination;
    unsigned int position = 1;
    for (;;)
    {
        if (mask == 0x80)
        {
            cached = *cursor;
            if (cursor - packed >= packedSize)
            {
                cached = 0;
            }
            else
            {
                ++cursor;
            }
        }
        int flag = mask & cached;
        mask >>= 1;
        if (mask == 0)
        {
            mask = 0x80;
        }
        if (flag != 0)
        {
            unsigned char value = (unsigned char)ReadBits(0x80, packed, packedSize, cursor, cached, mask);
            *output++ = value;
            g_LzssHistory[position] = value;
            position = (position + 1) & 0x1FFF;
        }
        else
        {
            unsigned int reference = ReadBits(0x1000, packed, packedSize, cursor, cached, mask);
            if (reference == 0)
            {
                break;
            }
            int length = ReadBits(8, packed, packedSize, cursor, cached, mask);
            for (int i = 0; i <= length + 2; ++i)
            {
                unsigned char value = g_LzssHistory[(reference + i) & 0x1FFF];
                *output++ = value;
                g_LzssHistory[position] = value;
                position = (position + 1) & 0x1FFF;
            }
        }
    }
    while (mask != 0x80)
    {
        mask >>= 1;
        if (mask == 0)
        {
            break;
        }
    }
    return destination;
}
}
