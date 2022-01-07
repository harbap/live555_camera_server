/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2021, Live Networks, Inc.  All rights reserved
// A test program that demonstrates how to stream - via unicast RTP
// - various kinds of file on demand, using a built-in RTSP server.
// main program

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <GroupsockHelper.hh> // for "weHaveAnIPv*Address()"
#include <H264FramedLiveSource.hh>
#include "H264LiveMediaSubsession.hh"
#include "neuparam.h"
#include <sys/types.h>  
#include <sys/stat.h> 
#include <unistd.h>
#include "writelog.h"


UsageEnvironment* env;

// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":
Boolean reuseFirstSource = False;

// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;
class Device Camera; 

char const* input_camera_name = "/tmp/fifo";


static char newDemuxWatchVariable;


void announceURL(RTSPServer* rtspServer, ServerMediaSession* sms) {
  if (rtspServer == NULL || sms == NULL) return; // sanuty check

  UsageEnvironment& env = rtspServer->envir();
  char buf[200] = {0};

  sprintf(buf,"Play this stream using the URL ");
  if (weHaveAnIPv4Address(env)) {
    char* url = rtspServer->ipv4rtspURL(sms);
    sprintf(buf + strlen(buf),"\" %s \"",url); 
    delete[] url;
    if (weHaveAnIPv6Address(env)) 
		sprintf(buf + strlen(buf), " or ");
  }
  if (weHaveAnIPv6Address(env)) {
    char* url = rtspServer->ipv6rtspURL(sms);

    sprintf(buf + strlen(buf),"\" %s \" ",url);
    delete[] url;
  }
  sprintf(buf + strlen(buf),"\n");
  env << buf;
  print_log(buf);
}

static void announceStream(RTSPServer* rtspServer, ServerMediaSession* sms,
			   char const* streamName, char const* inputFileName) {
  UsageEnvironment& env = rtspServer->envir();

  env << "\n\"" << streamName << "\" stream, from the file \""
      << inputFileName << "\"\n";
  announceURL(rtspServer, sms);
}



static MatroskaFileServerDemux* matroskaDemux;
static void onMatroskaDemuxCreation(MatroskaFileServerDemux* newDemux, void* /*clientData*/) {
  matroskaDemux = newDemux;
  newDemuxWatchVariable = 1;
}

static OggFileServerDemux* oggDemux;
static void onOggDemuxCreation(OggFileServerDemux* newDemux, void* /*clientData*/) {
  oggDemux = newDemux;
  newDemuxWatchVariable = 1;
}
#if  0				/**************file Stream****************/
int main(int argc, char** argv) {
  // Begin by setting up our usage environment:
  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  env = BasicUsageEnvironment::createNew(*scheduler);

  UserAuthenticationDatabase* authDB = NULL;
#ifdef ACCESS_CONTROL
  // To implement client access control to the RTSP server, do the following:
  authDB = new UserAuthenticationDatabase;
  authDB->addUserRecord("username1", "password1"); // replace these with real strings
  // Repeat the above with each <username>, <password> that you wish to allow
  // access to the server.
#endif


  Camera.Init();
  mkfifo(input_camera_name, 0777);
  if(0 == fork())
  {
	Camera.pipe_fd = fopen(input_camera_name, "w");
	if(NULL == Camera.pipe_fd)
	{
		printf("===============child process open pipe err =======\n ");
	}
	while(1)
	{
		usleep(15000);
		Camera.getnextframe();
	}
	
  }




  // Create the RTSP server:
  RTSPServer* rtspServer = RTSPServer::createNew(*env, 8554, authDB);
  if (rtspServer == NULL) {
    *env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
    exit(1);
  }

  char const* descriptionString
    = "Session streamed by \"testOnDemandRTSPServer\"";

  // Set up each of the possible streams that can be served by the
  // RTSP server.  Each such stream is implemented using a
  // "ServerMediaSession" object, plus one or more
  // "ServerMediaSubsession" objects for each audio/video substream.

  

  // A H.264 video elementary stream:
  {
    char const* streamName = "h264ESVideoTest";
    
    ServerMediaSession* sms
      = ServerMediaSession::createNew(*env, streamName, streamName,
				      descriptionString);
    sms->addSubsession(H264VideoFileServerMediaSubsession
		       ::createNew(*env, input_camera_name, reuseFirstSource));
    rtspServer->addServerMediaSession(sms);

    announceStream(rtspServer, sms, streamName, input_camera_name);
  }

 
  // Also, attempt to create a HTTP server for RTSP-over-HTTP tunneling.
  // Try first with the default HTTP port (80), and then with the alternative HTTP
  // port numbers (8000 and 8080).

  if (rtspServer->setUpTunnelingOverHTTP(80) || rtspServer->setUpTunnelingOverHTTP(8000) || rtspServer->setUpTunnelingOverHTTP(8080)) {
    *env << "\n(We use port " << rtspServer->httpServerPortNum() << " for optional RTSP-over-HTTP tunneling.)\n";
  } else {
    *env << "\n(RTSP-over-HTTP tunneling is not available.)\n";
  }

  env->taskScheduler().doEventLoop(); // does not return

  return 0; // only to prevent compiler warning
}
#else
/*******************************frame Stream*************************************/
extern Neupara_t neu_para_g;
#define CONFIG_RTSP_VERSION			"RTSP_V1.0"


int main(int argc,char **argv)
{
	int ret = 0;

	print_log("welcom to use neu rtsp server\n");
	print_log("sotf version:%s\n",CONFIG_RTSP_VERSION);
	print_log("compile date:%s %s\n",__DATE__,__TIME__);
	print_log("neu para init...\n");
	ret = neu_para_init();
	if(ret < 0){
		print_log("neu param is not exist!");
		exit(-1);
	}
	if(neu_para_g.mCamServermode != 1){
		print_log("camera mode is not rtsp mode!\n");
		exit(-2);
	}
	sleep(1);
	TaskScheduler *sch = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*sch);
	UserAuthenticationDatabase *authDb = NULL;

	RTSPServer *rtsp_srv = RTSPServer::createNew(*env, 8554, authDb);
	if(rtsp_srv == NULL){
		print_log("Failed to create rtsp server:%s\n",env->getResultMsg());
		exit(1);
	}
	const char *des_str = "Session streamed by\"720p-stream\"";
	const char *strname = "720p-stream";

	Camera.Init();
	
	ServerMediaSession *sms = ServerMediaSession::createNew(*env, strname, strname, des_str, False);
	sms->addSubsession(H264LiveMediaSubsession::createNew(*env,strname,reuseFirstSource));
	rtsp_srv->addServerMediaSession(sms);
	announceStream(rtsp_srv, sms, strname, "NULL");
	if(rtsp_srv->setUpTunnelingOverHTTP(80)||rtsp_srv->setUpTunnelingOverHTTP(8000)||rtsp_srv->setUpTunnelingOverHTTP(8080)){
		print_log("(we use port %d for optional RTSP-over-HTTP tunneling.)\n",rtsp_srv->httpServerPortNum());
	}else{
		print_log("(RTSP-over-HTTP tunneling is not available.)\n");
	}
	env->taskScheduler().doEventLoop();
	print_log("Exit server.....!\n");
	return 0;
}
#endif

