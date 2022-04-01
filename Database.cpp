//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() {
    pqxx::connection * conn = connect();
    createTable(conn, "tables.sql");
    conn->disconnect();
}

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

bool Database::hasAccount(pqxx::connection * conn, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM account"
       << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}


void Database::createAccount(pqxx::connection * conn, int accountId, double balance) {
    if (hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
    sleep(3);
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << "error\n";
        w.abort();
    }
}


void Database::updateAmount(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    pqxx::work w(*conn);
    try {
        std::stringstream ss;
        ss << "UPDATE position"
           << " SET amount = amount + " << amount
           << " WHERE account_id = " << accountId << " AND symbol = " << w.quote(symbol) << ";";
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << "error\n";
        w.abort();
    }
}

void Database::savePosition(pqxx::connection * conn, std::string symbol, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, account_id) VALUES (" << w.quote(symbol) << "," << accountId << ");";
    w.exec(ss.str());
    w.commit();
}



