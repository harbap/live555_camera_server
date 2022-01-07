
#include "H264LiveVideoSource.hh"
#include "H264FramedLiveSource.hh"
#include "writelog.h"

/***
StreamFramBase class
***/
H264StreamFramBase* H264StreamFramBase::createNew(UsageEnvironment &env,FramedSource *inputSrc,Boolean StartCodeInOut)
{
	return new H264StreamFramBase(env,inputSrc,true,StartCodeInOut);
}

H264StreamFramBase::H264StreamFramBase(UsageEnvironment &env,FramedSource *inputSrc,Boolean creaParsr,Boolean StartCodeInOut)
:H264VideoStreamFramer(env,inputSrc,creaParsr,StartCodeInOut,false)
{
	
}
H264StreamFramBase::~H264StreamFramBase()
{
}

struct timeval * H264StreamFramBase::GetPresentTime()
{
	return &fPresentationTime;
}
struct timeval * H264StreamFramBase::GetNextPresentTime()
{
	return &fNextPresentationTime;
}
double H264StreamFramBase::GetFramerate()
{
	return fFrameRate;
}

/****
LiveVideoSource class
****/

H264LiveVideoSource *H264LiveVideoSource::createNew(UsageEnvironment &env ,const char *channelID)
{
	return new H264LiveVideoSource(env,channelID);
}
H264LiveVideoSource::H264LiveVideoSource(UsageEnvironment &env,const char *channelID)
:FramedSource(env)
{
	fChanID = strDup(channelID);
	env << "H264LiveVideoSource::fChanID:" << fChanID << "\n";
	fTruncatedByte = new char[VIDEO_BUF_SIZE];
	fTruncatedByteNum = 0;
}
H264LiveVideoSource::~H264LiveVideoSource()
{
	delete[] fChanID;
	delete[] fTruncatedByte;
}
extern class Device Camera; 
void H264LiveVideoSource::doGetNextFrame()
{
	static int i = 0;
	int ret = 0;
	
	i ++;
	fFrameSize = 0;
	ret = Camera.getnextframe();
	if(ret == 0){
		if(i % 7 == 0){
			//printf("fMaxSize:%d\n",fMaxSize);
			printf("frame_len:%d\n",Camera.frame_len);
		}
		if(Camera.frame_len > 100000){
			printf("---warning frame more than 100000\n");
			print_log("frame_len:%d %d\n",Camera.frame_len,fMaxSize);
		}
		
		fFrameSize = Camera.frame_len;
		if(fFrameSize > fMaxSize){
			fFrameSize = fMaxSize;
			int frBufUsedBytes = fMaxSize;
			fNumTruncatedBytes = Camera.frame_len - frBufUsedBytes;
			//printf("Truncat %d Bytes\n",fNumTruncatedBytes);
			memmove(fTo,Camera.h264_buf,frBufUsedBytes);
			memmove(fTruncatedByte,Camera.h264_buf + frBufUsedBytes,fNumTruncatedBytes);
			fTruncatedByteNum = fNumTruncatedBytes;
		}else{
			if(fTruncatedByteNum > 0){
				memmove(fTo,fTruncatedByte,fTruncatedByteNum);
				memmove(fTo + fTruncatedByteNum,Camera.h264_buf,Camera.frame_len);
				fFrameSize += fTruncatedByteNum;
				//printf("send last truncated %d bytes\n",fTruncatedByteNum);
				fTruncatedByteNum = 0;
			}else{
				memmove(fTo,Camera.h264_buf,Camera.frame_len);
			}
		}
		
	}else{
		handleClosure();
		return;
	}
	#if 0
	fDurationInMicroseconds = 1000000 / 20;
	struct timeval *nextPT = m_pBase->GetNextPresentTime();
	gettimeofday(&fPresentationTime,NULL);
	*nextPT = fPresentationTime;
	#endif
	nextTask() = envir().taskScheduler().scheduleDelayedTask(0,(TaskFunc *)FramedSource::afterGetting,this);
}



















