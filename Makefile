ALL=client server db_test
all: $(ALL)
client: Client.cpp Socket.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
	g++ -std=gnu++11 -g -o $@ $^
server: Server.cpp Socket.cpp Database.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
	g++ -std=gnu++11 -g -o $@ $^ -lpqxx -lpq
db_test: Database.cpp DatabaseTest.cpp
	g++ -std=gnu++11 -g -o $@ $^ -lpqxx -lpq

.PHONY: clean
clean:
	rm -f *~ all
