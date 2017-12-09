#include "util.h"

//Code from jsmn example
int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
		strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}


char *parseApi(const char *url, const char *format)
{
	printf("Parsing JSON to get latest release\n");
	Result ret = httpDownloadData(url);
	jsmn_parser p = {};
	jsmn_init(&p);
	static char downloadUrl[0x100], returnDownloadUrl[0x100];
	jsmntok_t t[512] = {};
	u8* apiReqData = httpRetrieveData();
	int r = jsmn_parse(&p, (const char *)apiReqData, httpBufSize(), t, sizeof(t) / sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON %d", r);
	}
	bool inassets = false;
	for (int i = 0; i < r; i++) {
		if (!inassets && jsoneq((const char*)apiReqData, &t[i], "assets") == 0) {
			inassets = true;
		}
		if (inassets) {
			if (jsoneq((const char*)apiReqData, &t[i], "browser_download_url") == 0) {
				sprintf(downloadUrl, "%.*s", t[i+1].end-t[i+1].start, apiReqData + t[i+1].start);
				if(strstr(downloadUrl, format) != NULL)
				{
					strcpy(returnDownloadUrl, downloadUrl);
					printf("Downloading the latest release\n");
				}		
			}
		}
	}
	httpFree();
	return returnDownloadUrl;
}

extern PrintConsole bottom, top;

void progressbar(const char *string, double update, double total, bool progBarTotal)
{
	int nfill = 35;
	int totalfill = (int)((update/total)*(double)nfill);
	consoleSelect(&bottom);
	if(progBarTotal == false)
		printf("\x1b[14;0H%s%3.2f%% Complete   \x1b[15;0H[", string,((update/total)*100.0));
	else
		printf("\x1b[17;0H%s%3.2f%% Complete   \x1b[18;0H[", string,((update/total)*100.0));
	for(int a = 0; a < totalfill; a++)
		printf("=");

	printf(">");
	printf("%*s%s", nfill - totalfill, "", "]");
	consoleSelect(&top);
}