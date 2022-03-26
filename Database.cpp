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
    ss << "SELECT * FROM account WHERE account_id = " << id << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}

void Database::saveSymbol(std::string symbol, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, account_id) VALUES (" << w.quote(symbol) << ","  << accountId << ");";
    w.exec(ss.str());
    w.commit();
}

double Database::getAmount(std::string symbol, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT amount FROM position WHERE account_id = " << accountId << "AND symbol = " << symbol <<";";
    pqxx::result r(n.exec(ss.str()));
    return r[0][1].as<double>();
}

Database::~Database() {
    delete conn;
}

