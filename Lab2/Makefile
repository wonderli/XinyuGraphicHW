# sample makefile using Coin3D library

COIN_DIRECTORY = /usr/local/Coin3d
COIN_INCLUDE = -I$(COIN_DIRECTORY)/include
COIN_LIBRARIES = -L$(COIN_DIRECTORY)/lib

#CPP = g++  -R$(COIN_DIRECTORY)/lib
CPP = g++  
INCLUDES = $(COIN_INCLUDE) 

# Uncomment to turn on debugging:
CPPFLAGS = -g -DDEBUG

LIBRARIES =  $(COIN_LIBRARIES) -lCoin 

OBJECT_FILES = OSUInventor.o MyScene.o MyRayTracer.o Lab2.o
EXEC = rt

#OSUInventor.o:
#	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) OSUInventor.h OSUInventor.C
#MyScene.o:
#	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) MyScene.h MyScene.cpp
#MyRayTracer.o:
#	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) MyRayTracer.h MyRayTracer.cpp
#Lab2: $(OBJECT_FILES)
#	$(CPP) -o $(EXEC) $(CPPFLAGS) $(OBJECT_FILES) \
#        $(LDFLAGS) $(LIBRARIES)


.C.o:
	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) $<
.cpp.o:
	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) $<

rt: $(OBJECT_FILES)
	$(CPP) -o $(EXEC) $(CPPFLAGS) $(OBJECT_FILES) \
        $(LDFLAGS) $(LIBRARIES)


clean:
	rm *.o *~ *.ppm  $(EXEC)
