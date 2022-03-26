//
// Created by Tong on 3/24/22.
//

#include "DatabaseTest.h"

void DatabaseTest::testSaveAccount() {
    db.saveAccount(1, 10000);
    db.saveAccount(2, 1500);
    pqxx::nontransaction n(*db.conn);
    std::string query("SELECT * FROM account;");
    pqxx::result r(n.exec(query));
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        std::cout << c[0].as<int>() << " "
                  << c[1].as<int>() << "\n";
    }
}

void DatabaseTest::testHasAccount() {
    bool ans1 = db.hasAccount(1);
    assert(ans1);
    bool ans2 = db.hasAccount(3);
    assert(ans2);
}

int main(int argc, char *argv[]) {
    DatabaseTest test;
    test.testSaveAccount();
    return EXIT_SUCCESS;
}