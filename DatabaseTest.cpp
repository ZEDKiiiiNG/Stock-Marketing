//
// Created by Tong on 3/24/22.
//

#include "DatabaseTest.h"

void DatabaseTest::testSaveAccount(Database & db) {
    db.saveAccount(1, 10000);
    pqxx::nontransaction n(*db.conn);
    std::string query("SELECT * FROM account;");
    pqxx::result r(n.exec(query));
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        std::cout << c[0].as<int>() << " "
                  << c[1].as<int>() << "\n";
    }
}

int main(int argc, char *argv[]) {
    Database db;
    DatabaseTest test;
    test.testSaveAccount(db);
    return EXIT_SUCCESS;
}