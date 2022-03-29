//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() : conn(new pqxx::connection(DB_INFO)) {
    createTable("tables.sql");
}

void Database::createTable(const char *fileName) {
    std::ifstream ifs;
    ifs.open(fileName, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::invalid_argument("ERROR: open sql file\n");
    }
    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
        ss << line;
    }
    pqxx::work w(*conn);
    w.exec(ss.str());
    w.commit();
    ifs.close();
}

void Database::saveAccount(int accountId, double balance) {
    if (hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    w.exec(ss.str());
    w.commit();
}

bool Database::hasAccount(int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM account"
        << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}

void Database::savePosition(std::string symbol, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, account_id) VALUES (" << w.quote(symbol) << "," << accountId << ");";
    w.exec(ss.str());
    w.commit();
}

double Database::getAmount(std::string symbol, int accountId) {
    if (not hasPosition(symbol, accountId)) {
        return 0;
    }
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT amount FROM position"
        << " WHERE account_id = " << accountId << "AND symbol = " << n.quote(symbol) << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.begin()[0].as<double>();
}

void Database::updateAmount(std::string symbol, int accountId, double amount) {
    double curr = getAmount(symbol, accountId);
    if (curr + amount < 0) {
        throw std::invalid_argument(INSUFFICIENT_SHARE_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE position"
        << " SET amount = " << curr + amount
        << " WHERE account_id = " << accountId << "AND symbol = " << w.quote(symbol) << ";";
    w.exec(ss.str());
    w.commit();
}

bool Database::hasPosition(std::string symbol, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM position"
       << " WHERE account_id = " << accountId << "AND symbol = " << n.quote(symbol) << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}

void Database::updatePosition(std::string symbol, int accountId, double amount) {
    if (not hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    if (not hasPosition(symbol, accountId)) {
        savePosition(symbol, accountId);
    }
    updateAmount(symbol, accountId, amount);
}

void Database::placeOrder(int orderId, std::string symbol, int accountId, double amount, double limitPrice) {
    if (not hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    if (amount < 0) {
        updateAmount(symbol, accountId, amount);  // negative amount, sell order, deduct shares
    }
    else {
        updateBalance(accountId, -limitPrice * amount); // buy order, deduct total cost
    }
    saveOrder(orderId, symbol, amount, limitPrice, STATUS_OPEN, 0, accountId);
}

void Database::updateBalance(int accountId, double amount) {
    double curr = getBalance(accountId);
    if (curr + amount < 0) {
        throw std::invalid_argument(INSUFFICIENT_BALANCE_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE account"
       << " SET balance = " << curr + amount
       << " WHERE account_id = " << accountId  << ";";
    w.exec(ss.str());
    w.commit();
}

double Database::getBalance(int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT balance FROM account"
       << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.begin()[0].as<double>();
}

pqxx::result Database::cancelOrder(int orderId, int accountId) {
    pqxx::result r = getOrder(orderId, accountId, STATUS_OPEN);
    if (r.size() == 0) {
        throw std::invalid_argument(NO_OPEN_ORDER_ERROR);
    }

    // refund
    std::string symbol = r.begin()[1].as<std::string>();
    double amount = r.begin()[2].as<double>();
    double limitPrice = r.begin()[3].as<double>();
    if (amount < 0) {
        updateAmount(symbol, accountId, -amount);  // negative amount, sell order, refund shares
    }
    else {
        updateBalance(accountId, limitPrice * amount); // buy order, refund price
    }

    updateCancelOrder(orderId, accountId);
    return getOrder(orderId, accountId);
}

pqxx::result Database::getOrder(int orderId, int accountId, std::string status) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId;
    if (not status.empty()) {
        ss << " AND status = " << n.quote(status);
    }
    ss << ";";
    return pqxx::result(n.exec(ss.str()));
}

pqxx::result Database::getOrder(int orderId, int accountId) {
    return getOrder(orderId, accountId, "");
}

void Database::updateCancelOrder(int orderId, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE trade_order"
       << " SET status = " << w.quote(STATUS_CANCELLED)
       << ", update_time = " << time(NULL)
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId << ";";
    w.exec(ss.str());
    w.commit();
}

pqxx::result Database::getBuyOrder(double sellLimit, std::string symbol) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE symbol = " << n.quote(symbol) << " AND limit_price >= " << sellLimit
       << " ORDER BY limit_price DESC, update_time ASC";
    return pqxx::result(n.exec(ss.str()));
}

/*
void Database::executeBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double amountPurchased,
                               double diffPrice) {
    updatePosition(symbol, buyerAccountId, amountPurchased);
    updateBalance(buyerAccountId, amountPurchased * diffPrice);

}
 */

void Database::saveOrder(int orderId, std::string symbol, double amount, double limitPrice, std::string status,
                         double executePrice, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO trade_order (order_id, symbol, amount, limit_price, status, update_time, execute_price, account_id) VALUES ("
       << orderId << "," << w.quote(symbol) << "," << amount << "," << limitPrice << ","
       << w.quote(status) << "," << time(NULL) << "," << executePrice << "," << accountId << ");";
    w.exec(ss.str());
    w.commit();
}

Database::~Database() {
    delete conn;
}

