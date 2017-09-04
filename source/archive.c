#include <3ds.h>
#include <string.h>
#include "archive.h"
#include <libarchive/archive.h>
#include <libarchive/archive_entry.h>

static int copy_data(struct archive *ar, struct archive *aw)
{
	int r;
	const void *buff;
	size_t size;
	int64_t offset;
	for (;;) 
	{
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) 
		{
			printf("%s\n", archive_error_string(aw));
			return (r);
		}
	}
}

void archiveExtractFile(void *buf, size_t size, char *fileToExtract, char *ExtractAs, char *location)
{
	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;
	
	/* Select which attributes we want to restore. */ 
	flags = ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;

	a = archive_read_new();
	archive_read_support_format_7zip(a);
	archive_read_support_format_zip(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	if((r = archive_read_open_memory(a, buf, size)))
		printf("Couldn't open memory");
	printf("Error : %s\n", archive_error_string(a));
	for(;;)
	{
		r = archive_read_next_header(a, &entry);
		if(r == ARCHIVE_EOF)break;
		if(r == ARCHIVE_OK)
		{
			const char *file = archive_entry_pathname(entry);
			char extractLoc[100];
			if(strcmp(file, fileToExtract)==0)
			{
				printf("File match Found!\n");
				printf("%s\n",file);
				sprintf(extractLoc, "%s%s", location, ExtractAs);
				archive_entry_set_pathname(entry, extractLoc);
				r = archive_write_header(ext, entry);
				if(archive_entry_size(entry) > 0)
					copy_data(a, ext);
				archive_write_finish_entry(ext);
				break;
			}
		}
	
	}
	archive_read_close(a); 
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);
}