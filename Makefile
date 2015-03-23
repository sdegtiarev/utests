

CXX= g++
CXXFLAGS= -g -std=c++11 -I$(HOME)/include
LDFLAGS=
LDLIBS= -lpthread 

all: m2 qa 

clean:
	rm -rf *.o m2 qa core 
