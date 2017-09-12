#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <fcntl.h>
#include "kernel.h"
#include "asm.h"
#include "exploits.h"
#include "archive.h"
#include "log.h"
#include "httpc.h"
#include "fs.h"
#include "jsmn.h"

extern void progressbar(const char *string, double update, double total, bool progBarTotal);
PrintConsole top, bottom;

#define result(str,ret,steps,step_count) print("Result for %s:",str); \
if(ret == 0) \
{	\
	progressbar("Total Progress:", step_count, steps, true);	\
	print("\x1b[1;32mSuccess\x1b[1;37m\n");	\
}	\
else	\
{	\
	print("\x1b[1;31mFail: %08lX\x1b[1;37m\n", ret);	\
}	

/*
Code Plan:-

1.)Download the files for Step 1(excluding starter.zip)
1.b)Put the files in the correct directory, needs 7z and zip extractor
2.)Launch udsploit and then safehax, logging if the process went successfully
2b.)Safeb9s executes.After this the user should be running luma.
These steps are additional --------------------------------------------------------------------------
2c.)User executes soundhax again.
3.)Init AM
4.)Install titles hblauncher_loader and then FBI
4a.)Download starter.zip
4b.)Replace the boot.3dsx in the sd root with  boot.3dsx from the pack
5.)Plan finish
*/
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
					strcpy(returnDownloadUrl, downloadUrl);
					printf("Downloading the latest release\n");
				}
			}
		}
		httpFree();
		return returnDownloadUrl;
	}

void downloadExtractStep1()
{
	progressbar("Total Progress:", 0, 5, true);
	printf("Downloading safeb9sinstaller\n");
	Result ret = httpDownloadData(parseApi("https://api.github.com/repos/d0k3/SafeB9SInstaller/releases/latest", ".zip"));//safeb9sinstaller by d0k3
	result("Safeb9sinstaller Download", ret, 5, 1);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "SafeB9SInstaller.bin", "safehaxpayload.bin","/");
	httpFree();
	printf("Downloading boot9strap\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/SciresM/boot9strap/releases/latest",".zip"));//b9s by scrisem
	result("b9s Download", ret, 5, 2);
	mkdir("/boot9strap",0777);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm", "boot9strap.firm", "/boot9strap/");
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm.sha", "boot9strap.firm.sha", "/boot9strap/");
	httpFree();
	printf("Downloading luma\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/AuroraWright/Luma3DS/releases/latest", ".7z"));//luma by aurorawright
	result("Luma Download", ret, 5, 3);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot.firm", "boot.firm", "/");
	httpFree();
}

void ciaInstall(void *data, u32 size)
{
	Handle cia;
	Result ret = amInit();
	print("Result for %s:","amInit"); (ret == 0) ? print("\x1b[1;32mSuccess\x1b[1;37m\n"):print("\x1b[1;31mFail: %08lX\x1b[1;37m\n", ret);
	AM_InitializeExternalTitleDatabase(false);
	ret = AM_StartCiaInstall(MEDIATYPE_SD, &cia);
	print("Result for %s:","Start_CiaInstall"); (ret == 0) ? print("\x1b[1;32mSuccess\x1b[1;37m\n"):print("\x1b[1;31mFail: %08lX\x1b[1;37m\n", ret);
	ret = FSFILE_Write(cia, NULL, 0, data, size, 0);
	print("Result for %s:","CIA write"); (ret == 0) ? print("\x1b[1;32mSuccess\x1b[1;37m\n"):print("\x1b[1;31mFail: %08lX\x1b[1;37m\n", ret);
	ret = AM_FinishCiaInstall(cia);
	print("Result for %s:","Finish Cia Install"); (ret == 0) ? print("\x1b[1;32mSuccess\x1b[1;37m\n"):print("\x1b[1;31mFail: %08lX\x1b[1;37m\n", ret);
	amExit();
}

void doExploitsStep1()
{
	Result ret = 1;
	while(ret > 0)
	{
		ret = udsploit();
		result("Udsploit", ret, 5, 4);
		if(ret == 0)
			ret = hook_kernel();
		result("hook_kernel", ret, 5, 5);
	}
	safehax();
}

