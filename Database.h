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
#include <mutex>

class Database {
public:
    std::mutex mtx;
    Database();
    pqxx::connection * connect();
    void createAccount(pqxx::connection * conn, int accountId, double balance);
    void updatePosition(pqxx::connection * conn, std::string symbol, int accountId, double amount);
    void placeOrder(pqxx::connection * conn, int orderId, std::string symbol, int accountId, double amount, double limitPrice);
    pqxx::result cancelOrder(pqxx::connection * conn, int orderId, int accountId);
    pqxx::result getOrder(pqxx::connection * conn, int orderId, int accountId);
    friend class DatabaseTest;
private:
    bool hasAccount(pqxx::connection * conn, int accountId);
    void createTable(pqxx::connection * conn, const char * fileName);
    bool hasPosition(pqxx::connection * conn, std::string symbol, int accountId);
    void handleSellOrder(pqxx::connection * conn, int sellOrderId, std::string symbol, int sellerAccountId,
                         double sellAmount, double sellLimit);
    void handleBuyOrder(pqxx::connection * conn, int buyOrderId, std::string symbol, int buyerAccountId, double buyAmount, double buyLimit);

    // get query
    std::string getUpdateAmountQuery(pqxx::work * w, std::string symbol, int accountId, double amount);
    std::string getUpdateBalanceQuery(pqxx::work * w, int accountId, double amount);
    std::string getUpdateCancelOrderQuery(pqxx::work * w, int orderId, int accountId);
    std::string getOpenOrderQuery(pqxx::work * w, int orderId, int accountId);
    std::string getSaveOrderQuery(pqxx::work * w, int orderId, std::string symbol, double amount, double limitPrice,
                                  std::string status, double executePrice, int accountId);
    std::string getBuyOrderQuery(pqxx::work *w, double sellLimit, std::string symbol, int sellerAccountId);
    std::string getExecuteBuyOrderQuery(pqxx::work *w, int buyOrderId, std::string symbol, int buyerAccountId,
                                        double executeAmount, double remainAmount, double buyLimit,
                                        double executePrice);
    std::string getUpdatePositionQuery(pqxx::work *w, std::string symbol, int accountId, double amount);
    std::string getUpdateOpenOrderQuery(pqxx::work *w, int orderId, int accountId, double remainAmount);
    std::string getExecuteSellOrderQuery(pqxx::work *w, int sellOrderId, std::string symbol,
                                                   int sellerAccountId, double executeAmount,
                                                   double remainAmount, double executePrice);
    std::string getSellOrderQuery(pqxx::work *w, double buyLimit, std::string symbol, int buyerAccountId);
    std::string getLockOrderQuery(pqxx::work *w, int orderId, int accountId);

    // fort test
    void saveOrder(pqxx::connection * conn, int orderId, std::string symbol, double amount, double limitPrice,
                   std::string status, double executePrice, int accountId);
    double getAmount(pqxx::connection * conn, std::string symbol, int accountId); // amount in position
    double getBalance(pqxx::connection * conn, int accountId);

};


#endif
