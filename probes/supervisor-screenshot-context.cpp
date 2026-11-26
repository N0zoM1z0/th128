// Non-runnable VC10 /GL context for screenshot handling and surface copies.
#include "../src/SupervisorFrame.hpp"
#include "../src/Supervisor.hpp"
#include "../src/AnmManager.hpp"
#include "../src/GameErrorContext.hpp"
#include "../src/PbgArchive.hpp"
#include "../src/Input.hpp"

AnmManager *g_AnmManager;
InputState g_InputStates[4];

extern "C" void __stdcall SupervisorScreenshotProbe(SupervisorFrame *frame)
{
    frame->HandleScreenshot();
}
