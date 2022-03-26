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

class Server {
//std::string getXmlContent(const std::string& raw);
private:
    Database db;

    void handleCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement);
    void handleTransection(TiXmlElement* rootElement, TiXmlElement* rootResultElement);

    void handleAccountCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement);
    void handleSymbolCreate(TiXmlElement* rootElement, TiXmlElement* rootResultElement);
public:
    const char * getXmlContent(const char* raw);

    void handleRequest(TiXmlElement* rootElement, TiXmlElement* rootResultElement);
};


#endif
