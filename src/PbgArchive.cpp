#include "PbgArchive.hpp"

#include "BlockCipher.hpp"
#include "DebugLog.hpp"

#include <new>
#include <stdlib.h>
#include <string.h>

namespace Pbg
{
ArchiveEntry::ArchiveEntry() : name(NULL)
{
}

ArchiveEntry::~ArchiveEntry()
{
    if (name != NULL)
    {
        free(name);
        name = NULL;
    }
}

bool Archive::Open(const char *filename)
{
    Close();
    DebugPrint("info : %s open arcfile\r\n", filename);
    file = new File;
    if (file != NULL)
    {
        if (ReadHeader(filename))
        {
            char *copy = (char *)malloc(strlen(filename) + 1);
            if (copy != NULL)
            {
                strcpy(copy, filename);
            }
            name = copy;
            if (name != NULL)
            {
                file->Open(name, "r");
                return true;
            }
        }
        DebugPrint("info : %s not found\r\n", filename);
        Close();
    }
    return false;
}

bool Archive::ReadHeader(const char *filename)
{
    // magic, unpacked listing size, packed listing size, entry count
    unsigned int header[4];
    unsigned char *listing = NULL;
    if (file == NULL)
    {
        return false;
    }
    if (file->Open(filename, "r") && file->Read(header, sizeof(header)) != 0)
    {
        DecryptBlocks((unsigned char *)header, sizeof(header), 0x1B, 0x37, sizeof(header), sizeof(header));
        if (header[0] == 0x31414854)
        {
            header[1] -= 123456789;
            header[2] -= 987654321;
            count = header[3] - 135792468;
            unsigned int listingOffset = file->GetSize() - header[2];
            file->Seek(listingOffset, FILE_BEGIN);
            unsigned int packedSize = header[2];
            unsigned char *packed = (unsigned char *)malloc(packedSize);
            if (packed != NULL)
            {
                if (file->Read(packed, packedSize) != 0)
                {
                    DecryptBlocks(packed, packedSize, 0x3E, 0x9B, 0x80, packedSize);
                    listing = DecompressLzss(packed, packedSize, NULL, header[1]);
                    if (listing != NULL)
                    {
                        entries = ParseArchiveDirectory((const char *)listing, count, listingOffset);
                        if (entries != NULL)
                        {
                            free(packed);
                            free(listing);
                            return true;
                        }
                    }
                }
                free(packed);
                if (listing != NULL)
                {
                    free(listing);
                }
            }
        }
    }
    if (file != NULL)
    {
        delete file;
    }
    file = NULL;
    return false;
}

void *Archive::ReadEntry(const char *entryName, void *destination)
{
    unsigned char *packed = NULL;
    if (file == NULL)
    {
        return NULL;
    }
    ArchiveEntry *entry = entries;
    if (entry != NULL)
    {
        for (int i = count; i > 0; --i, ++entry)
        {
            if (_stricmp(entryName, entry->name) == 0)
            {
                unsigned int packedSize = entry[1].offset - entry->offset;
                unsigned int size = entry->size;
                if (packedSize != size || (packed = (unsigned char *)destination) == NULL)
                {
                    packed = (unsigned char *)malloc(packedSize);
                    if (packed == NULL)
                    {
                        break;
                    }
                }
                if (file->Seek(entry->offset, FILE_BEGIN) && file->Read(packed, packedSize) != 0)
                {
                    unsigned char sum = 0;
                    const char *cursor = entry->name;
                    for (int length = strlen(cursor); length != 0; --length)
                    {
                        sum += *cursor++;
                    }
                    const ArchiveCipherKey &key = g_ArchiveCipherKeys[(unsigned char)(sum % 8)];
                    DecryptBlocks(packed, packedSize, key.key, key.step, key.block, key.limit);
                    unsigned char *result = packed;
                    if (packedSize != size)
                    {
                        result = DecompressLzss(packed, packedSize, (unsigned char *)destination, size);
                    }
                    if (packed != destination && packed != NULL)
                    {
                        free(packed);
                    }
                    return result;
                }
                break;
            }
        }
    }
    DebugPrint("info : %s error\r\n", name);
    if (packed != NULL)
    {
        free(packed);
    }
    return NULL;
}

Archive::~Archive()
{
    Close();
}

void Archive::Close()
{
    if (name != NULL)
    {
        DebugPrint("info : %s close arcfile\r\n", name);
        if (name != NULL)
        {
            free(name);
            name = NULL;
        }
    }
    name = NULL;
    if (entries != NULL)
    {
        delete[] entries;
    }
    entries = NULL;
    if (file != NULL)
    {
        delete file;
    }
    file = NULL;
    count = 0;
}

ArchiveEntry *__stdcall ParseArchiveDirectory(const char *listing, int count, unsigned int archiveSize)
{
    ArchiveEntry *entries = new ArchiveEntry[count + 1];
    if (entries == NULL)
    {
        return NULL;
    }
    const char *cursor = listing;
    for (int i = 0; i < count; ++i)
    {
        char *copy = (char *)malloc(strlen(cursor) + 1);
        if (copy != NULL)
        {
            strcpy(copy, cursor);
        }
        entries[i].name = copy;
        int length = strlen(cursor) + 1;
        if (length % 4 != 0)
        {
            length += 4 - length % 4;
        }
        cursor += length;
        const unsigned int *fields = (const unsigned int *)cursor;
        entries[i].offset = fields[0];
        entries[i].size = fields[1];
        entries[i].reserved = fields[2];
        cursor += 12;
    }
    entries[count].offset = archiveSize;
    entries[count].size = 0;
    return entries;
}
}
