//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() : *conn(new pqxx::connection(DB_INFO)) {
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

void Database::saveAccount(int id, int balance) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << id << "," << balance << ");";
    w.exec(ss.str());
    w.commit();
}

