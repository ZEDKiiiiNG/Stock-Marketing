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

class Server {
//std::string getXmlContent(const std::string& raw);
public:
    const char * getXmlContent(const char* raw);
};


#endif
