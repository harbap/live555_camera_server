
#ifndef _H264LIVE_MEDIA_SUBSESSION_H_
#define _H264LIVE_MEDIA_SUBSESSION_H_


#include "OnDemandServerMediaSubsession.hh"

class H264LiveMediaSubsession : public OnDemandServerMediaSubsession
{
public:
	static H264LiveMediaSubsession *createNew(UsageEnvironment &env,const char *charID,Boolean reuseFist);
	void checkforAuxSDPLine1();
	void afterPlayinDummy1();
protected:
	H264LiveMediaSubsession(UsageEnvironment &env,const char *charID,Boolean reuseFist);
	virtual ~H264LiveMediaSubsession();

	virtual char const* getAuxSDPLine(RTPSink* rtpSink,FramedSource* inputSource);
  	virtual FramedSource* createNewStreamSource(unsigned clientSessionId,		      unsigned& estBitrate);
  	virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,        unsigned char rtpPayloadTypeIfDynamic,FramedSource* inputSource);
private:
	const char *fChanID;
	char *fAuxSDPLine;
	char fDoneFlag;
	RTPSink *fDummyRTPSink;
};




#endif /*_H264LIVE_MEDIA_SUBSESSION_H_*/

