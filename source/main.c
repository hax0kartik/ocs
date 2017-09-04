#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>
#include <fcntl.h>
#include "kernel.h"
#include "exploits.h"
#include "archive.h"
#include "httpc.h"
#include "fs.h"
//#include "links.h"
PrintConsole top, bottom;

/*
Code Plan:-

1.)Download the files for Step 1(excluding starter.zip)
1.b)Put the files in the correct directory, needs 7z and zip extractor
2.)Launch udsploit and then safehax, logging if the process went successfully
2b.)Safeb9s executes.After this the user should be running luma.
These steps are additional --------------------------------------------------------------------------
2c.)User executes soundhax again.
3.)Use Patch_KCL to srvall_init() and then init AM
4.)Install titles hblauncher_loader and then FBI
4a.)Download starter.zip
4b.)Replace the boot.3dsx in the sd root with  boot.3dsx from the pack
5.)Plan finish 
/*Code stolen shamelessly from https://github.com/yellows8/hblauncher_loader */
void downloadExtractStep1()
{
	printf("Downloading safeb9sinstaller\n");
	Result ret = httpDownloadData("https://github.com/d0k3/SafeB9SInstaller/releases/download/v0.0.7/SafeB9SInstaller-20170605-122940.zip");//safeb9sinstaller by d0k3
	printf("1: %08X\n", (unsigned int)ret);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "SafeB9SInstaller.bin", "safehaxpayload.bin","/");
	httpFree();
	printf("Downloading boot9strap\n");
	ret = httpDownloadData("https://github.com/SciresM/boot9strap/releases/download/1.2/boot9strap-1.2.zip");//b9s by scrisem
	printf("Download: %08X\n", (unsigned int)ret);
	mkdir("/boot9strap",0777);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm", "boot9strap.firm", "/boot9strap/");
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot9strap.firm.sha", "boot9strap.firm.sha", "/boot9strap/");
	httpFree();
	printf("Downloading luma\n");
	ret = httpDownloadData("https://github.com/AuroraWright/Luma3DS/releases/download/v8.1.1/Luma3DSv8.1.1.7z");//luma by aurorawright
	printf("3: %08X\n", (unsigned int)ret);
	archiveExtractFile(httpRetrieveData(), httpBufSize(), "boot.firm", "boot.firm", "/");
	httpFree();
}

void doExploitsStep1()
{
	Result ret = 1;
	while(ret > 0)
	{
		ret = udsploit();
		printf("Udsploit:%s|%08X",((ret == 0)?"Success":"Failure"), (unsigned int)ret); 
		if(ret == 0) 
			ret = hook_kernel();
		printf("hook_kernel:%s|%08lX",((ret == 0)?"Success":"Failure"), ret);
	}
	safehax();
}
int main()
{
	//preliminary stuff
	gfxInitDefault();
	consoleInit(GFX_TOP, &top);
	consoleSelect(&top);
	httpcInit(0);
	//printf("Downloading files for Step 1...\n");
	//downloadExtractStep1();
	printf("Running exploits\n");
	doExploitsStep1();
		while(aptMainLoop())
		{
			hidScanInput();
			
			if(hidKeysDown() & KEY_A)
				break;
			
		}
	httpcExit();	
	gfxExit();	
}