#include <3ds.h>

void fsOpenAndWrite(const char *location, void *data, size_t size);

Result fsOpenAndWriteNAND(const char *location, void *data, size_t size);

u8 *fsOpenAndRead(const char *location, u32 *readSize);

Result checkFileExists(const char *location);