#pragma once

// Release builds keep the calls but compile the logger to an empty body.
void DebugPrint(const char *format, ...);
