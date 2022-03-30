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

    assert(db.getAmount("ABC", 1) == 0);
}

void DatabaseTest::testOrder() {
    assert(db.getBalance(1) == 10000);
    db.updateBalance(1, -1000);
    assert(db.getBalance(1) == 9000);

    db.placeOrder(2, "BTC", 2, -5, 110);  // sell
    assert(db.getAmount("BTC", 2) == 10); // deduct share
    assert(db.getBalance(2) == 1500);

    db.placeOrder(1, "SYM", 1, 18, 230);  // buy
    assert(db.getBalance(1) == 4860); // deduct balance
    assert(db.getAmount("SYM", 1) == 228.8);

}

void DatabaseTest::testException() {
    try {
        db.updatePosition("SYM", 5, 100);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_NOT_EXIST_ERROR);
    }

    try {
        db.saveAccount(1, 200);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_EXIST_ERROR);
    }

    try {
        db.placeOrder(1, "SYM", 1, 3, 1650); // buy
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == INSUFFICIENT_BALANCE_ERROR);
        assert(db.getBalance(1) == 4860);
    }

    try {
        db.placeOrder(1, "BTC", 2, -11, 110); // sell
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == INSUFFICIENT_SHARE_ERROR);
        assert(db.getAmount("BTC", 2) == 10);
    }

    try {
        db.placeOrder(1, "BTC", 3, -5, 110);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_NOT_EXIST_ERROR);
    }
}

void DatabaseTest::testCancel() {
    pqxx::result r = db.getOrder(1, 1, STATUS_OPEN);
    displayOrder(r);

    // db.updateCancelOrder(1, 1);
    db.cancelOrder(1, 1);  // buy, refund price
    assert(db.getBalance(1) == 9000);
    r = db.getOrder(1, 1);
    displayOrder(r);

    db.cancelOrder(2, 2); // refund share
    assert(db.getAmount("BTC", 2) == 15);
    r = db.getOrder(2, 2);
    displayOrder(r);

    try {
        db.cancelOrder(2, 2);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == NO_OPEN_ORDER_ERROR);
    }

}

void DatabaseTest::displayOrder(pqxx::result & r) {
    for (pqxx::result::const_iterator c = r.begin(); c != r.end(); ++c) {
        std::cout << c[0].as<int>() << " " // order_id
                  << c[1].as<std::string>() << " "  // symbol
                  << c[2].as<double>() << " "  // amount
                  << c[3].as<double>() << " "  // limit_price
                  << c[4].as<std::string>() << " "  // status
                  << c[5].as<int>() << " "  // update_time
                  << c[6].as<double>() << " "  // execute_price
                  << c[7].as<int>() << "\n";  // account_id
    }
    std::cout << '\n';
}

void DatabaseTest::testHandleSell() {
    db.saveAccount(3, 10000);
    db.saveAccount(4, 10000);
    db.saveOrder(3, "TEA", 5, 112, STATUS_OPEN, 0, 3);  // buy
    db.saveOrder(4, "TEA", 2, 114, STATUS_OPEN, 0, 3);
    db.saveOrder(5, "TEA", 3, 113, STATUS_OPEN, 0, 3);
    db.saveOrder(6, "TEA", -8, 110, STATUS_OPEN, 0, 4);
    pqxx::result r = db.getBuyOrder(110, "TEA");
    displayOrder(r);

    db.updateOpenOrder(3, 3, 4);
    r = db.getOrder(3, 3);
    displayOrder(r);

    // assume 6, "TEA", 4, -8, 110
    db.executeBuyOrder(4, "TEA", 3, 2, 0, 114, 110);
    assert(db.getAmount("TEA", 3) ==  2);
    assert(db.getBalance(3) == 10008);
    r = db.getOrder(4, 3);
    displayOrder(r);

    db.executeSellOrder(6, "TEA", 4, 2, -6, 110);
    assert(db.getBalance(4) == 10220);
    r = db.getOrder(6, 4);
    displayOrder(r);

    db.saveAccount(5, 10000);
    db.saveAccount(6, 10000);
    db.saveAccount(7, 10000);
    db.updatePosition("HW", 6, 15);
    db.placeOrder(7, "HW", 5, 5, 112);  // buy
    db.placeOrder(8, "HW", 5, 3, 114);
    db.placeOrder(9, "HW", 7, 2, 116);
    db.placeOrder(10, "HW", 6, -8, 110);

    assert(db.getBalance(7) == 10000 - 116 * 2);
    assert(db.getAmount("HW", 7) == 2);
    assert(db.getBalance(5) == 10000 - 114 * 3 - 112 * 5);
    assert(db.getAmount("HW", 5) == 6);
    assert(db.getBalance(6) == 10000 + 116 * 2 + 114 * 3 + 112 * 3);
    assert(db.getAmount("HW", 6) == 7);

    r = db.getOrder(7, 5);
    displayOrder(r);
    r = db.getOrder(8, 5);
    displayOrder(r);
    r = db.getOrder(9, 7);
    displayOrder(r);
    r = db.getOrder(10, 6);
    displayOrder(r);
}

int main(int argc, char *argv[]) {
    DatabaseTest test;
    test.testSaveAccount();
    test.testHasAccount();
    test.testPosition();
    test.testOrder();
    test.testException();
    test.testCancel();
    test.testHandleSell();
    return EXIT_SUCCESS;
}