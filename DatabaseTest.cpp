//
// Created by Tong on 3/24/22.
//

#include "DatabaseTest.h"

void DatabaseTest::testSaveAccount() {
    db.saveAccount(1, 10000);
    db.saveAccount(2, 1500);
}

void DatabaseTest::testHasAccount() {
    bool ans1 = db.hasAccount(1);
    assert(ans1);
    bool ans2 = db.hasAccount(3);
    assert(not ans2);
}

void DatabaseTest::testPosition() {
    assert(not db.hasPosition("SYM", 1));
    db.savePosition("SYM", 1);
    assert(db.hasPosition("SYM", 1));
    assert(not db.hasPosition("SYM", 2));

    assert(db.getAmount("SYM", 1) == 0);

    db.updateAmount("SYM", 1, 200);
    assert(db.getAmount("SYM", 1) == 200);
    db.updateAmount("SYM", 1, 18.8);
    assert(db.getAmount("SYM", 1) == 218.8);

    db.updatePosition("BTC", 2, 15);
    assert(db.getAmount("BTC", 2) == 15);
    db.updatePosition("BTC", 1, 22.2);
    assert(db.getAmount("BTC", 1) == 22.2);
    db.updatePosition("SYM", 1, 10);
    assert(db.getAmount("SYM", 1) == 228.8);
}

void DatabaseTest::testOrder() {
    assert(db.getBalance(1) == 10000);
    db.updateBalance(1, -1000);
    assert(db.getBalance(1) == 9000);

    db.saveOrder(2, "BTC", 2, -5, 110);  // sell
    assert(db.getAmount("BTC", 2) == 10); // deduct share
    assert(db.getBalance(2) == 1500);

    db.saveOrder(1, "SYM", 1, 18, 230);  // buy
    assert(db.getBalance(1) == 4860); // deduct balance
    assert(db.getAmount("SYM", 1) == 228.8);

}

void DatabaseTest::testException() {
    try {
        db.updatePosition("SYM", 5, 100);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(e.what() == ACCOUNT_NOT_EXIST_ERROR);
    }
}

int main(int argc, char *argv[]) {
    DatabaseTest test;
    test.testSaveAccount();
    test.testHasAccount();
    test.testPosition();
    test.testOrder();
    test.testException();
    return EXIT_SUCCESS;
}