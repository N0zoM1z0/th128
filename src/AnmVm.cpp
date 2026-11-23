#include "AnmVm.hpp"

#include <string.h>

AnmVm::AnmVm()
{
    memset(this, 0, sizeof(AnmVm));
    anmFileIndex = -1;
}

// Reset a VM selected for reuse. The target clears the complete 0x4E8-byte
// object but preserves its script state and world position, then reinstates
// the default transforms and the two self-referential list sentinels.
void AnmVm::ResetForReuse()
{
    const int savedScriptState = scriptState;
    const ZunVec3 savedPosition = position;

    memset(this, 0, sizeof(AnmVm));

    scriptState = savedScriptState;
    scaleX = 1.0f;
    scaleY = 1.0f;
    scale2X = 1.0f;
    scale2Y = 1.0f;
    unknown058 = 1.0f;
    unknown05C = 1.0f;
    position = savedPosition;

    unknown3F0 = -1;
    transformMatrix[0][0] = 1.0f;
    transformMatrix[1][1] = 1.0f;
    transformMatrix[2][2] = 1.0f;
    transformMatrix[3][3] = 1.0f;

    flags = 7;
    timer070.previous = -999999;
    timer070.current = 0;
    timer070.currentF = 0.0f;
    timer070.gameSpeed = 0;
    timer070.initialized = 0;
    timer070.controlFlags = 0x40;
    stateFlags = 0x40;

    managerNode.vm = this;
    childNode.vm = this;
}
