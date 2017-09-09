#include "log.h"
#include <3ds.h>
#include <stdarg.h>
#include <stdio.h>

int print(const char *format, ...)
{
	va_list arg;
	int done;
	FILE *file = fopen("/ocs.log", "a");
	va_start (arg, format);
	done = vfprintf(stdout, format, arg);
	done = vfprintf(file, format, arg);
	va_end (arg);
	fclose(file);
   return done;
}
