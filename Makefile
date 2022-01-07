
CROSS_COMPILE?=		arm-linux-gnueabihf-
ARCH		 ?=  arm

ifeq ($(CROSS_COMPILE),arm-linux-gnueabihf-)
OPENSSL_INCLUDE := -I/workdir/openssl/lib/include
OPENSSL_LIB := -L/workdir/openssl/lib/lib
endif


INCLUDES 	 = -I./include/live555/usageEnvironment/ -I./include/live555/groupsock/ \
				-I./include/live555/liveMedia/ -I./include/live555/basicUsageEnvironment  \
				-I./include/x264 -I./include/encoder -I./include/cJSON $(OPENSSL_INCLUDE)
LIVE555_LIBS =  ./lib/livelib/libliveMedia.a ./lib/livelib/libgroupsock.a \
				./lib/livelib/libBasicUsageEnvironment.a ./lib/livelib/libUsageEnvironment.a
X264_LIBS 	 =  ./lib/x264lib/libx264.a ./lib/x264lib/libx264.so.157
LIBS         =  $(LIVE555_LIBS) $(X264_LIBS) ./lib/cJSON/libcJSON.a $(OPENSSL_LIB) -lssl -lcrypto

COMPILE_OPTS =      $(INCLUDES) -I. -O2 -std=c++11 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64 -g
C 			 =         c
C_COMPILER   =        $(CROSS_COMPILE)gcc
C_FLAGS 	 =       $(COMPILE_OPTS) $(CPPFLAGS) $(CFLAGS)
CPP 		 =           cpp
CPLUSPLUS_COMPILER =    $(CROSS_COMPILE)g++
CPLUSPLUS_FLAGS =   $(COMPILE_OPTS) -Wall -DBSD=1 $(CPPFLAGS) $(CXXFLAGS)
OBJ 		 =           o
LINK 		 =  $(CROSS_COMPILE)g++ -o
LINK_OPTS    =  -ldl  -lm -lpthread -ldl -g
CONSOLE_LINK_OPTS = $(LINK_OPTS)
LIN1_OBJ 	 = test_h264.o 
LIN1_OBJ	+= sonix_xu_ctrls.o H264FramedLiveSource.o H264LiveMediaSubsession.o H264LiveVideoSource.o neuparam.o
LIN1_OBJ    += writelog.o 

APP1 = rtsp_server_arm

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<
.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

$(APP1): $(LIN1_OBJ)
	$(LINK) $@  $(LIN1_OBJ)  $(LIBS) $(CONSOLE_LINK_OPTS)	
	
	
clean:
	-rm -rf *.$(OBJ) $(APP) core *.core *~ include/*~

