#pragma once

// Header of a loaded SCPT script file (0x24 bytes), followed by
// `includeLength` bytes of include data, `subCount` sub offsets and the
// NUL-terminated sub names.
struct ScptHeader
{
    unsigned int magic;             // 'SCPT'
    unsigned short version;         // +0x04, must be 1
    unsigned short includeLength;   // +0x06
    unsigned char unknown08[0x10 - 0x08];
    unsigned short subCount;        // +0x10
    unsigned char unknown12[0x24 - 0x12];
};

struct EclSub
{
    const char *name;
    void *code;
};

struct EclInstr;
class EclVm;

// RTTI SptResourceInf (vtable 0x004A26DC): script file registry. EclResourceInf
// (vtable 0x004A2720) derives from it and overrides the include slot.
class SptResourceInf
{
public:
    virtual int Load(void *file);
    virtual int LoadIncludes(void *includes);

    EclInstr *FindSub(const char *name);
    EclVm *CreateVm(const char *subName);

    int fileCount;                  // +0x04
    int subCount;                   // +0x08
    ScptHeader *files[32];          // +0x0C
    EclSub *subs;                   // +0x8C
};
