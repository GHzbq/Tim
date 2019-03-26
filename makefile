all: client server

client: client.cpp
	g++ client.cpp -o $^

server: server.cpp
	g++ server.cpp -o $^


.PHONY: clean

clean: client server
	rm -rf $^
