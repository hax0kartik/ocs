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
	if(!file) return NULL;
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

Result checkFileExists(const char *location)
{
	FILE *file;
	if (file = fopen(location, "r")){
	fclose(file);
		return 1;
	}
	return 0;
}
Result fsOpenAndWriteNAND(const char *location, void *data, size_t size)
{
	Handle log;
	fsInit();
	FS_Archive ctrArchive;
	FS_Path path = fsMakePath (PATH_EMPTY,"");
	Result ret = FSUSER_OpenArchive(&ctrArchive, ARCHIVE_NAND_CTR_FS,path);
	if(ret != 0)
	{
		printf("FATAL\nCouldn't open CTR-NAND for writing");
		fsExit();
		return ret;
	}
	FS_Path path2 = fsMakePath(PATH_ASCII, location);
	ret = FSUSER_OpenFile(&log,ctrArchive,path2,FS_OPEN_WRITE|FS_OPEN_CREATE,0x0);
	if(ret != 0)
	{
		printf("FATAL\nCouldn't open boot.firm for writing");
		fsExit();
		FSUSER_CloseArchive (ctrArchive);
		return ret;
	}
	ret = FSFILE_Write(log,NULL,0x0,data,size,FS_WRITE_FLUSH);
	if(ret != 0)
	{
		printf("FATAL\nCouldn't write boot.firm");
		FSFILE_Close(log);
		FSUSER_CloseArchive (ctrArchive);
		fsExit();
		return ret;
	}
	FSFILE_Close(log);
	FSUSER_CloseArchive (ctrArchive);
	fsExit();
	return ret;
}