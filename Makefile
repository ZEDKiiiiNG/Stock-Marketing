ALL=client server
all: $(ALL)
client: Client.cpp Socket.cpp
	g++ -std=gnu++11 -g -o $@ $^
server: Server.cpp Socket.cpp
	g++ -std=gnu++11 -g -lpqxx -lpq -o $@ $^

.PHONY: clean
clean:
	rm -f *~ all
