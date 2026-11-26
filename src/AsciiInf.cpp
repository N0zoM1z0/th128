#include "AsciiInf.hpp"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

AsciiTextId AsciiInf::CreateText(const ZunVec3 *position, const char *text)
{
    if (textCount >= 320)
    {
        AsciiTextId empty = { 0 };
        return empty;
    }

    AsciiTextEntry *entry = &entries[textCount++];
    strcpy(entry->text, text);
    entry->position = *position;
    entry->color = color;
    entry->scaleX = scaleX;
    entry->scaleY = scaleY;
    entry->style = style;
    entry->alignmentX = alignmentX;
    entry->alternateMode = alternateMode;
    entry->alignmentY = alignmentY;
    entry->unknown138 = unknown1C874;
    entry->unknown13C = unknown1C878;
    entry->unknown140 = unknown1C87C;
    entry->font = font;
    entry->initialColor = color;
    entry->backgroundColor = backgroundColor;
    entry->timer.Initialize();
    entry->timer.currentF = 0.0f;
    entry->timer.current = 0;
    entry->timer.previous = -1;
    entry->unknown148 = unknown1C880;
    entry->unknown14C = unknown1C884;
    ++nextTextId;
    if (nextTextId == 0)
        nextTextId = 1;
    entry->id.id = nextTextId;
    return entry->id;
}

AsciiTextId __cdecl AsciiInf::CreateTextF(const ZunVec3 *position, const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    AsciiTextId result;
    result = CreateText(position, buffer);
    return result;
}
