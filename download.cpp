#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <iostream>
#include "netsdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

// RFC: No docs on how to disable verbose debugging. 
// RFC: based on demo code.. this is a mainly C with some C++ constructs.

char *ip = getenv("IPCAMERA_TARGET");         // IP of your camera
char *user = getenv("IPCAMERA_USERNAME");     // username for camera (read access required)
char *password = getenv("IPCAMERA_PASSWORD"); // password for camera
int port = getenv("IPCAMERA_PORT") ? atoi(getenv("IPCAMERA_PORT")) : 34567;

/* Globals! Eek */
int rv;
int progress;
long cplayid;

//typedef void(CALL_METHOD *fDownLoadPosCallBack) (long lPlayHandle, long lTotalSize, long lDownLoadSize, long dwUser);
void TestCallBack( long a, long b, long c, long d) 
{
	rv = c;
	cplayid = a;
	printf("\rProgress %d,%d,%d,%d",a,b,c,d);
}

long g_LoginID=0;
//#define INCLUDE_NAT

int  main(int argc,char *argv[])
{
	/* RFC: get options */
	int c;
	int scanonly = 0;
	int keepexisting = 0;
	int limit = 64;
	char *dl = NULL;
	char *date = NULL;
	char *end = NULL;
	while ((c =getopt(argc,argv,"l:d:e:ski:u:p:t:P:")) != -1) {
		switch(c) {
			case 'i':
				limit = atoi(optarg);
				break;
			case 'l':
				dl = optarg;
				break;
			case 'd':
				date = optarg;
				break;
			case 's':
				scanonly =1;
				break;
			case 'k':
				keepexisting =1;
				break;
			case 'e':
				end = optarg;	
				break;
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

	if(limit <1) {
		printf("Invalid limit value (%d) provided\n",limit);
		exit(1);
	}

	if(!dl || !date) {
		printf("No details provided\n");
		exit(1);
	}
	printf("Download directory is %s for the date %s\n",dl,date);
	struct stat sb;
	if(stat(dl,&sb) < 0) {
		perror("Directory issue");
		exit(1);
	}

	if(!S_ISDIR(sb.st_mode)) {
		printf("%s not a directory, aborting\n",dl);
		exit(1);
	}

	
	int yr,mon,dd=0;
	int yr2,mon2,dd2=0;

	/* today, yesterday - supported */
	/* TODO: week, <num>d format (e.g. 3d for 3 days) */
	if(strcmp(date,"today") && strcmp(date,"yesterday") &&
           (sscanf(date,"%d-%d-%d",&yr,&mon,&dd) != 3)) {
		printf("Failure to parse date\n");
		exit(1);
	}

	if(end) {
		if(strcmp(end,"today") && strcmp(end,"yesterday") &&
			(sscanf(end,"%d-%d-%d",&yr2,&mon2,&dd2) != 3)) {
			printf("Failure to parse end date\n");
			exit(1);
        	}
	}
	H264_DVR_Init(NULL,0);
	printf("H264_DVR_Init\n");

	H264_DVR_DEVICEINFO OutDev;	
	memset(&OutDev,0,sizeof(OutDev));
	int nError = 0;
	g_LoginID = H264_DVR_Login(ip, port, user, password,(LPH264_DVR_DEVICEINFO)(&OutDev),&nError);	
	printf("g_LoginID=%d,nError:%d\n",g_LoginID,nError);
	
	if(g_LoginID == 0) {
		fprintf(stderr,"Error with login, error code is %d\n",nError);
		H264_DVR_Cleanup();
		exit(1);
	}

	H264_DVR_FINDINFO findInfo;
	findInfo.nChannelN0=0;
	findInfo.nFileType=0;
	
	if (dd == 0) { // not very robust method!
		time_t t;
		tm *tt;

		t = time(NULL);
		if(!strcmp(date,"yesterday"))
			t -= 86400;

		tt = localtime(&t);
	
		findInfo.startTime.dwYear = tt->tm_year+1900;
		findInfo.startTime.dwMonth = tt->tm_mon+1;
		findInfo.startTime.dwDay = tt->tm_mday;

		if(!end) {
			findInfo.endTime.dwYear = tt->tm_year+1900;
			findInfo.endTime.dwMonth = tt->tm_mon+1;
			findInfo.endTime.dwDay = tt->tm_mday;
		}
	} else {
		findInfo.startTime.dwYear = yr;
		findInfo.startTime.dwMonth = mon;
		findInfo.startTime.dwDay = dd;

		if(!end) {
			findInfo.endTime.dwYear = yr;
			findInfo.endTime.dwMonth = mon;
			findInfo.endTime.dwDay = dd;
		}
	}

	/* set up end */
	if(end) {
		if(dd2 == 0) { // not very robust method
			time_t t = time(NULL);
			tm *tt;

			if(!strcmp(end,"yesterday"))
				t -= 86400;

			tt = localtime(&t);

			findInfo.endTime.dwYear = tt->tm_year+1900;
			findInfo.endTime.dwMonth = tt->tm_mon+1;
			findInfo.endTime.dwDay = tt->tm_mday;
		} else {
			findInfo.endTime.dwYear = yr2;
			findInfo.endTime.dwMonth = mon2;
			findInfo.endTime.dwDay = dd2;
		}
	}

	findInfo.startTime.dwHour = 0;
	findInfo.startTime.dwMinute = 0;
	findInfo.startTime.dwSecond = 0;

	findInfo.endTime.dwHour = 23;
	findInfo.endTime.dwMinute = 59;
	findInfo.endTime.dwSecond = 59;
	
	/* RFC: the max items does not do what you think you have to paginate to find items
 	        use own counter to determine if limit reached */

	H264_DVR_FILE_DATA pData[100];
	int nFindCount = 0;
	
	int processed = 0; // counter for how many processed, so we can limit the results;
	int downloaded = 0;

	while(processed < limit) {
		long lRet= H264_DVR_FindFile(g_LoginID, &findInfo, pData, 100, &nFindCount, 30000);

		if(lRet>0&&nFindCount>0)
		{
		   	printf("search success,playback file num=%d\n", nFindCount);
			int i;
			// /idea0/2018-05-03/001/06.38.38-06.39.06[M][@c3b][1].h264
		
			for(i=0;i<nFindCount;i++) {

				if(processed >= limit)
					break;

				char *tok;
				char *dup = strdup(pData[i].sFileName);
				tok = strtok(dup,"/");
				char *day = strtok(NULL,"/");
				strtok(NULL,"/");
				char *path = strtok(NULL,"/");

				char yyyy[5], mm[3], dd[3];
				printf("day=%s,path=%s\n",day,path);
				char fname[1024], tmp_fname[1024]; // tmp name

				if(sscanf(day,"%4s-%2s-%2s",&yyyy,&mm,&dd) == 3) {
					/* TDOO: only mkdir if dir doesn't exist */
					snprintf(fname,sizeof(fname)-1,"%s/%s",dl,yyyy);
					mkdir(fname,0777);
					snprintf(fname,sizeof(fname)-1,"%s/%s/%s",dl,yyyy,mm);
					mkdir(fname,0777);
					snprintf(fname,sizeof(fname)-1,"%s/%s/%s/%s",dl,yyyy,mm,dd);
					mkdir(fname,0777);
					snprintf(fname,sizeof(fname)-1,"%s/%s/%s/%s/%s-%s",dl,yyyy,mm,dd,day,path);
					snprintf(tmp_fname,sizeof(tmp_fname)-1,"%s/%s/%s/%s/.ipcamera-%s-%s.tmp",dl,yyyy,mm,dd,day,path);
				} else {
					snprintf(fname,sizeof(fname)-1,"%s/%s-%s",dl,day,path);
					snprintf(tmp_fname,sizeof(tmp_fname)-1,"%s/.ipcamera-%s-%s.tmp",dl,day,path);
				}
				printf("filename is %s to %s (size=%d)\n",pData[i].sFileName,fname,pData[i].size);
				//printf("tmp file is %s\n",tmp_fname);
	
				if(scanonly) {
					processed++;
					continue; // don't download
       	                 	}

				if(keepexisting) {
					if(stat(fname,&sb)==0) {
						if(S_ISREG(sb.st_mode)) {
							printf("file exists, skipping\n");
							processed++;
							continue;
						}
					}
				}

				rv = 0;
				cplayid = 0;
				lRet = H264_DVR_GetFileByName(g_LoginID,&pData[i],tmp_fname,TestCallBack,i,NULL);
				if (lRet > 0) {
					while(1) {
						//if(rv >= pData[i].size || rv == -1)
						if(rv == -1)
							break;
						sleep(1);
					}
					if(cplayid) {
						printf("\nStop Get %d\n",cplayid);
						H264_DVR_StopGetFile(cplayid);
					}
					printf("success\n");
					sleep(5);
					if(rename(tmp_fname,fname)) {
						perror("Rename");
						exit(1);
					}
					downloaded++;
				} else {
					printf("Failed\n");
				}
				free(dup);
				processed++;
			}
			/* Bug workaround: Searcg sometimes will find earlier elements, we break out
			   if we notice search start has not changed */
			if( findInfo.startTime.dwYear == pData[nFindCount-1].stEndTime.year &&
			    findInfo.startTime.dwMonth == pData[nFindCount-1].stEndTime.month &&
			    findInfo.startTime.dwDay == pData[nFindCount-1].stEndTime.day &&
			    findInfo.startTime.dwHour == pData[nFindCount-1].stEndTime.hour &&
			    findInfo.startTime.dwMinute == pData[nFindCount-1].stEndTime.minute &&
			    findInfo.startTime.dwSecond == pData[nFindCount-1].stEndTime.second ) {
				printf("Bug prevention: search unchanged.. exit\n");
				break;
			}
	
			/* RFC: sent end date of last found to start date of next search */
			/*	in testing if max number is 64, but we always check regardless */
			findInfo.startTime.dwYear = pData[nFindCount-1].stEndTime.year;
			findInfo.startTime.dwMonth = pData[nFindCount-1].stEndTime.month;
			findInfo.startTime.dwDay = pData[nFindCount-1].stEndTime.day;
			findInfo.startTime.dwHour = pData[nFindCount-1].stEndTime.hour;
			findInfo.startTime.dwMinute = pData[nFindCount-1].stEndTime.minute;
			findInfo.startTime.dwSecond = pData[nFindCount-1].stEndTime.second;

			/* RFC: Debug data */
			printf("%d:%d:%d\n",findInfo.startTime.dwHour,
					    findInfo.startTime.dwMinute,
					    findInfo.startTime.dwSecond);

		} else {
			break;
		}	
	}
	
	printf("Processed: %d\n",processed);
	printf("New downloads: %d\n",downloaded);
	if(g_LoginID>0)
	{
		H264_DVR_Logout(g_LoginID);
		printf("Logout success!!!\n");
	}
	H264_DVR_Cleanup();
	printf("H264_DVR_Cleanup\n");

	return 0;
}
