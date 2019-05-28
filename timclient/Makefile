cc=g++

client=chatclient

.PHONY: all

all: $(client)
	
$(client): ./chatclientsrc/chatclient.cc
	$(cc) -o $@ $^  -lpthread -std=c++11

.PHONY: clean

clean: $(client)
	rm -f $(client)
