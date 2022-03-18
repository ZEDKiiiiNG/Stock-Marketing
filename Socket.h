//
// Created by Tong on 3/18/22.
//

#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "Request.h"
#include "Response.h"
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>

#define BAGLOG 10
#define LARGE_BUFFER_LEN 1000000
#define SMALL_BUFFER_LEN 8000
#define TERM_CHUNK_LEN 5

class Socket {
private:
    struct addrinfo * getAddrInfo(const char * hostname, const char * port, bool isServer);
    int makeSocket(struct addrinfo * servinfo);
    void bindPort(int sockfd, struct addrinfo * servinfo);
    void listenPort(int sockfd);
    void connToServer(int sockfd, struct addrinfo * servinfo);
    int recvSmallData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr);
    int recvLargeData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr, int len, bool chunked);
    void sendMesg(int sockfd, const char * ptr, int msgLen);
    bool chunkEnd(std::vector<char> & buffer);
    std::vector<char>::iterator findTermChunk(std::vector<char> & buffer);
public:
    int setupServer(const char * port);
    int acceptConn(int sockfd);
    int setupClient(std::string hostname, std::string port);
    Request recvRequest(int sockfd, int id);
    Response recvResponse(int sockfd);
    std::vector<char> recvMesg(int sockfd);
    void sendMesg(int sockfd, std::vector<char> msg);
    void sendMesg(int sockfd, std::string msg);
    void closeConn(int sockfd);
};


#endif
