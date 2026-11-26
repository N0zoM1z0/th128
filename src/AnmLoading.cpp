#include "AnmManager.hpp"
#include "AnmResource.hpp"
#include "FileSystem.hpp"
#include "Chain.hpp"
#include "GameErrorContext.hpp"

#include <new>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace
{
    inline const AnmArchiveEntry *NextAnmEntry(const AnmArchiveEntry *entry)
    {
        return reinterpret_cast<const AnmArchiveEntry *>(
            reinterpret_cast<const unsigned char *>(entry) + entry->nextOffset);
    }
}

AnmResource *AnmManager::LoadAnm(int slot, const char *filename)
{
    ChainDebugLog("::preloadAnim : %s\n", filename);
    if (slot >= 38)
    {
        g_GameErrorContext.Fatal("\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x8a\x69\x94\x5b\x90\xe6\x82\xaa\x91\xab\x82\xe8\x82\xdc\x82\xb9\x82\xf1\x0d\x0a");
        return 0;
    }

    char path[260];
    sprintf(path, "%s", filename);
    const AnmArchiveEntry *header = reinterpret_cast<const AnmArchiveEntry *>(
        FileSystem::OpenPath(path, 0, 0));
    int textureIndex = 0;
    AnmResource *resource = new AnmResource;
    loadedAnms[slot] = resource;
    if (header == 0)
    {
        return 0;
    }

    resource->fileId = slot;
    resource->anmFileHeader = const_cast<AnmArchiveEntry *>(header);
    strcpy(resource->filename, filename);

    int scripts = header->scriptCount;
    int sprites = header->spriteCount;
    const AnmArchiveEntry *countedEntry = header;
    int textures = 1;
    while (countedEntry->nextOffset != 0)
    {
        countedEntry = NextAnmEntry(countedEntry);
        scripts += countedEntry->scriptCount;
        sprites += countedEntry->spriteCount;
        ++textures;
    }
    resource->textureCount = textures;
    const unsigned int textureBytes = textures * sizeof(AnmTextureEntry);
    resource->textures = static_cast<AnmTextureEntry *>(malloc(textureBytes));
    memset(resource->textures, 0, textureBytes);
    resource->anmFiles = static_cast<AnmFileEntry *>(malloc(sprites * sizeof(AnmFileEntry)));
    resource->scriptOffsets = static_cast<unsigned int *>(malloc(scripts * sizeof(unsigned int)));
    resource->spriteCount = sprites;
    resource->scriptCount = scripts;

    const AnmArchiveEntry *entry = header;
    for (;;)
    {
        if (entry == 0)
        {
            g_GameErrorContext.Fatal("\x83\x41\x83\x6a\x83\x81\x82\xaa\x93\xc7\x82\xdd\x8d\x9e\x82\xdf\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8e\xb8\x82\xed\x82\xea\x82\xc4\x82\xe9\x82\xa9\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
            return 0;
        }
        if (resource->CacheExternalTexture(entry, textureIndex) < 0)
        {
            return 0;
        }
        ++textureIndex;
        if (entry->nextOffset == 0)
        {
            return resource;
        }
        entry = NextAnmEntry(entry);
    }
}

int AnmResource::CacheExternalTexture(const AnmArchiveEntry *entry, int textureIndex)
{
    if (entry->version != 7)
    {
        g_GameErrorContext.Fatal("\x83\x41\x83\x6a\x83\x81\x82\xcc\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x82\xaa\x88\xe1\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
        return -1;
    }
    if (entry->hasEmbeddedTexture == 0)
    {
        const char *name = reinterpret_cast<const char *>(entry) + entry->nameOffset;
        if (name[0] == '@')
        {
            return 1;
        }
        char path[260];
        int dataSize;
        sprintf(path, "%s", name);
        unsigned char *data = FileSystem::OpenPath(path, &dataSize, 1);
        if (data == 0)
        {
            g_GameErrorContext.Fatal("\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x20\x25\x73\x20\x82\xaa\x93\xc7\x82\xdd\x8d\x9e\x82\xdf\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8e\xb8\x82\xed\x82\xea\x82\xc4\x82\xe9\x82\xa9\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a", name);
            return -1;
        }
        textures[textureIndex].rawDataSize = dataSize;
        textures[textureIndex].rawData = data;
    }
    return 1;
}
