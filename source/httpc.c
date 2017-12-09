#include <3ds.h>
#include <stdlib.h>
#include <stdio.h>
#include "httpc.h"
#include "util.h"
u8 *buf;
u8 *lastbuf = NULL;
u32 size=0, readsize=0;

Result httpcDownloadDataTimeout(httpcContext *context, u8* buffer, u32 size, u32 *downloadedsize, u64 timeoutsecs)
{
	Result ret=0;
	Result dlret=HTTPC_RESULTCODE_DOWNLOADPENDING;
	u32 pos=0, sz=0;
	u32 dlstartpos=0;
	u32 dlpos=0;

	if(downloadedsize)*downloadedsize = 0;

	ret=httpcGetDownloadSizeState(context, &dlstartpos, NULL);
	if(R_FAILED(ret))return ret;

	while(pos < size && dlret==HTTPC_RESULTCODE_DOWNLOADPENDING)
	{
		sz = size - pos;

		dlret=httpcReceiveDataTimeout(context, &buffer[pos], sz, timeoutsecs);
		if(dlret == HTTPC_RESULTCODE_TIMEDOUT)
		{
			return dlret;
		}
		ret=httpcGetDownloadSizeState(context, &dlpos, NULL);
		if(R_FAILED(ret))return ret;

		pos = dlpos - dlstartpos;
	}

	if(downloadedsize)*downloadedsize = pos;

	return dlret;
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

	ret = httpcAddRequestHeaderField(&context, (char*)"User-Agent", (char*)"OCS");
	if(ret>0)return ret;
	
	ret = httpcSetSSLOpt(&context, 1<<9);
	if(ret>0)return ret;

	ret = httpcBeginRequest(&context);
	if(ret>0)return ret;
	
	ret = httpcGetResponseStatusCode(&context, &statuscode);
	if(ret>0)return ret;

	if((statuscode >=301 && statuscode <=303) || (statuscode >= 307 && statuscode <= 308))
	{
		char newurl[1024];
		httpcGetResponseHeader(&context, (char*)"Location", newurl, 1024);
		httpcCloseContext(&context);
		return httpDownloadData(newurl);
	}
	ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret>0)return ret;
	do {
        ret = httpcDownloadDataTimeout(&context, buf+size, 0x1000, &readsize, 2.3e+11);
        size += readsize;
		progressbar("Download:", size, contentsize, false);
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
		//if(buf != NULL) free(buf);
        httpcCloseContext(&context);
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