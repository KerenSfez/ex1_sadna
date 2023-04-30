CXX=g++
CXXFLAGS=-Wall -std=c++11
TARGETS=client server

default: all
all: $(TARGETS)

client: client.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

server: server.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm $(TARGETS) *~ *core

