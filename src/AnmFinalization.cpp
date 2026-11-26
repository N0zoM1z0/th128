#include "AnmResource.hpp"
#include "AnmManager.hpp"
#include "AnmHelpers.hpp"
#include "Chain.hpp"
#include "GameErrorContext.hpp"

namespace
{
    struct AnmArchiveScript
    {
        unsigned int id;
        unsigned int offset;
    };

    inline void ReleasePendingAnm(AnmManager *manager, int slot)
    {
        if (slot >= 0 && slot < sizeof(manager->loadedAnms) / sizeof(manager->loadedAnms[0]))
        {
            manager->loadedAnms[slot]->Release();
            delete manager->loadedAnms[slot];
            manager->loadedAnms[slot] = 0;
        }
    }
}

void AnmResource::SetSprite(int spriteIndex, const AnmFileEntry *sprite)
{
    memcpy(&anmFiles[spriteIndex], sprite, sizeof(AnmFileEntry));
    anmFiles[spriteIndex].uvLeft = anmFiles[spriteIndex].left / anmFiles[spriteIndex].textureWidth;
    anmFiles[spriteIndex].uvRight = anmFiles[spriteIndex].right / anmFiles[spriteIndex].textureWidth;
    anmFiles[spriteIndex].uvTop = anmFiles[spriteIndex].top / anmFiles[spriteIndex].textureHeight;
    anmFiles[spriteIndex].uvBottom = anmFiles[spriteIndex].bottom / anmFiles[spriteIndex].textureHeight;
    anmFiles[spriteIndex].spriteWidth =
        (anmFiles[spriteIndex].right - anmFiles[spriteIndex].left) / sprite->scaleX;
    anmFiles[spriteIndex].spriteHeight =
        (anmFiles[spriteIndex].bottom - anmFiles[spriteIndex].top) / sprite->scaleY;
}

