#include <3ds.h>
#include "fs.h"
void fsOpenAndWrite(const char *location, void *data, size_t size)
{
	Handle file, log;
	FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
	FS_Path filePath = fsMakePath(PATH_ASCII, location);
	FSUSER_OpenFileDirectly(&file, ARCHIVE_SDMC, archivePath, filePath, FS_OPEN_WRITE|FS_OPEN_CREATE, 0x0);
	FSFILE_Write(file, NULL, 0x0, data, size, FS_WRITE_FLUSH);
	FSFILE_Close(file);
}