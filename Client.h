//
// Created by Tong on 3/18/22.
//

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdlib.h>
#include <iostream>
#include "Socket.h"
#include "tinyxml.h"
#include "tinystr.h"
#include <thread>

class Client {
public:
    void start();
    void multiThreadStart();
};


#endif
