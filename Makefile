
#CC = gcc
CC  = arm-poky-linux-gnueabi-gcc -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/poky/1.7/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi
CXX = arm-poky-linux-gnueabi-g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/poky/1.7/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi

CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)

INCPATH      =   -I ./httpssl/include  -I ./include/ -I ./HttpServer/include

LINK          = arm-poky-linux-gnueabi-g++ -march=armv7-a -mthumb-interwork -mfloat-abi=hard -mfpu=neon -mtune=cortex-a9 --sysroot=/opt/poky/1.7/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi

LFLAGS        = -Wl,-O1 -Wl,-rpath,/opt/poky/1.7/sysroots/cortexa9hf-vfp-neon-poky-linux-gnueabi/lib

LIBS          =  $(SUBLIBS) -lssl -lcrypto ./libcurl/libcurl.so  ./HttpServer/lib/libevent-2.1.so.6 ./lib/libnetsnmp.a ./lib/libnetsnmptrapd.a  -ldl  -lrt -lpthread

TARGET        = tranter


OBJECTS       =  build/main.o     \
                 build/comserver.o  \
                 build/err_com.o	\
                 build/rs485Com3.o  \
                 build/rs485Com4.o  \
				 build/rs485server.o  \
				 build/net_spd.o  \
                 build/comport.o  \
                 build/HttpPost.o  \
                 build/IpcamServer.o  \
                 build/HttpServer.o \
                 build/snmp.o \
                 build/mywalkapp.o \
                 build/AtlasServer.o \
                 build/snmptrap.o \
                 build/tea.o      \
                 build/rtc.o      \
                 build/server.o     \
                 build/MyCritical.o		\
                 build/config.o   \
                 build/net_rsu.o      \
                 build/ipscan.o		\
                 build/cJSON.o		\
		 		 build/json_reader.o      \
                 build/jsonPackage.o  \
                 build/json_value.o     \
                 build/json_writer.o

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)



build/main.o: main.cpp \
     main.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/main.o main.cpp

build/comserver.o: comserver.cpp \
     comserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/comserver.o comserver.cpp

build/err_com.o: err_com.cpp \
     err_com.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/err_com.o err_com.cpp

build/rs485Com3.o: rs485Com3.cpp \
     rs485Com3.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/rs485Com3.o rs485Com3.cpp

build/rs485Com4.o: rs485Com4.cpp \
     rs485Com4.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/rs485Com4.o rs485Com4.cpp

build/rs485server.o: rs485server.cpp \
     rs485server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/rs485server.o rs485server.cpp

build/net_spd.o: net_spd.cpp \
     net_spd.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/net_spd.o net_spd.cpp

build/comport.o: comport.cpp \
     comport.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/comport.o comport.cpp

build/HttpPost.o: HttpPost.cpp \
     HttpPost.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/HttpPost.o HttpPost.cpp

build/IpcamServer.o: IpcamServer.cpp \
     IpcamServer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/IpcamServer.o IpcamServer.cpp

build/HttpServer.o: HttpServer.cpp \
     HttpServer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/HttpServer.o HttpServer.cpp

build/snmp.o: snmp.cpp \
     snmp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/snmp.o snmp.cpp

build/snmptrap.o: snmptrap.cpp \
     snmp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/snmptrap.o snmptrap.cpp

build/mywalkapp.o: mywalkapp.cpp \
     snmp.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/mywalkapp.o mywalkapp.cpp

build/AtlasServer.o: AtlasServer.cpp \
     AtlasServer.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/AtlasServer.o AtlasServer.cpp

build/tea.o: tea.cpp \
    tea.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/tea.o tea.cpp

build/rtc.o: rtc.cpp \
    rtc.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/rtc.o rtc.cpp

build/config.o: config.cpp \
     config.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/config.o config.cpp

build/net_rsu.o: net_rsu.c \
     net_rsu.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/net_rsu.o net_rsu.c

build/ipscan.o: ipscan.c \
     ipscan.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/ipscan.o ipscan.c

build/cJSON.o: cJSON.c \
     cJSON.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/cJSON.o cJSON.c

build/MyCritical.o: MyCritical.cpp \
    MyCritical.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/MyCritical.o MyCritical.cpp

build/server.o: server.cpp \
     server.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) $(LIBS) -o build/server.o server.cpp

build/jsonPackage.o: src/jsonPackage.cpp
	$(CXX) -o build/jsonPackage.o -c src/jsonPackage.cpp $(INCPATH)

build/json_reader.o: src/json_reader.cpp
	$(CXX) -o build/json_reader.o -c src/json_reader.cpp $(INCPATH)

build/json_value.o: src/json_value.cpp
	$(CXX) -o build/json_value.o -c src/json_value.cpp $(INCPATH)

build/json_writer.o: src/json_writer.cpp
	$(CXX) -o build/json_writer.o -c src/json_writer.cpp $(INCPATH)


clean:
	@rm -vf $(TARGET) build/*.o *~





