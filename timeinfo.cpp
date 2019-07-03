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

char *ip = getenv("IPCAMERA_TARGET");         // IP of your camera
char *user = getenv("IPCAMERA_USERNAME");     // username for camera (read access required)
char *password = getenv("IPCAMERA_PASSWORD"); // password for camera
int port = getenv("IPCAMERA_PORT") ? atoi(getenv("IPCAMERA_PORT")) : 34567;

long g_LoginID=0;
//#define INCLUDE_NAT

int  main(int argc,char *argv[])
{

        /* RFC: get options */
        int c;
        while ((c =getopt(argc,argv,"u:p:t:P:")) != -1) {
                switch(c) {
                        case 'u':
                                user = optarg;
                                break;
                        case 'p':
                                password = optarg;
                                break;
                        case 't':
                                ip = optarg;
                                break;
                        case 'P':
                                port = atoi(optarg);
                                break;
                        default:
                                exit(1);

                }
        }

	H264_DVR_Init(NULL,0);
	fprintf(stderr,"H264_DVR_Init\n");

	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;
	g_LoginID = H264_DVR_Login(ip, port, user, password,(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);
	fprintf(stderr,"g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	
        if(g_LoginID == 0) {
                fprintf(stderr,"Error with login, error code is %d\n",nError);
                H264_DVR_Cleanup();
                exit(1);
        }

	SDK_NetNTPConfig ntpconfig;

	DWORD dwRetLen = 0;
	int nWaitTime= 10000;
	H264_DVR_GetDevConfig(g_LoginID,E_SDK_CONFIG_NET_NTP,0,(char*)&ntpconfig,sizeof(SDK_NetNTPConfig),&dwRetLen,nWaitTime);
	printf("Enabled: %d\n",ntpconfig.Enable);
	printf("UpdatePeriod: %d\n",ntpconfig.UpdatePeriod);
	printf("TimeZone: %d\n",ntpconfig.TimeZone);
	printf("Debug:%s|%d\n",ntpconfig.Server.ServerName, ntpconfig.Server.ip.l);

 	// Hack it
	strcpy(ntpconfig.Server.ServerName,"ntp1.isp.sky.com");
	ntpconfig.Server.ip.c[0] = 90;
	ntpconfig.Server.ip.c[1] = 207;
	ntpconfig.Server.ip.c[2] = 238;
	ntpconfig.Server.ip.c[3] = 105;

        printf("TimeZone: %d\n",ntpconfig.TimeZone);
        printf("Debug:%s|%d\n",ntpconfig.Server.ServerName, ntpconfig.Server.ip.l);

	/* DNS check */
	SDK_NetDNSConfig dnsconfig;
	H264_DVR_GetDevConfig(g_LoginID,E_SDK_CONFIG_NET_DNS,0,(char*)&dnsconfig,sizeof(SDK_NetDNSConfig),&dwRetLen,nWaitTime);

	printf("Debug:%d, %d.%d.%d.%d\n",dnsconfig.PrimaryDNS.l,dnsconfig.PrimaryDNS.c[0],dnsconfig.PrimaryDNS.c[1],dnsconfig.PrimaryDNS.c[2],dnsconfig.PrimaryDNS.c[3]);
	if(g_LoginID>0)
	{
		H264_DVR_Logout(g_LoginID);
		fprintf(stderr,"Logout success!!!\n");
	}
	H264_DVR_Cleanup();
	fprintf(stderr,"H264_DVR_Cleanup\n");

	return 0;
}
