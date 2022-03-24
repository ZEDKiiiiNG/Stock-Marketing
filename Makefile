ALL=client server
all: $(ALL)
client: Client.cpp Socket.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
	g++ -std=gnu++11 -g -o $@ $^
server: Server.cpp Socket.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
	g++ -std=gnu++11 -g -o $@ $^

.PHONY: clean
clean:
	rm -f *~ all
