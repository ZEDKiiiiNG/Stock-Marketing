//
// Created by Tong on 3/24/22.
//

#include "DatabaseTest.h"

/*
void DatabaseTest::testSaveAccount() {
    db.createAccount(1, 10000);
    db.createAccount(2, 1500);
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
        db.createAccount(1, 200);
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
    pqxx::result r = db.getOrderByStatus(1, 1, STATUS_OPEN);
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

    db.createAccount(10, 10000);
    db.createAccount(11, 10000);
    db.updatePosition("UME", 10, 15);
    db.placeOrder(15, "UME", 10, -5, 100);
    db.placeOrder(16, "UME", 11, 3, 110);
    db.cancelOrder(15, 10);
    r = db.getOrder(15, 10);
    displayOrder(r);

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
    db.createAccount(3, 10000);
    db.createAccount(4, 10000);
    db.saveOrder(3, "TEA", 5, 112, STATUS_OPEN, 0, 3);  // buy
    db.saveOrder(4, "TEA", 2, 114, STATUS_OPEN, 0, 3);
    db.saveOrder(5, "TEA", 3, 113, STATUS_OPEN, 0, 3);
    db.saveOrder(6, "TEA", -8, 110, STATUS_OPEN, 0, 4);
    db.saveOrder(28, "TEA", 6, 110, STATUS_OPEN, 0, 4);
    pqxx::result r = db.getBuyOrder(110, "TEA", 4);
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

    db.createAccount(5, 10000);
    db.createAccount(6, 10000);
    db.createAccount(7, 10000);
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
    assert(db.getAmount("HW", 6) == 15 - 8);

    r = db.getOrder(7, 5);
    displayOrder(r);
    r = db.getOrder(8, 5);
    displayOrder(r);
    r = db.getOrder(9, 7);
    displayOrder(r);
    r = db.getOrder(10, 6);
    displayOrder(r);
}

void DatabaseTest::testHandleBuy() {
    db.saveOrder(23, "TF", -5, 116, STATUS_OPEN, 0, 3);  // sell
    db.saveOrder(24, "TF", -2, 114, STATUS_OPEN, 0, 3);
    db.saveOrder(25, "TF", -3, 113, STATUS_OPEN, 0, 3);
    db.saveOrder(26, "TF", 8, 115, STATUS_OPEN, 0, 4);
    db.saveOrder(27, "TF", -3, 112, STATUS_OPEN, 0, 4);
    pqxx::result r = db.getSellOrder(115, "TF", 4);
    displayOrder(r);

    db.createAccount(8, 10000);
    db.createAccount(9, 10000);
    db.updatePosition("STAR", 8, 16);
    db.placeOrder(11, "STAR", 8, -5, 116); // sell
    db.placeOrder(12, "STAR", 8, -2, 114);
    db.placeOrder(13, "STAR", 8, -3, 113);
    db.placeOrder(14, "STAR", 9, 8, 115); // buy

    assert(db.getAmount("STAR", 8) == 16 - 5 -  2 - 3);
    assert(db.getBalance(8) == 10000 + 113 * 3 + 114 * 2);
    assert(db.getAmount("STAR", 9) == 5);
    assert(db.getBalance(9) == 10000 - 115 * 8 + (115 -113) * 3 + (115 - 114) * 2);

    r = db.getOrder(11, 8);
    displayOrder(r);
    r = db.getOrder(12, 8);
    displayOrder(r);
    r = db.getOrder(13, 8);
    displayOrder(r);
    r = db.getOrder(14, 9);
    displayOrder(r);
}

void DatabaseTest::testMix() {
    db.createAccount(13, 10000);
    db.createAccount(12, 10000);
    db.updatePosition("CHO", 13, 20);
    db.placeOrder(17, "CHO", 13, -5, 116); // sell
    db.placeOrder(18, "CHO", 13, -2, 114);
    db.placeOrder(19, "CHO", 13, -3, 113);
    db.placeOrder(20, "CHO", 12, 4, 115); // buy
    db.placeOrder(21, "CHO", 13, -6, 110);
    db.placeOrder(22, "CHO", 12, 9, 118);

    pqxx::result r = db.getOrder(17, 13);
    displayOrder(r);
    r = db.getOrder(18, 13);
    displayOrder(r);
    r = db.getOrder(19, 13);
    displayOrder(r);
    r = db.getOrder(20, 12);
    displayOrder(r);
    r = db.getOrder(21, 13);
    displayOrder(r);
    r = db.getOrder(22, 13);
    displayOrder(r);
}
 */

/*
void DatabaseTest::testUpdateAmountMulti() {
    pqxx::connection * conn1 = db.connect();
    std::string symbol = "WE";
    int accountId = 14;
    double amount1 = 5;
    double amount2 = -2;
    db.createAccount(conn1, accountId, 10000);
    db.savePosition(conn1, symbol, accountId);

    pqxx::connection * conn2 = db.connect();
    std::thread t1(&Database::updateAmount, this->db, conn1, symbol, accountId, amount1);
    std::thread t2(&Database::updateAmount, this->db, conn2, symbol, accountId, amount2);
    t1.join();
    t2.join();
    conn1->disconnect();
    conn2->disconnect();
}
 */

void DatabaseTest::testCreateAccountMulti() {
    pqxx::connection * conn1 = db.connect();
    pqxx::connection * conn2 = db.connect();
    std::thread t1(&DatabaseTest::handleCreateAccount, this, conn1, 31, 1000);
    std::thread t2(&DatabaseTest::handleCreateAccount, this, conn2, 31, 2000);
    t1.join();
    t2.join();
    conn1->disconnect();
    conn2->disconnect();
}

void DatabaseTest::handleCreateAccount(pqxx::connection *conn, int accountId, double balance) {
    try {
        db.createAccount(conn, accountId, balance);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << " " << accountId << '\n';
    }
}

void DatabaseTest::testUpdatePositionMuti() {
    pqxx::connection * conn1 = db.connect();
    pqxx::connection * conn2 = db.connect();
    pqxx::connection * conn3 = db.connect();
    db.createAccount(conn1, 32, 10000);
    std::thread t1(&Database::updatePosition, this->db, conn1, "SYM1", 32, 2);
    std::thread t2(&Database::updatePosition, this->db, conn2, "SYM1", 32, 3);
    std::thread t3(&Database::updatePosition, this->db, conn3, "SYM1", 32, 5);
    t1.join();
    t2.join();
    t3.join();
    conn1->disconnect();
    conn2->disconnect();
    conn3->disconnect();
}

void DatabaseTest::testUpdateBalanceMuti() {
    pqxx::connection * conn1 = db.connect();
    db.updateBalance(conn1, 32, -10005);
}

int main(int argc, char *argv[]) {
    DatabaseTest test;
    /*
    test.testSaveAccount();
    test.testHasAccount();
    test.testPosition();
    test.testOrder();
    test.testException();
    test.testCancel();
    test.testHandleSell();
    test.testHandleBuy();
    test.testMix();
    */
    // test.testUpdateAmountMulti();
    test.testCreateAccountMulti();
    test.testUpdatePositionMuti();
    //test.testUpdateBalanceMuti();
    return EXIT_SUCCESS;
}