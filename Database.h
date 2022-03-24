//
// Created by Tong on 3/24/22.
//

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <pqxx/pqxx>
#include "Constant.h"
#include <string>
#include <fstream>

class Database {
private:
    pqxx::connection * conn;

public:
    Database();
    void saveAccount(int id, int balance);
    ~Database();
private:
    void createTable(const char * fileName);
};


#endif
