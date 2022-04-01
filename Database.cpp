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
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
}


void Database::updateAmount(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    pqxx::work w(*conn);
    try {
        std::stringstream ss;
        ss << "UPDATE position"
           << " SET amount = amount + " << amount
           << " WHERE account_id = " << accountId
           << " AND symbol = " << w.quote(symbol) << ";";
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what();
        w.abort();
    }
}

void Database::updateBalance(pqxx::connection * conn, int accountId, double amount) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE account"
       << " SET balance = balance +" << amount
       << " WHERE account_id = " << accountId
       << " AND balance + " << amount << ">= 0;";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what();
        w.abort();
    }
}

void Database::updatePosition(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    if (not hasAccount(conn, accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, amount, account_id) VALUES ("
    << w.quote(symbol) << "," << amount << "," << accountId << ")"
    << " ON CONFLICT (symbol, account_id) DO UPDATE"
    << " SET amount = position.amount + " << amount << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
    }
}

pqxx::result Database::getOrderByStatus(pqxx::connection * conn, int orderId, int accountId, std::string status) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId;
    if (not status.empty()) {
        ss << " AND status = " << n.quote(status);
    }
    ss << " ORDER BY status DESC;";
    return pqxx::result(n.exec(ss.str()));
}

pqxx::result Database::getOrder(pqxx::connection * conn, int orderId, int accountId) {
    return getOrderByStatus(conn, orderId, accountId, "");
}




