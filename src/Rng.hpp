#pragma once

// Game random number generator. Field names are provisional.
struct Rng
{
    unsigned short seed;            // +0x00
    unsigned int generationCount;   // +0x04

    unsigned short GetU16();
    unsigned int GetU32();
    float GetFloat();               // [0, 1)
    float GetFloatSigned();         // [-1, 1)
    float GetAngle();               // [-pi, pi)
};
