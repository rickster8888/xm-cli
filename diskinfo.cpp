#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <iostream>
#include "netsdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* RFC: TODO iterate through available disk and partitions */

/* These should be on commandline, environment variable and/or config file */
char *ip = (char*)"192.168.0.10"; // IP of your camera
char *user = (char*)"admin"; // username for camera (read access required)
char *password = (char*)"xxxxxx"; // password for camera

long g_LoginID=0;
//#define INCLUDE_NAT

int  main(int argc,char *argv[])
{
	H264_DVR_Init(NULL,0);
	printf("H264_DVR_Init\n");

	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;
	g_LoginID = H264_DVR_Login(ip, 34567, user, password,(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);	
	printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	
	SDK_StorageDeviceInformationAll diskinfo;
	DWORD dwRetLen = 0;
	int nWaitTime= 10000;
	H264_DVR_GetDevConfig(g_LoginID,E_SDK_CONFIG_DISK_INFO,0,(char*)&diskinfo,sizeof(SDK_StorageDeviceInformationAll),&dwRetLen,nWaitTime);
  	printf("Total Space: %d\n",diskinfo.vStorageDeviceInfoAll[0].diPartitions[0].uiTotalSpace);
	printf("Remain Space: %d\n",diskinfo.vStorageDeviceInfoAll[0].diPartitions[0].uiRemainSpace);
	if(g_LoginID>0)
	{
		H264_DVR_Logout(g_LoginID);
		printf("Logout success!!!\n");
	}
	H264_DVR_Cleanup();
	printf("H264_DVR_Cleanup\n");

	return 0;
}
