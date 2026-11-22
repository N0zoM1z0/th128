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

// A THA1 archive opened from disk: directory, entry count, archive name and
// the underlying stream.
struct Archive
{
    ArchiveEntry *entries;
    int count;
    char *name;
    IFile *file;

    ~Archive();
    void Close();
};
}
