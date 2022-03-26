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
    assert(not ans2);
}

void DatabaseTest::testSymbol() {
    db.saveSymbol("SYM", 1);

    pqxx::nontransaction n(*db.conn);
    /*
    std::stringstream ss;
    ss << "SELECT * FROM position WHERE account_id = " << 1 << "AND symbol = " << n.quote("SYM") <<";";
    pqxx::result r(n.exec(ss.str()));
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        std::cout << c[0].as<std::string>() << "\n"
        << c[1].as<double>() << "\n";
    }
     */



    double amount = db.getAmount("SYM", 1);
    assert(amount == 0);
    std::cout << amount << '\n';

}

int main(int argc, char *argv[]) {
    DatabaseTest test;
    test.testSaveAccount();
    test.testHasAccount();
    test.testSymbol();
    return EXIT_SUCCESS;
}