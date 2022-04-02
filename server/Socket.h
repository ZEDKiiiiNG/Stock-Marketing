//
// Created by Tong on 3/18/22.
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <iostream>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include <sstream>
#include "Constant.h"

class Socket {
private:
    struct addrinfo * getAddrInfo(const char * hostname, const char * port, bool isServer);
    int makeSocket(struct addrinfo * servinfo);
    void bindPort(int sockfd, struct addrinfo * servinfo);
    void listenPort(int sockfd);
    void connToServer(int sockfd, struct addrinfo * servinfo);
    int recvSmallData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr);
    int recvLargeData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr, int len);
    void sendMesg(int sockfd, const char * ptr, int msgLen);
    int getRequestLen(std::vector<char> & buffer);

public:
    int setupServer(const char * port);
    int acceptConn(int sockfd);
    int setupClient(std::string hostname, std::string port);
    std::vector<char> recvMesg(int sockfd);
    void sendMesg(int sockfd, std::vector<char> msg);
    void sendMesg(int sockfd, std::string msg);
    void closeConn(int sockfd);
};


#endif
