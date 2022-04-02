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
#include <time.h>

class Client {
public:
    void start(int numOfLoop, int mode);
    void multiThreadStart(int numOfThread,int numOfLoop, int mode );
    double calc_time(struct timespec start, struct timespec end);
    void scalabilityStart(size_t MAX_TEST_NUM);

};


#endif
