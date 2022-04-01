//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() : conn(new pqxx::connection(DB_INFO)) {
    createTable("tables.sql");
}

pqxx::connection * Database::connect() {
    conn = new connection("dbname=ACC_BBALL user=postgres password=passw0rd");
    if (!conn->is_open()) {
        throw std::invalid_argument("Can't open database\n");
    }
    return conn;
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
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
    ifs.close();
}


void Database::updateAmount(pqxx::connection conn1, std::string symbol, int accountId, double amount) {
    pqxx::work w(*conn1);
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

Database::~Database() {
    conn->disconnect();
    // delete conn;
}

