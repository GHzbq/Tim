cc=g++

server=chatserver

.PHONY: all

all: $(server)
	
$(server): ./chatserversrc/chatserver.cc
	$(cc) -o $@ $^  -lpthread -std=c++11 -ljsoncpp

.PHONY: clean

clean: $(server)
	rm -f $(server)
