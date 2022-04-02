//
// Created by Tong on 3/18/22.
//

#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdlib.h>
#include <iostream>
#include <vector>
#include "Socket.h"
#include "tinyxml.h"
#include <cstring>
#include "DatabaseTest.h"
#include <sstream>
#include <iomanip>
#include <thread>
#include <pqxx/pqxx>
#include <mutex>

class Server {
//std::string getXmlContent(const std::string& raw);
private:
    Database db;
    int orderId;

    void handleRequest(TiXmlElement* rootElement, TiXmlElement* rootResultElement, pqxx::connection * conn);

    void handleCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement, pqxx::connection * conn);
    void handleTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, pqxx::connection * conn);

    void handleAccountCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement, pqxx::connection * conn);
    void handleSymbolCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement, pqxx::connection * conn);

    void handleOrderTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId, pqxx::connection * conn);
    void handleQueryTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId, pqxx::connection * conn);
    void handleCancelTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement, int accountId, pqxx::connection * conn);

public:
    Server(){
        orderId = 0;
    }
    const char * getXmlContent(const char* raw);
    void serveRequest(Socket socket);
    void serveRequestMulti(Socket socket,int listen_fd);
    void processRequest(Socket socket, std::vector<char>  request, int msg_fd);
    void runServer(Socket & socket);
    void runServerPreCreate(Socket & socket);
};


#endif
