// Non-runnable compiler context: the directory's new[]/delete[] take the
// entry constructor and destructor addresses, as in the original.
#include "../src/PbgArchive.hpp"
#include "../src/DebugLog.hpp"
#include "../src/BlockCipher.hpp"

#include <stdlib.h>

Pbg::ArchiveCipherKey Pbg::g_ArchiveCipherKeys[8];

// Shapes of the other original callers (replay and score loaders), whose
// sizes come from header fields.
struct ProbeBlobHeader
{
    unsigned char reserved[0x10];
    unsigned int packedSize;
    unsigned int size;
    unsigned char *body;
    unsigned int bodySize;
    unsigned int bodyPacked;
};

static void *ProbeReplayLike(ProbeBlobHeader *header, unsigned char *data, unsigned char *buffer)
{
    DecryptBlocks(data, header->bodySize, 0x5E, 0xE7, 0x800, header->bodySize);
    DecryptBlocks(data, header->bodySize, 0x7D, 0x36, 0x80, header->bodySize);
    return Pbg::DecompressLzss(data, header->bodySize, buffer, header->bodyPacked);
}

static void *ProbeScoreLike(ProbeBlobHeader *header, unsigned char *destination)
{
    DecryptBlocks(header->reserved + 0x18, header->size, 0xAC, 0x35, 0x10, header->size);
    return Pbg::DecompressLzss(header->reserved + 0x18, header->packedSize, destination, header->size);
}

// Hypothesis: the decoder's address escapes in the original, which keeps it
// a plain __stdcall function (all four arguments on the stack).
typedef unsigned char *(__stdcall *ProbeDecoder)(const unsigned char *, int, unsigned char *, unsigned int);
ProbeDecoder g_ProbeDecoder = Pbg::DecompressLzss;

extern "C" void __stdcall PbgArchiveProbe(Pbg::ArchiveEntry **a, Pbg::ArchiveEntry **b, int count)
{
    ProbeReplayLike((ProbeBlobHeader *)a, (unsigned char *)b, (unsigned char *)*a);
    ProbeScoreLike((ProbeBlobHeader *)b, (unsigned char *)*b);
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
