
#ifndef _H264LIVEVIDEOSOURCE_HH
#define _H264LIVEVIDEOSOURCE_HH

#include "FramedSource.hh"
#include "H264VideoFileServerMediaSubsession.hh"
#include "H264VideoStreamFramer.hh"


#define VIDEO_BUF_SIZE		(1024*1024)

class H264StreamFramBase: public H264VideoStreamFramer{
public:
	static H264StreamFramBase *createNew(UsageEnvironment &env,FramedSource *inputSrc,Boolean StartCodeInOut = false);
	struct timeval *GetPresentTime();
	struct timeval *GetNextPresentTime();
	double GetFramerate();
protected:
	H264StreamFramBase(UsageEnvironment &env,FramedSource *inputSrc,Boolean creaParsr,Boolean StartCodeInOut);
	~H264StreamFramBase();
};



class H264LiveVideoSource: public FramedSource{

public:
	static H264LiveVideoSource * createNew(UsageEnvironment &env,const char *channelID);
	void setBase(H264StreamFramBase *pBase) {m_pBase = pBase;}
private:
	virtual void doGetNextFrame();
	H264StreamFramBase *m_pBase;
	const char *fChanID;
	char *fTruncatedByte;
	unsigned int fTruncatedByteNum;

protected:
	H264LiveVideoSource(UsageEnvironment &env,const char *channelID = "720p-stream");
	virtual ~H264LiveVideoSource();
	

};


#endif 

