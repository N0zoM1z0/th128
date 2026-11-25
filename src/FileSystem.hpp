#ifndef TH128_FILE_SYSTEM_HPP
#define TH128_FILE_SYSTEM_HPP

#include <windows.h>

namespace FileSystem
{
unsigned char *__stdcall OpenPath(const char *path, int *outSize, int isExternal);
int __stdcall FileExists(const char *path);
int __stdcall WriteDataToFile(const char *path, const void *data, unsigned int size);
int __stdcall OpenWriteStream(const char *path);
int __stdcall OpenReadStream(const char *path);
int __stdcall WriteStream(const void *data, unsigned int size);
unsigned char *__stdcall ReadStream(unsigned int size);
int __stdcall CloseStream();
}

extern HANDLE g_StreamFileHandle;

#endif
