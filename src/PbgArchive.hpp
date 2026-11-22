#pragma once

#include "PbgFile.hpp"

namespace Pbg
{
// One 16-byte archive directory entry. The directory is allocated with
// new[] so the vector iterators call these out-of-line members.
struct ArchiveEntry
{
    char *name;
    unsigned int offset;
    unsigned int size;
    unsigned int reserved;

    ArchiveEntry();
    ~ArchiveEntry();
};

// A THA1 archive (the original source is src\\pack\\_ArcMngr.cpp) opened from disk: directory, entry count, archive name and
// the underlying stream.
struct Archive
{
    ArchiveEntry *entries;
    int count;
    char *name;
    IFile *file;

    ~Archive();
    bool Open(const char *filename);
    void Close();
    bool ReadHeader(const char *filename);
};

// Builds the directory from the decoded THA1 listing: per entry a
// NUL-terminated name padded to four bytes, then offset, size and a reserved
// word. The extra final entry records the archive size as an end offset.
ArchiveEntry *__stdcall ParseArchiveDirectory(const char *listing, int count, unsigned int archiveSize);
}
