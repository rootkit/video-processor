CXX=g++
CPPFLAGS=-g -I$(HOME)/.libs/include
LDFLAGS=-L$(HOME)/.libs/lib -L$(HOME)/.libs/lib64 -Wl,-rpath=$(HOME)/.libs/lib:$(HOME)/.libs/lib64
LDLIBS=-ldlib -lcblas -llapack -laws-cpp-sdk-core -laws-cpp-sdk-s3 -lhiredis -lev -lredox -lrestclient-cpp -lboost_system -lboost_filesystem $(shell pkg-config --libs opencv)

SRCS=$(wildcard src/*.cpp)
OBJS=$(subst .cpp,.o,$(SRCS))

all: face_swapper

face_swapper: $(OBJS)
	$(CXX) $(LDFLAGS) -o face_swapper $(OBJS) $(LDLIBS)

run: face_swapper
	./face_swapper

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f src/*.o
	rm -f face_swapper
