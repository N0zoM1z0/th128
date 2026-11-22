#include "AnmVm.hpp"

#include <stdlib.h>
#include <string.h>

// Extra state of the effect started by this callback (0x2C bytes).
struct AnmEffectData
{
    ZunVec3 position;           // +0x00, screen coordinates
    ZunTimer timer;             // +0x0C
    float scale;                // +0x20
    unsigned int color;         // +0x24
    unsigned int color2;        // +0x28
};

// Effect start callback from the table at 0x004B1E9C (slot 6): converts the
// game-space position to the screen and enters script state 13.
int __fastcall StartAnmVmEffect06(AnmVm *vm, const float *position)
{
    AnmEffectData *data = (AnmEffectData *)malloc(sizeof(AnmEffectData));
    vm->extraData = data;
    memset(data, 0, sizeof(AnmEffectData));
    data->position.x = position[0] + 128.0 + 192.0;
    data->position.y = position[1] + 16.0;
    data->position.z = position[2];
    data->timer.Reset();
    data->scale = 128.0f;
    data->color = 0x40FF0000;
    data->color2 = 0x00FF00FF;
    vm->position = data->position;
    vm->scriptState = 13;
    return 0;
}

// Empty slot of the effect table (default handler).
int __fastcall AnmVmEffectNoop412B30(AnmVm *, const float *)
{
    return 0;
}

// Empty slot of the effect table (default handler).
int __fastcall AnmVmEffectNoop412B40(AnmVm *, const float *)
{
    return 0;
}

// Empty slot of the effect table (default handler).
int __fastcall AnmVmEffectNoop412DF0(AnmVm *, const float *)
{
    return 0;
}

// Empty slot of the effect table (default handler).
int __fastcall AnmVmEffectNoop412E00(AnmVm *, const float *)
{
    return 0;
}
