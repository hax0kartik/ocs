#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include "httpc.h"
#include "jsmn.h"

#pragma once 

typedef struct 
{
    char magic[4];
    u8 versionMajor;
    u8 versionMinor;
	
}lumainfo;

char *parseApi(const char *url, const char *format);

void progressbar(const char *string, double update, double total, bool progBarTotal);