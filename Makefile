

CXX= g++
CXXFLAGS= -g -std=c++11 -I$(HOME)/include
LDFLAGS=
LDLIBS= -lpthread 

all: m2 sche 
	-@ssu sche

clean:
	rm -rf *.o m2 sche core 