AnmResource *AnmResource::FinalizeNextEntry()
{
    ChainDebugLog("::postloadAnim : %d, %d\n", fileId, finalizeCursor);
    const AnmArchiveEntry *entry = static_cast<const AnmArchiveEntry *>(anmFileHeader);
    int entryIndex = 0;
    int scriptBase = 0;
    int spriteBase = 0;
    int textureIndex = 0;
    bool processed = false;
    for (;;)
    {
        if (entryIndex == finalizeCursor - 1)
        {
            int spriteIndex = spriteBase;
            if (entry == 0)
            {
                g_GameErrorContext.Fatal("\x83\x41\x83\x6a\x83\x81\x82\xaa\x93\xc7\x82\xdd\x8d\x9e\x82\xdf\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8e\xb8\x82\xed\x82\xea\x82\xc4\x82\xe9\x82\xa9\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
                finalizeCursor = 0;
                return 0;
            }
            if (entry->version != 7)
            {
                g_GameErrorContext.Fatal("\x83\x41\x83\x6a\x83\x81\x82\xcc\x83\x6f\x81\x5b\x83\x57\x83\x87\x83\x93\x82\xaa\x88\xe1\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
                finalizeCursor = 0;
                return 0;
            }
            if (entry->hasEmbeddedTexture == 0)
            {
                const char *name = reinterpret_cast<const char *>(entry) + entry->nameOffset;
                if (name[0] == '@')
                {
                    if (name[1] == 'R')
                    {
                        textures[textureIndex].CreateRenderTarget();
                    }
                    else
                    {
                        textureMemoryBytes += textures[textureIndex].CreateEmptyTexture(
                            entry->width, entry->height, entry->format);
                    }
                }
                else
                {
                    const int size = textures[textureIndex].LoadExternalTexture(
                        entry->format, entry->width, entry->height, entry->offsetX, entry->offsetY);
                    if (size < 0)
                    {
                        g_GameErrorContext.Fatal("\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x20\x25\x73\x20\x82\xaa\x8d\xec\x90\xac\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8e\xb8\x82\xed\x82\xea\x82\xc4\x82\xe9\x82\xa9\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a", name);
                        finalizeCursor = 0;
                        return 0;
                    }
                    textureMemoryBytes += size;
                }
            }
            else
            {
                const AnmEmbeddedTexture *data = reinterpret_cast<const AnmEmbeddedTexture *>(
                    reinterpret_cast<const unsigned char *>(entry) + entry->textureOffset);
                const int size = textures[textureIndex].LoadEmbeddedTexture(
                    data, entry->format, entry->width, entry->height);
                if (size < 0)
                {
                    g_GameErrorContext.Fatal("\x83\x65\x83\x4e\x83\x58\x83\x60\x83\x83\x82\xaa\x8d\xec\x90\xac\x82\xc5\x82\xab\x82\xdc\x82\xb9\x82\xf1\x81\x42\x83\x66\x81\x5b\x83\x5e\x82\xaa\x8e\xb8\x82\xed\x82\xea\x82\xc4\x82\xe9\x82\xa9\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\x0d\x0a");
                    finalizeCursor = 0;
                    return 0;
                }
                textureMemoryBytes += size;
            }

            AnmD3DSurfaceDesc desc;
            reinterpret_cast<AnmD3DTextureSurfaceView *>(textures[textureIndex].texture)->GetLevelDesc(0, &desc);
            const unsigned int *offset = reinterpret_cast<const unsigned int *>(entry + 1);
            for (int i = 0; i < entry->spriteCount; ++i, ++offset)
            {
                const AnmArchiveSprite *raw = reinterpret_cast<const AnmArchiveSprite *>(
                    reinterpret_cast<const unsigned char *>(entry) + *offset);
                AnmFileEntry sprite;
                sprite.fileId = fileId;
                sprite.textureIndex = textureIndex;
                sprite.texture = &textures[textureIndex].texture;
                sprite.scaleX = (float)desc.Width / entry->width;
                sprite.scaleY = (float)desc.Height / entry->height;
                sprite.left = raw->x * sprite.scaleX;
                sprite.top = raw->y * sprite.scaleY;
                sprite.right = (raw->width + raw->x) * sprite.scaleX;
                sprite.bottom = (raw->height + raw->y) * sprite.scaleY;
                sprite.textureWidth = (float)desc.Width;
                sprite.textureHeight = (float)desc.Height;
                SetSprite(spriteIndex, &sprite);
                ++spriteIndex;
            }

            const AnmArchiveScript *script = reinterpret_cast<const AnmArchiveScript *>(offset);
            for (int i = 0; i < entry->scriptCount; ++i)
            {
                scriptOffsets[scriptBase + i] = reinterpret_cast<unsigned int>(entry) + script[i].offset;
            }
            processed = true;
        }
        spriteBase += entry->spriteCount;
        scriptBase += entry->scriptCount;
        ++textureIndex;
        if (entry->nextOffset == 0)
        {
            finalizeCursor = 0;
            return this;
        }
        ++entryIndex;
        entry = reinterpret_cast<const AnmArchiveEntry *>(
            reinterpret_cast<const unsigned char *>(entry) + entry->nextOffset);
        if (entryIndex == finalizeCursor || processed)
        {
            ++finalizeCursor;
            return this;
        }
    }
}

int AnmManager::ProcessPendingAnms()
{
    AnmManager *manager = g_AnmManager;
    for (unsigned int i = 0; i < 38; ++i)
    {
        AnmResource *resource = manager->loadedAnms[i];
        if (resource != 0)
        {
            if (resource->releaseRequested != 0)
            {
                ReleasePendingAnm(manager, (int)i);
                manager->loadedAnms[i]->releaseRequested = 0;
            }
            else if (resource->finalizeCursor != 0)
            {
                return manager->loadedAnms[i]->FinalizeNextEntry() != 0 ? 0 : -1;
            }
        }
    }
    return 0;
}
