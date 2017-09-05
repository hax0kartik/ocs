#include <3ds.h>
#include "fs.h"
void fsOpenAndWrite(const char *location, void *data, size_t size)
{
	Handle file;
	FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
	FS_Path filePath = fsMakePath(PATH_ASCII, location);
	FSUSER_OpenFileDirectly(&file, ARCHIVE_SDMC, archivePath, filePath, FS_OPEN_WRITE|FS_OPEN_CREATE, 0x0);
	FSFILE_Write(file, NULL, 0x0, data, size, FS_WRITE_FLUSH);
	FSFILE_Close(file);
}

void fsOpenAndRead(const char *location, void *data, u32 *readsize, u32 size)
{
	Handle file;
	FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
	FS_Path filePath = fsMakePath(PATH_ASCII, location);
	FSUSER_OpenFileDirectly(&file, ARCHIVE_SDMC, archivePath, filePath, FS_OPEN_READ, 0x0);
	FSFILE_Read(file, readsize, 0x0, data, size);
	FSFILE_Close(file);
}

u64 fsGetFileSize(const char *location)
{
	Handle file;
	FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
	FS_Path filePath = fsMakePath(PATH_ASCII, location);
	FSUSER_OpenFileDirectly(&file, ARCHIVE_SDMC, archivePath, filePath, FS_OPEN_READ, 0x0);
	u64 size;
	FSFILE_GetSize (file, &size);
	FSFILE_Close(file);
	return size;
}