void downloadExtractStep2()
{
	progressbar("Total Progress:", 0, 9, true);
	printf("Downloading  and Installing lumaupdater\n");
	Result ret = httpDownloadData(parseApi("https://api.github.com/repos/KunoichiZ/lumaupdate/releases/latest", ".cia")); //lumaupdater by hamcha & KunoichiZ
	result("Download", ret, 9, 1);
	ciaInstall(httpRetrieveData(), httpBufSize());
	httpFree();
	printf("Downloading and Installing DSP1\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/zoogie/DSP1/releases/latest", ".cia"));//DSP1 by zoogie
	result("Download", ret, 9, 2);
	ciaInstall(httpRetrieveData(), httpBufSize());
	httpFree();
	printf("Downloading and Installing Anemone3DS\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/astronautlevel2/Anemone3DS/releases/latest", ".cia"));//Anemone3ds by AstronaultLevel2
	result("Download", ret, 9, 3);
	ciaInstall(httpRetrieveData(), httpBufSize());
	httpFree();
	printf("Downloading and Installing FBI\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/steveice10/FBI/releases/latest", ".cia"));//FBI by steveice10
	result("Download", ret, 9, 4);
	ciaInstall(httpRetrieveData(), httpBufSize());
	httpFree();
	printf("Downloading boot.3dsx\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/fincs/new-hbmenu/releases/latest", ".3dsx"));// By smealum & others
	result("Download", ret, 9, 5);
	fsOpenAndWrite("/boot.3dsx",httpRetrieveData(), httpBufSize());
	httpFree();
	printf("Downloading godmode9\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/d0k3/GodMode9/releases/latest", ".zip"));// By d0k3
	result("Download", ret, 9, 6);
	mkdir("/luma/payloads", 0777);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "GodMode9.firm", "GodMode9.firm", "/luma/payloads/");
	printf("Downloading godmode9 sd card cleaup script\n");
	ret = httpDownloadData("http://3ds.guide/gm9_scripts/cleanup_sd_card.gm9"); //By d0k3
	result("Download", ret, 9, 7);
	mkdir("/gm9",0777);
	mkdir("/gm9/scripts", 0777);
	fsOpenAndWrite("/gm9/scripts/cleanup_sd_card.gm9", httpRetrieveData(), httpBufSize());
	printf("Downloading godmode9 ctr-nand luma script\n");
	ret = httpDownloadData("http://3ds.guide/gm9_scripts/setup_ctrnand_luma3ds.gm9"); //By d0k3
	result("Download", ret, 9, 8);
	fsOpenAndWrite("/gm9/scripts/setup_ctrnand_luma3ds.gm9", httpRetrieveData(), httpBufSize());
	//Best time to install hblauncher_loader
	printf("Downloading hblauncher_loader\n");
	ret = httpDownloadData(parseApi("https://api.github.com/repos/yellows8/hblauncher_loader/releases/latest", ".zip"));//hblauncher_loader by yellows8
	result("Download", ret, 9, 9);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "hblauncher_loader.cia", "hblauncher_loader.cia", "/");
	httpFree();
	u32 size;
	u8 *data = fsOpenAndRead("/hblauncher_loader.cia", &size);
	printf("Trying to install hblauncher_loader.cia\n");
	ciaInstall(data, size);
	free(data);
}

int main()
{
	//preliminary stuff
	gfxInitDefault();
	consoleInit(GFX_TOP, &top);
	consoleInit(GFX_BOTTOM, &bottom);
	consoleSelect(&bottom);
	printf("\x1b[1;37m");
	printf("Welcome to \x1b[1mOCS!!\x1b[0m\nMade by:- \x1b[1;32mKartik\x1b[1;37m\nSpecial Thanks to :-\n\x1b[1;33mChromaryu\x1b[1;37m:- For Testing\n\x1b[1;35mSmealum\x1b[1;37m and \x1b[1;33myellows8\x1b[1;37m:- For udsploit\n\x1b[1;36mTinivi\x1b[1;37m for safehax");
	consoleSelect(&top);
	printf("\x1b[1;37m");
	bool cfwflag = false;
	acWaitInternetConnection();
	printf("Press A to begin\n");
	while(aptMainLoop())
		{
			hidScanInput();

			if(hidKeysDown() & KEY_A)
				break;

		}
	printf("Checking if cfw is installed\n");
	Result ret = checkRunningCFW();
	(ret == 0xF8C007F4) ? (cfwflag = false) : (cfwflag = true);
	consoleSelect(&top);
	httpcInit(0);
	if(cfwflag == false)
	{
		printf("Not running cfw\n");
		if(checkFileExists("/safehaxpayload.bin") == 0) //check if files already exsist for step 1.
		{	
			printf("Downloading files for CFW installation\n");
			downloadExtractStep1();
		}	
		printf("Running exploits\n");
		doExploitsStep1();
	}
	else
	{
		//User is running luma cfw
		printf("Running cfw\n");
		printf("Downloading files for Step 2...\n");
		//parseApi("https://api.github.com/repos/pirater12/ocs/releases/latest");
		downloadExtractStep2();
		printf("Proccess Finished. Press Start to exit and enjoy\n");
	}
		while(aptMainLoop())
		{
			hidScanInput();

			if(hidKeysDown() & KEY_START)
				break;

		}
	httpcExit();
	gfxExit();
}
