#ifndef TH128_GAME_ERROR_CONTEXT_HPP
#define TH128_GAME_ERROR_CONTEXT_HPP

struct GameErrorContext
{
    char buffer[0x2000]; // +0x0000
    char *cursor;        // +0x2000
    unsigned char fatal; // +0x2004

    const char *__cdecl Log(const char *fmt, ...);
    const char *__cdecl Fatal(const char *fmt, ...);
};

extern GameErrorContext g_GameErrorContext;

#endif
