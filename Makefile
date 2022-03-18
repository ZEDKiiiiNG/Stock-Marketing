client: Client.cpp
    g++ -std=gnu++11 -g -o $@ $^
server: Server.cpp
    g++ -std=gnu++11 -g -o $@ $^

.PHONY: clean
clean:
	rm -f *~ client server