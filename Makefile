

CXX= g++
CXXFLAGS= -g -std=c++11 -I$(HOME)/include
LDFLAGS=
LDLIBS=

all: m2

clean:
	rm -rf *.o m2 core 
