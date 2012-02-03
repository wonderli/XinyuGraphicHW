# sample makefile using Coin3D library

COIN_DIRECTORY = /usr/local/Coin3d
COIN_INCLUDE = -I$(COIN_DIRECTORY)/include
COIN_LIBRARIES = -L$(COIN_DIRECTORY)/lib

CPP = g++  -R$(COIN_DIRECTORY)/lib
INCLUDES = $(COIN_INCLUDE) 

# Uncomment to turn on debugging:
CPPFLAGS = -g -DDEBUG

LIBRARIES =  $(COIN_LIBRARIES) -lCoin 

OBJECT_FILES = OSUInventor.o Lab2.o
EXEC = rt

.C.o:
	$(CPP) -c  $(CPPFLAGS) $(INCLUDES) $<

lab1: $(OBJECT_FILES)
	$(CPP) -o $(EXEC) $(CPPFLAGS) $(OBJECT_FILES) \
        $(LDFLAGS) $(LIBRARIES)

clean:
	rm *.o *~ *.ppm $(EXEC)
