TAR   ?= tar
MAKE  ?= make
CMAKE ?= cmake
GIT   ?= git
WGET  ?= wget
CURL  ?= curl

LIBRARY_INSTALL_DIR ?= $(HOME)/.libs

REST_CLIENT_REPO := https://github.com/mrtazz/restclient-cpp.git
REDOX_REPO       := https://github.com/hmartiro/redox.git
JSON_SOURCE      := https://github.com/nlohmann/json/releases/download/v2.0.4/json.hpp

AWS_SDK_VERSION  := 1.0.5
AWS_SDK_DIR      := aws-sdk-cpp-$(AWS_SDK_VERSION)
AWS_SDK_TARBALL  := $(AWS_SDK_VERSION).tar.gz
AWS_SDK_URL      := https://github.com/aws/aws-sdk-cpp/archive/$(AWS_SDK_TARBALL)

DLIB_VERSION     := 19.1
DLIB_DIR         := dlib-$(DLIB_VERSION)
DLIB_TARBALL     := dlib-$(DLIB_VERSION).tar.bz2
DLIB_URL         := http://dlib.net/files/$(DLIB_TARBALL)

EASY_LOGGING_TARBALL_URL := https://github.com/easylogging/easyloggingpp/releases/download/9.84/easyloggingpp_v9.84.tar.gz

CXX=g++
CPPFLAGS=-g -I$(HOME)/.libs/include -std=c++11
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

install_dependencies: install_redox install_restclient install_aws_sdk install_dlib install_easy_logging

install_redox:
	$(eval TEMPDIR := $(shell mktemp -d))
	@$(GIT) clone $(REDOX_REPO) $(TEMPDIR)
	@mkdir -p $(TEMPDIR)/build
	@cd $(TEMPDIR)/build && $(CMAKE) .. -DCMAKE_INSTALL_PREFIX=$(LIBRARY_INSTALL_DIR)
	@$(MAKE) -C $(TEMPDIR)/build
	@$(MAKE) -C $(TEMPDIR)/build install
	@rm -rf $TEMPDIR

install_aws_sdk:
	$(eval TEMPDIR := $(shell mktemp -d))
	@$(WGET) -O $(TEMPDIR)/$(AWS_SDK_TARBALL) $(AWS_SDK_URL)
	@cd $(TEMPDIR) && $(TAR) xvzf $(AWS_SDK_TARBALL)
	@mkdir -p $(TEMPDIR)/$(AWS_SDK_DIR)/build
	@cd $(TEMPDIR)/$(AWS_SDK_DIR)/build && $(CMAKE) .. -DCMAKE_INSTALL_PREFIX=$(LIBRARY_INSTALL_DIR) -DBUILD_ONLY="s3"
	@$(MAKE) -C $(TEMPDIR)/$(AWS_SDK_DIR)/build
	@$(MAKE) -C $(TEMPDIR)/$(AWS_SDK_DIR)/build install
	@rm -rf $TEMPDIR

install_restclient:
	$(eval TEMPDIR := $(shell mktemp -d))
	@$(GIT) clone $(REST_CLIENT_REPO) $(TEMPDIR)
	@cd $(TEMPDIR) && ./autogen.sh && ./configure --prefix=$(LIBRARY_INSTALL_DIR)
	@$(MAKE) -C $(TEMPDIR)
	@$(MAKE) -C $(TEMPDIR) install
	@rm -rf $TEMPDIR

install_dlib:
	$(eval TEMPDIR := $(shell mktemp -d))
	@$(WGET) -O $(TEMPDIR)/$(DLIB_TARBALL) $(DLIB_URL)
	@cd $(TEMPDIR) && $(TAR) xvjf $(DLIB_TARBALL)
	@mkdir -p $(TEMPDIR)/$(DLIB_DIR)/build
	@cd $(TEMPDIR)/$(DLIB_DIR)/build && $(CMAKE) .. -DCMAKE_INSTALL_PREFIX=$(LIBRARY_INSTALL_DIR)
	@$(MAKE) -C $(TEMPDIR)/$(DLIB_DIR)/build
	@$(MAKE) -C $(TEMPDIR)/$(DLIB_DIR)/build install
	@rm -rf $TEMPDIR

install_json:
	@$(WGET) -q -O $(LIBRARY_INSTALL_DIR)/include/json.hpp $(JSON_SOURCE)

install_easy_logging:
	@$(CURL) -L -s $(EASY_LOGGING_TARBALL_URL) | tar xz -C $(LIBRARY_INSTALL_DIR)/include
