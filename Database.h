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
    void saveAccount(int accountId, double balance);
    bool hasAccount(int accountId);
    void updatePosition(std::string symbol, int accountId, double amount);
    void placeOrder(int orderId, std::string symbol, int accountId, double amount, double limitPrice);
    pqxx::result cancelOrder(int orderId, int accountId);
    pqxx::result getOrder(int orderId, int accountId);
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
    pqxx::result getOrder(int orderId, int accountId, std::string status);
    void updateCancelOrder(int orderId, int accountId);
    void handleSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double sellAmount, double sellLimit);
    pqxx::result getBuyOrder(double sellLimit, std::string symbol);
    void executeBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double executeAmount,
                         double remainAmount, double buyLimit, double executePrice);
    void saveOrder(int orderId, std::string symbol, double amount, double limitPrice, std::string status, double executePrice, int accountId);
    void updateOpenOrder(int orderId, int accountId, double amount);
    void executeSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double executeAmount,
                          double remainAmount, double executePrice);
    pqxx::result getSellOrder(double buyLimit, std::string symbol);
};


#endif
