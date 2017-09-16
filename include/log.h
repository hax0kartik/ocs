#include <3ds.h>
#include <stdio.h>

FILE *File;
static inline void logInit()
{
	File = fopen("/ocs.log", "a");
}

int print(const char *format, ...);

static inline void logExit()
{
	fclose(File);
}