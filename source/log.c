#include "log.h"
#include <3ds.h>
#include <stdarg.h>

int print(const char *format, ...)
{
	va_list arg;
	int done;
	va_start (arg, format);
	done = vfprintf(stdout, format, arg);
	done = vfprintf(File, format, arg);
	va_end (arg);
   return done;
}

