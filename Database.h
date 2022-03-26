//
// Created by Tong on 3/24/22.
//

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <pqxx/pqxx>
#include "Constant.h"
#include <string>
#include <fstream>
#include "iostream"

class Database {
private:
    pqxx::connection * conn;

public:
    Database();
    void saveAccount(int id, double balance);
    bool hasAccount(int id);
    void updatePosition(std::string symbol, int accountId, double amount);
    ~Database();
    friend class DatabaseTest;
private:
    void createTable(const char * fileName);
    void savePosition(std::string symbol, int accountId);
    double getAmount(std::string symbol, int accountId);
    void updateAmount(std::string symbol, int accountId, double amount);
    bool hasPosition(std::string symbol, int accountId);
};


#endif
