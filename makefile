all: client server

client: client.cpp util.hpp
	g++ client.cpp -o $@ -std=c++11

server: server.cpp util.hpp
	g++ server.cpp -o $@ -std=c++11;


.PHONY: clean

clean: client server
	rm -rf $^
