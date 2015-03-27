

CXX= g++
CXXFLAGS= -g -std=c++11 -I. -I$(HOME)/include
LDFLAGS=
LDLIBS= -lpthread 

all: m2 sche 


clean:
	rm -rf *.o m2 sche core 
