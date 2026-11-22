// Non-runnable compiler context: the directory's new[]/delete[] take the
// entry constructor and destructor addresses, as in the original.
#include "../src/PbgArchive.hpp"
#include "../src/DebugLog.hpp"

#include <stdlib.h>

Pbg::ArchiveCipherKey Pbg::g_ArchiveCipherKeys[8];

// Placeholder for the not yet reconstructed LZSS decoder (0x00450860): a
// defined LTCG callee that takes its four arguments on the stack.
unsigned char *__stdcall Pbg::DecompressLzss(const unsigned char *packed, int packedSize,
                                             unsigned char *destination, unsigned int size)
{
    unsigned char *out;
    unsigned char mask = 0x80;
    unsigned char bits = 0;
    if (destination == NULL && (destination = (unsigned char *)malloc(size)) == NULL)
    {
        return NULL;
    }
    out = destination;
    const unsigned char *in = packed;
    for (;;)
    {
        if (mask == 0x80)
        {
            bits = *in;
            if (in - packed < packedSize)
            {
                ++in;
            }
            else
            {
                bits = 0;
            }
        }
        if ((bits & mask) == 0 || (unsigned int)(out - destination) >= size)
        {
            break;
        }
        *out++ = *in;
        mask >>= 1;
        if (mask == 0)
        {
            mask = 0x80;
        }
    }
    return destination;
}

extern "C" void __stdcall PbgArchiveProbe(Pbg::ArchiveEntry **a, Pbg::ArchiveEntry **b, int count)
{
    Pbg::Archive *archives = new Pbg::Archive[count];
    archives[0].Open((const char *)a);
    archives[1].Open((const char *)b);
    archives[0].ReadEntry((const char *)b, *a);
    archives[1].ReadEntry((const char *)a, NULL);
    archives[0].Close();
    archives[count - 1].Close();
    delete[] archives;
    *a = Pbg::ParseArchiveDirectory((const char *)b, count, 16);
    *b = Pbg::ParseArchiveDirectory((const char *)a, count + 2, 32);
    delete[] *a;
    delete[] *b;
    *a = new Pbg::ArchiveEntry[count];
    *b = new Pbg::ArchiveEntry[count + 1];
    delete[] *a;
    delete[] *b;
}
