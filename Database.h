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
#include <time.h>
#include <stdexcept>

class Database {
private:
    pqxx::connection * conn;

public:
    Database();
    void saveAccount(int id, double balance);
    bool hasAccount(int id);
    void updatePosition(std::string symbol, int accountId, double amount);
    void saveOrder(int orderId, std::string symbol, int accountId, double amount, double limit);
    void cancelOrder(int orderId, int accountId);
    ~Database();
    friend class DatabaseTest;
private:
    void createTable(const char * fileName);
    void savePosition(std::string symbol, int accountId);
    double getAmount(std::string symbol, int accountId); // amount in position
    void updateAmount(std::string symbol, int accountId, double amount);  // amount in position
    bool hasPosition(std::string symbol, int accountId);
    void updateBalance(int accountId, double amount);
    double getBalance(int accountId);
    pqxx::result getOrder(int orderId, int accountId);
};


#endif
