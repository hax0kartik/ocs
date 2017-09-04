#include <3ds.h>

void fsOpenAndWrite(const char *location, void *data, size_t size);

void fsOpenAndRead(const char *location, void *data, u32 *readsize, u32 size);

u64 fsGetFileSize(const char *location);