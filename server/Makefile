ALL=server db_test
all: $(ALL)
server: Server.cpp Socket.cpp Database.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp
	g++ -std=gnu++11 -g -o $@ $^ -lpqxx -lpq -lpthread
db_test: Database.cpp DatabaseTest.cpp
	g++ -std=gnu++11 -g -o $@ $^ -lpqxx -lpq -lpthread

.PHONY: clean
clean:
	rm -f *~ $(ALL)
