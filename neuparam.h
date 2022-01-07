#ifndef _NEUPARAM_H_
#define _NEUPARAM_H_

typedef struct{
	unsigned int mCamResolution;
	unsigned int mCamServermode;
	unsigned int mCamWidth;
	unsigned int mCamHeight;
}Neupara_t;

int neu_para_init();

#endif
