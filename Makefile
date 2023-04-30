CXX=g++
CXXFLAGS=-Wall -std=c++11
TARGETS=client server

default: all
all: $(TARGETS)

TAR=tar
TARFLAGS=-cvf
TARNAME=342768264_x.tgz
TARSRCS=$(SRC) Makefile Answers.pdf server.cpp client.cpp server client

client: client.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

server: server.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm $(TARGETS) *~ *core

tar:
	tar -czf 342768264_x.tgz Makefile Answers.pdf server.cpp client.cpp server client
