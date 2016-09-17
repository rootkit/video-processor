CXX=g++
CPPFLAGS=-g -I$(HOME)/.libs/include
LDFLAGS=-L$(HOME)/.libs/lib -Wl,-rpath=$(HOME)/.libs/lib
LDLIBS=-ldlib -lcblas -llapack $(shell pkg-config --libs opencv)

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
