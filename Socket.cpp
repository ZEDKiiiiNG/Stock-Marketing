//
// Created by Tong on 3/18/22.
//

#include "Socket.h"

struct addrinfo * Socket::getAddrInfo(const char * hostname, const char * port, bool isServer) {
    struct addrinfo hints;
    struct addrinfo * servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (isServer) {
        hints.ai_flags = AI_PASSIVE;
    }
    int status = getaddrinfo(hostname, port, &hints, &servinfo);
    if (status != 0) {
        throw std::invalid_argument("no-id: ERROR getaddrinfo\n");
    }
    return servinfo;
}

int Socket::makeSocket(struct addrinfo *servinfo) {
    int sockfd = socket(servinfo->ai_family,
                        servinfo->ai_socktype,
                        servinfo->ai_protocol);

    if (sockfd == -1) {
        throw std::invalid_argument("no-id: ERROR socket\n");
    }
    return sockfd;
}

void Socket::bindPort(int sockfd, struct addrinfo *servinfo) {
    // allow the program to reuse the port
    int yes = 1;
    int status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1) {
        throw std::invalid_argument("no-id: ERROR setsockopt\n");
    }
    // bind
    status = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (status == -1) {
        throw std::invalid_argument("no-id: ERROR bind\n");
    }
}

void Socket::listenPort(int sockfd) {
    int status = listen(sockfd, BAGLOG);
    if (status == -1) {
        throw std::invalid_argument("no-id: ERROR listen\n");
    }
}

int Socket::setupServer(const char *port) {
    struct addrinfo * servinfo = getAddrInfo(NULL, port, true);
    int sockfd = makeSocket(servinfo);
    bindPort(sockfd, servinfo);
    listenPort(sockfd);
    freeaddrinfo(servinfo);
    return sockfd;
}

int Socket::acceptConn(int sockfd) {
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int new_fd = accept(sockfd, (struct sockaddr *)&socket_addr, &socket_addr_len);
    if (new_fd == -1) {
        throw std::invalid_argument("no-id: ERROR accept\n");
    }
    return new_fd;
}

void Socket::connToServer(int sockfd, struct addrinfo *servinfo) {
    int status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (status == -1) {
        throw std::invalid_argument("no-id: ERROR connect\n");
    }
}

int Socket::setupClient(std::string hostname, std::string port) {
    struct addrinfo * servinfo = getAddrInfo(hostname.c_str(), port.c_str(), false);
    int sockfd = makeSocket(servinfo);
    connToServer(sockfd, servinfo);
    freeaddrinfo(servinfo);
    return sockfd;
}

int Socket::recvSmallData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr) {
    int nBytes = recv(sockfd, ptr, bufferLen, 0);
    if (nBytes == -1) {
        throw std::invalid_argument("no-id: ERROR receive small data\n");
    }
    return nBytes;
}

int Socket::getRequestLen(std::vector<char> &buffer) {
    std::stringstream ss(buffer.data());
    std::string line;
    std::getline(ss, line);
    return std::stoi(line);
}

std::vector<char> Socket::recvMesg(int sockfd) {
    std::vector<char> buffer(LARGE_BUFFER_LEN, '\0');
    char * ptr = buffer.data();
    int nBytes = recvSmallData(sockfd, buffer, LARGE_BUFFER_LEN, buffer.data());
    int len = getRequestLen(buffer);
    ptr += nBytes;
    recvLargeData(sockfd, buffer, LARGE_BUFFER_LEN - nBytes, ptr, len - nBytes);
    buffer.resize(len);
    return buffer;
}

int Socket::recvLargeData(int sockfd, std::vector<char> & buffer, int bufferLen, char * ptr, int len) {
    int total = 0;
    int nBytes;
    while (total < len) {
        nBytes = recv(sockfd, ptr, bufferLen - total, 0);
        if (nBytes == -1) {
            throw std::invalid_argument("no-id: ERROR receive large data\n");
        }
        total += nBytes;
        ptr += nBytes;
    }
    return total;
}


void Socket::sendMesg(int sockfd, const char * ptr, int msgLen) {
    int sentLen = 0;
    int nBytes;
    while (sentLen < msgLen) {
        nBytes = send(sockfd, ptr, msgLen - sentLen, 0);
        if (nBytes == - 1) {
            throw std::invalid_argument("no-id: ERROR send message\n");
        }
        ptr += nBytes;
        sentLen += nBytes;
    }
}

void Socket::sendMesg(int sockfd, std::vector<char> msg) {
    sendMesg(sockfd, msg.data(), msg.size());
}

void Socket::sendMesg(int sockfd, std::string msg) {
    sendMesg(sockfd, msg.c_str(), msg.length());
}

void Socket::closeConn(int sockfd) {
    close(sockfd);
}
