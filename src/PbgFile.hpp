#pragma once
#include <windows.h>

namespace Pbg
{
// Abstract stream. Its vtable at 0x004A4850 has seven _purecall slots and
// then the virtual destructor.
class IFile
{
public:
    virtual unsigned char Open(const char *filename, const char *mode) = 0;
    virtual void Close() = 0;
    virtual unsigned int Read(void *destination, unsigned int length) = 0;
    virtual unsigned char Write(const void *source, unsigned int length) = 0;
    virtual unsigned int Tell() = 0;
    virtual unsigned int GetSize() = 0;
    virtual unsigned char Seek(long offset, unsigned int origin) = 0;
    virtual ~IFile() {}
};

// Disk file stream; vtable at 0x004A4828.
class File : public IFile
{
public:
    File() : handle(INVALID_HANDLE_VALUE), access(0) {}
    ~File();

    unsigned char Open(const char *filename, const char *mode);
    void Close();
    unsigned int Read(void *destination, unsigned int length);
    unsigned char Write(const void *source, unsigned int length);
    unsigned int Tell();
    unsigned int GetSize();
    unsigned char Seek(long offset, unsigned int origin);
    virtual void *ReadWhole(unsigned int maximumSize);

    HANDLE handle;
    DWORD access;
};

// In-memory stream over a malloc buffer; vtable at 0x004A491C. It does not
// override Open, so its first slot stays _purecall.
class MemFile : public IFile
{
public:
    ~MemFile();

    void Close();
    unsigned int Read(void *destination, unsigned int length);
    unsigned char Write(const void *source, unsigned int length);
    unsigned int Tell();
    unsigned int GetSize();
    unsigned char Seek(long offset, unsigned int origin);

    unsigned int size;
    unsigned char *cursor;
    unsigned char *data;
};
}
