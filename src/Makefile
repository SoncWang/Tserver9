#CC = gcc
#CC  = arm-fsl-linux-gnueabi-gcc
#CXX = arm-fsl-linux-gnueabi-g++
CC  = gcc
CXX = g++

CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT $(DEFINES)

INCPATH      = -I../include/

#LINK          = arm-fsl-linux-gnueabi-g++
LINK          = g++

#LFLAGS        = -Wl,-O1 -Wl,-rpath,/opt/gcc-4.4.4-glibc-2.11.1-multilib-1.0/arm-fsl-linux-gnueabi/lib
LFLAGS        = -Wl,-O1 -Wl,-rpath

#LIBS          =  $(SUBLIBS)  -L./httpssl/lib -lssl -lcrypto -ldl   -lpthread 
LIBS          = $(SUBLIBS)

TARGET        = jsonOut

OBJECTS       =  build/json_reader.o      \
                 build/jsonPackage.o  \
                 build/json_value.o     \
                 build/json_writer.o
                 

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) ${OPTIMIZE} -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
#	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

build/jsonPackage.o: jsonPackage.cpp 
	$(CXX) -o build/jsonPackage.o -c jsonPackage.cpp $(INCPATH) -std=c++11

build/json_reader.o: json_reader.cpp 
	$(CXX) -o build/json_reader.o -c json_reader.cpp $(INCPATH) -std=c++11
	
build/json_value.o: json_value.cpp 
	$(CXX) -o build/json_value.o -c json_value.cpp $(INCPATH) -std=c++11
	
build/json_writer.o: json_writer.cpp 
	$(CXX) -o build/json_writer.o -c json_writer.cpp $(INCPATH) -std=c++11

clean:
	@rm -vf $(TARGET) build/*.o *~






