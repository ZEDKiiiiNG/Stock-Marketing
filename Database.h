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
#include <unistd.h>

class Database {
private:
    pqxx::connection * conn;

public:
    Database();
    pqxx::connection * connect();
    void saveAccount(int accountId, double balance);
    void updatePosition(std::string symbol, int accountId, double amount);
    void placeOrder(int orderId, std::string symbol, int accountId, double amount, double limitPrice);
    pqxx::result cancelOrder(int orderId, int accountId);
    pqxx::result getOrder(int orderId, int accountId);
    ~Database();
    friend class DatabaseTest;
private:
    bool hasAccount(int accountId);
    void createTable(const char * fileName);
    void savePosition(std::string symbol, int accountId);
    double getAmount(std::string symbol, int accountId); // amount in position
    void updateAmount(pqxx::connection * conn1, std::string symbol, int accountId, double amount);  // amount in position
    bool hasPosition(std::string symbol, int accountId);
    void updateBalance(int accountId, double amount);
    double getBalance(int accountId);
    pqxx::result getOrderByStatus(int orderId, int accountId, std::string status);
    void updateCancelOrder(int orderId, int accountId);
    void handleSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double sellAmount, double sellLimit);
    void handleBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double buyAmount, double buyLimit);
    pqxx::result getBuyOrder(double sellLimit, std::string symbol, int sellAccountId);
    void executeBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double executeAmount,
                         double remainAmount, double buyLimit, double executePrice);
    void saveOrder(int orderId, std::string symbol, double amount, double limitPrice, std::string status, double executePrice, int accountId);
    void updateOpenOrder(int orderId, int accountId, double amount);
    void executeSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double executeAmount,
                          double remainAmount, double executePrice);
    pqxx::result getSellOrder(double buyLimit, std::string symbol, int buyerAccountId);

};


#endif
