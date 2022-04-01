//
// Created by Tong on 3/24/22.
//

#include "Database.h"

pqxx::connection * Database::connect() {
    pqxx::connection * conn = new pqxx::connection(DB_INFO);
    if (!conn->is_open()) {
        throw std::invalid_argument("Can't open database\n");
    }
    return conn;
}

void Database::createTable(pqxx::connection * conn, const char *fileName) {
    std::ifstream ifs;
    ifs.open(fileName, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::invalid_argument("Can't open sql file\n");
    }
    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
        ss << line;
    }
    pqxx::work w(*conn);
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
    ifs.close();
}


void Database::updateAmount(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    pqxx::work w(*conn);
    try {
        std::stringstream ss;
        ss << "SELECT amount FROM position"
           << " WHERE account_id = " << accountId << "AND symbol = " << w.quote(symbol) << ";";
        pqxx::result r(w.exec(ss.str()));
        double curr = r.begin()[0].as<double>();

        std::stringstream ss1;
        ss1 << "UPDATE position"
           << " SET amount = " << curr + amount
           << " WHERE account_id = " << accountId << "AND symbol = " << w.quote(symbol) << ";";
        w.exec(ss1.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << "error\n";
        w.abort();
    }
}

void Database::saveAccount(pqxx::connection * conn, int accountId, double balance) {
    /*
    if (hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
     */
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    w.exec(ss.str());
    w.commit();
}


void Database::savePosition(pqxx::connection * conn, std::string symbol, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, account_id) VALUES (" << w.quote(symbol) << "," << accountId << ");";
    w.exec(ss.str());
    w.commit();
}



