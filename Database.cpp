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

void Database::saveAccount(int id, double balance) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << id << "," << balance << ");";
    w.exec(ss.str());
    w.commit();
}

bool Database::hasAccount(int id) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM account"
        << " WHERE account_id = " << id << ";";
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
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT amount FROM position"
        << " WHERE account_id = " << accountId << "AND symbol = " << n.quote(symbol) << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.begin()[0].as<double>();
}

void Database::updateAmount(std::string symbol, int accountId, double amount) {
    double curr = getAmount(symbol, accountId);
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

void Database::saveOrder(int orderId, std::string symbol, int accountId, double amount, double limit) {
    if (amount < 0) {
        updateAmount(symbol, accountId, amount);  // negative amount, sell order, deduct shares
    }
    else {
        updateBalance(accountId, -limit * amount); // buy order, deduct total cost
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO trade_order (order_id, symbol, amount, limit_price, update_time, account_id) VALUES ("
    << orderId << "," << w.quote(symbol) << "," << amount << ","
    << limit << "," << time(NULL) << "," << accountId << ");";
    w.exec(ss.str());
    w.commit();
}

void Database::updateBalance(int accountId, double amount) {
    double curr = getBalance(accountId);
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

Database::~Database() {
    delete conn;
}

