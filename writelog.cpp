#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "writelog.h"

#define  LOGFILE_MAX_SIZE				(1024*1024*50)

static const char *filePath_g = "/home/workdir/live555/log/";
static const char *fileName = "log_";

int open_logfile(FILE **retfp)
{
	FILE *fp = NULL;
	int res = 0;
	char logName[20] = {0},filePath[50] = {0},cmd[100] = {0};
	struct stat sb;
	time_t tt;
	struct tm* tm_ptr;

	time(&tt);
	tm_ptr = localtime(&tt);
	sprintf(logName,"%s%04d%02d.log",fileName,tm_ptr->tm_year + 1900,tm_ptr->tm_mon+1);
	sprintf(filePath,"%s%s",filePath_g, logName);

	res = stat(filePath_g,&sb);
	//printf("%s res = %d \n",filePath_g,res);
	if(res == -1){
		printf("file is not exist...\n");
		if(mkdir(filePath_g,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH) == -1){
			printf("%s create dir fail\n",filePath_g);
			return -1;
		}
	}else {
		res = stat(filePath,&sb);
		//printf("%s res = %d \n",filePath,res);
		if(res == 0){
			//printf("%s size:%d\n",filePath,sb.st_size);
			if(sb.st_size >= LOGFILE_MAX_SIZE){
				sprintf(filePath, "%s%04d%0d_%02d.log", 
				    fileName,tm_ptr->tm_year+1900, tm_ptr->tm_mon+1,tm_ptr->tm_mday);
			}
		}else{
			/*delete all logfile*/
			sprintf(cmd,"rm -f %s*.log",filePath_g);
			printf("delete all log files cmd:%s \n",cmd);
			fp = popen(cmd,"r");
			if(fp == NULL){
				printf("delete filed!\n");
			}else{
				printf("delete ok\n");
				fclose(fp);
			}
		}
	}
	
	fp = fopen(filePath,"a+");
	if(fp == NULL){
	}
	*retfp = fp;
	return 0;
	
}

void print_log(const char *fmt,...)
{
	va_list ap;
	char buf[1024] = {0},dt[100] = {0};
	FILE *fp;
	time_t tt;
	struct tm *tm_ptr = NULL;
	
	time(&tt);
	tm_ptr = localtime(&tt);
	if(open_logfile(&fp) != 0)
		return;
	memset(buf,0,sizeof(buf));
	va_start(ap,fmt);
	vsprintf(buf,fmt,ap);
	va_end(ap);

	memset(dt,0,sizeof(dt));
	sprintf(dt,"[%d-%d %d:%d]",tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min);

	fwrite(dt,1,strlen(dt),fp);
	fwrite(buf,1,strlen(buf),fp);
	fclose(fp);

	return;
}

void print_data(char * filename, char * data, int len)
{
	int i;
	FILE *fp;
	time_t tt;
	time(&tt);
	struct tm* tm_ptr = localtime(&tt);
	char dt[256] = {0};

	if(open_logfile(&fp) != 0)
		return;
	sprintf(dt,"[%d-%d %d:%d]",tm_ptr->tm_mon + 1, tm_ptr->tm_mday, tm_ptr->tm_hour, tm_ptr->tm_min);
	fwrite(dt,1,strlen(dt),fp);

	memset(dt,0,sizeof(dt));
	for(i = 0;i < len;i++){
		sprintf(dt + strlen(dt),"%02x ",(unsigned char)data[i]);
		if(((i+1) % 16) == 0){
			strcat(dt,"\n");
			fwrite(dt,1,strlen(dt),fp);
			dt[0] = 0x00;
		}
	}
	strcat(dt,"\n");
	fwrite(dt,1,strlen(dt),fp);
	fclose(fp);
}




