#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "httpc.h"
u8 *buf;
u8 *lastbuf = NULL;
u32 size=0, readsize=0;

extern PrintConsole bottom, top;

void progressbar(const char *string, double update, double total)
{
	int nfill = 35;
	int totalfill = (int)((update/total)*(double)nfill);
	consoleSelect(&bottom);
	printf("\x1b[14;0H%s%3.2f%% Complete   \x1b[15;0H[", string,((update/total)*100.0));
	for(int a = 0; a < totalfill; a++)
		printf("=");
	
	printf(">");
	printf("%*s%s", nfill - totalfill, "", "]"); 
	consoleSelect(&top);
}
 
Result httpDownloadData(const char* url)
{
	httpcContext context;
	u32 statuscode, contentsize;
	Result ret;
	buf = (u8*)malloc(0x1000);
	if(buf == NULL) return -1;
	size = 0, readsize = 0;
	ret = httpcOpenContext(&context, HTTPC_METHOD_GET, url, 0);
	if(ret>0)return ret;
	
	ret = httpcSetSSLOpt(&context, 1<<9);
	if(ret>0)return ret;
	
	ret = httpcBeginRequest(&context);
	if(ret>0)return ret;
	
	ret = httpcGetResponseStatusCode(&context, &statuscode);
	if(ret>0)return ret;
	
	printf("StatusCode :%d\n", (int)statuscode);
	if((statuscode >=301 && statuscode <=303) || (statuscode >= 307 && statuscode <= 308))
	{
		char newurl[1024];
		httpcGetResponseHeader(&context, (char*)"Location", newurl, 1024);
		httpcCloseContext(&context);
		printf("newurl : %s\n", newurl);
		return httpDownloadData(newurl);
	}
	ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret>0)return ret;
	
	do {
        ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
        size += readsize; 
		progressbar("Download:", size, contentsize);
        if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
                lastbuf = buf; 
                buf = (u8*)realloc(buf, size + 0x1000);
                if(buf==NULL){ 
                    httpcCloseContext(&context);
                    free(lastbuf);
                    return -1;
                }
            }
    } while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING); 
	
	if(ret!=0){
        httpcCloseContext(&context);
        free(buf);
        return -1;
    }

    lastbuf = buf;
    buf = (u8*)realloc(buf, size);
    if(buf==NULL){ 
        httpcCloseContext(&context);
        free(lastbuf);
        return -1;
    }
	httpcCloseContext(&context);
	//free(buf);
	return 0;
}

u8 * httpRetrieveData()
{
	return buf;
}

size_t httpBufSize()
{
	return size;
}

void httpFree()
{
	//if(lastbuf!=NULL) free(lastbuf);
	free(buf);
}