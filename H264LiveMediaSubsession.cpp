#include "H264LiveMediaSubsession.hh"
#include "H264VideoRTPSink.hh"
#include "H264VideoStreamFramer.hh"
#include "H264VideoRTPSink.hh"
#include "H264LiveVideoSource.hh"
		  

H264LiveMediaSubsession* H264LiveMediaSubsession::createNew(UsageEnvironment &env,const char *charID,Boolean reuseFist)
{
	return new H264LiveMediaSubsession(env,charID,reuseFist);
}
H264LiveMediaSubsession::H264LiveMediaSubsession(UsageEnvironment &env,const char *charID,Boolean reuseFist)
:OnDemandServerMediaSubsession(env,reuseFist)
{
	fAuxSDPLine = NULL;
	fDoneFlag = 0;
	fDummyRTPSink = NULL;
	fChanID = strDup(charID);
	if(fChanID == NULL)
		fChanID = strDup("720-stream");
	env << "H264LiveMediaSubsession fChanID:" <<fChanID << "\n";
}

H264LiveMediaSubsession::~H264LiveMediaSubsession()
{
	if(fAuxSDPLine != NULL){
		delete[] fAuxSDPLine;
		fAuxSDPLine = NULL;
	}
	if(fChanID != NULL){
		delete[] fChanID;
		fChanID = NULL;
	}
}

static void checkForAuxSDPLine(void *clientData)
{
	H264LiveMediaSubsession *subsess = (H264LiveMediaSubsession*)clientData;
	subsess->checkforAuxSDPLine1();
}
static void afterPlayinDummy(void *clientData)
{
	H264LiveMediaSubsession *subsess = (H264LiveMediaSubsession*)clientData;
	subsess->afterPlayinDummy1();
}
void H264LiveMediaSubsession::afterPlayinDummy1()
{
	envir().taskScheduler().unscheduleDelayedTask(nextTask());
	fDoneFlag = 1;
}
void H264LiveMediaSubsession::checkforAuxSDPLine1()
{
	const char *das1;
	if(fAuxSDPLine != NULL){
		fDoneFlag = 1;
	}else if(fDummyRTPSink != NULL&&(das1 = fDummyRTPSink->auxSDPLine()) != NULL){
		fAuxSDPLine = strDup(das1);
		fDummyRTPSink = NULL;
		fDoneFlag = 1;
	}else if(!fDoneFlag){
		int usec = 10;	//10ms
		nextTask() = envir().taskScheduler().scheduleDelayedTask(usec, checkForAuxSDPLine, this);
	}
}

char const* H264LiveMediaSubsession::getAuxSDPLine(RTPSink* rtpSink, FramedSource* inputSource) 
{
	if(fAuxSDPLine != NULL)
		return fAuxSDPLine;
	if(fDummyRTPSink == NULL){
		fDummyRTPSink =rtpSink;
		fDummyRTPSink->startPlaying(*inputSource, afterPlayinDummy, this);
		checkForAuxSDPLine(this);
	}
	envir().taskScheduler().doEventLoop(&fDoneFlag);
}

FramedSource* H264LiveMediaSubsession::createNewStreamSource(unsigned /*clientSessionId*/, unsigned& estBitrate) 
{    
	printf("create stream source...\n");
	estBitrate = 500;
	H264LiveVideoSource *liveSrc = H264LiveVideoSource::createNew(envir(),fChanID);
	if(liveSrc == NULL)
		return NULL;
	H264StreamFramBase *pBase = H264StreamFramBase::createNew(envir(),liveSrc);
	liveSrc->setBase(pBase);
	return pBase;	
}

RTPSink* H264LiveMediaSubsession::createNewRTPSink(Groupsock* rtpGroupsock,unsigned char rtpPayloadTypeIfDynamic,FramedSource* /*inputSource*/) 
{
	printf("create Video RTSPSink\n");
	return H264VideoRTPSink::createNew(envir(), rtpGroupsock, rtpPayloadTypeIfDynamic);
}

