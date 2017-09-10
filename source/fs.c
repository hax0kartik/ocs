#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
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

u8 *fsOpenAndRead(const char *location, u32 *readSize)
{
	FILE *file = fopen(location, "rb");
	// seek to end of file
	fseek(file,0,SEEK_END);
	// file pointer tells us the size
	off_t size = ftell(file);
	// seek back to start
	fseek(file,0,SEEK_SET);
	//allocate a buffer
	u8 *data=malloc(size);
	//read contents !
	off_t bytesRead = fread(data, 1, size, file);
	//close the file because we like being nice and tidy
	*readSize = size;
	fclose(file);
	return data;
}

void fsOpenAndWriteNAND(const char *location, void *data, size_t size)
{
	Handle file;
	FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
	FS_Path filePath = fsMakePath(PATH_ASCII, location);
	FSUSER_OpenFileDirectly(&file, ARCHIVE_NAND_CTR_FS, archivePath, filePath, FS_OPEN_WRITE|FS_OPEN_CREATE, 0x0);
	FSFILE_Write(file, NULL, 0x0, data, size, FS_WRITE_FLUSH);
	FSFILE_Close(file);
}