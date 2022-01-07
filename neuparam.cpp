#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <fstream>

#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "cJSON.h"
#include "neuparam.h"


#define NEU_PARA_PATH 	"/home/workdir/auto/neu_param.json"

Neupara_t neu_para_g;

int neu_para_init()
{
	FILE *fp = NULL;
	cJSON *configJSON = NULL,*item = NULL;
	char *jsonBuff = NULL;
	int len = 0;

	fp = fopen(NEU_PARA_PATH,"rb");
	if(fp == NULL)
		return -1;

	fseek(fp,0,SEEK_END);
	len = ftell(fp);
	fseek(fp,0,SEEK_SET);
	jsonBuff = (char*)malloc(len + 1);
	if(jsonBuff == NULL)
		return -2;
	fread(jsonBuff,len,1,fp);	
	fclose(fp);
	configJSON = cJSON_Parse(jsonBuff);
	if(configJSON == NULL){
		printf("json error!");
		return -3;
	}
	item = cJSON_GetObjectItem(configJSON, "cam_resolution");
    if (!item){

        neu_para_g.mCamResolution = 0;
    } else {
        neu_para_g.mCamResolution = item->valueint;
    }
	item = cJSON_GetObjectItem(configJSON,"cam_sermode");
	if(!item){
		neu_para_g.mCamServermode = 0;
	}else {
		neu_para_g.mCamServermode = item->valueint;
	}

	free(jsonBuff);
    cJSON_Delete(configJSON);

	if(neu_para_g.mCamResolution == 0){
		neu_para_g.mCamWidth = 640;
		neu_para_g.mCamHeight = 360;
	}else if(neu_para_g.mCamResolution == 1){
		neu_para_g.mCamWidth = 640;
		neu_para_g.mCamHeight = 480;
	}else if(neu_para_g.mCamResolution == 2){
		neu_para_g.mCamWidth = 1280;
		neu_para_g.mCamHeight = 720;
	}else if(neu_para_g.mCamResolution == 3){
		neu_para_g.mCamWidth = 1920;
		neu_para_g.mCamHeight = 1080;
	}
	
	return 0;
}




