//
// Created by Tong on 3/24/22.
//

#include "DatabaseTest.h"

void DatabaseTest::testSaveAccount() {
    db.createAccount(testconn, 1, 10000);
    db.createAccount(testconn, 2, 1500);
}

void DatabaseTest::testHasAccount() {
    bool ans1 = db.hasAccount(testconn, 1);
    assert(ans1);
    bool ans2 = db.hasAccount(testconn, 3);
    assert(not ans2);
}

void DatabaseTest::testPosition() {
    /*
    assert(not db.hasPosition("SYM", 1));
    db.savePosition("SYM", 1);
    assert(db.hasPosition("SYM", 1));
    assert(not db.hasPosition("SYM", 2));

    assert(db.getAmount("SYM", 1) == 0);

    db.updateAmount("SYM", 1, 200);
    assert(db.getAmount("SYM", 1) == 200);
    db.updateAmount("SYM", 1, 18.8);
    assert(db.getAmount("SYM", 1) == 218.8);
     */

    db.updatePosition(testconn, "BTC", 2, 15);
    assert(db.getAmount(testconn, "BTC", 2) == 15);
    db.updatePosition(testconn, "BTC", 1, 22.2);
    assert(db.getAmount(testconn, "BTC", 1) == 22.2);
    db.updatePosition(testconn, "SYM", 1, 10);
    assert(db.getAmount(testconn, "SYM", 1) == 10);
    db.updatePosition(testconn, "SYM", 1, 200);
    assert(db.getAmount(testconn, "SYM", 1) == 210);
}

/*
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
 */

void DatabaseTest::testException() {
    std::cout << "test exception start\n";
    try {
        db.updatePosition(testconn, "SYM", 5, 100);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_NOT_EXIST_ERROR);
    }

    try {
        db.createAccount(testconn, 1, 200);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_EXIST_ERROR);
    }

    try {
        db.placeOrder(testconn, 1, "SYM", 1, 8, 1650); // buy
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == INSUFFICIENT_BALANCE_ERROR);
        assert(db.getBalance(testconn, 1) == 10000);
    }

    try {
        db.placeOrder(testconn, 5, "BTC", 1, -24, 110); // sell
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == INSUFFICIENT_SHARE_ERROR);
        assert(db.getAmount(testconn, "BTC", 1) == 22.2);
    }

    try {
        db.placeOrder(testconn, 1, "BTC", 3, -5, 110);
    } catch (std::invalid_argument & e) {
        std::cout << e.what() << '\n';
        assert(std::string(e.what()) == ACCOUNT_NOT_EXIST_ERROR);
    }
    std::cout << "test exception end\n";
}


void DatabaseTest::testCancel() {
    /*
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
     */

    db.createAccount(testconn, 10, 10000);
    db.createAccount(testconn, 11, 10000);
    db.updatePosition(testconn, "UME", 10, 15);
    db.placeOrder(testconn, 15, "UME", 10, -5, 100);
    db.placeOrder(testconn, 16, "UME", 11, 3, 110);
    db.cancelOrder(testconn, 15, 10);
    pqxx::result r = db.getOrder(testconn, 15, 10);
    displayOrder(r);

}


void DatabaseTest::testHandleSell() {
    /*
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
     */

    db.createAccount(testconn, 5, 10000);
    db.createAccount(testconn, 6, 10000);
    db.createAccount(testconn, 7, 10000);
    db.updatePosition(testconn, "HW", 6, 15);
    db.placeOrder(testconn, 7, "HW", 5, 5, 112);  // buy
    db.placeOrder(testconn, 8, "HW", 5, 3, 114);
    db.placeOrder(testconn, 9, "HW", 7, 2, 116);
    db.placeOrder(testconn, 10, "HW", 6, -8, 110);

    assert(db.getBalance(testconn, 7) == 10000 - 116 * 2);
    assert(db.getAmount(testconn, "HW", 7) == 2);
    assert(db.getBalance(testconn, 5) == 10000 - 114 * 3 - 112 * 5);
    assert(db.getAmount(testconn, "HW", 5) == 6);
    assert(db.getBalance(testconn, 6) == 10000 + 116 * 2 + 114 * 3 + 112 * 3);
    assert(db.getAmount(testconn, "HW", 6) == 15 - 8);

    pqxx::result r = db.getOrder(testconn, 7, 5);
    displayOrder(r);
    r = db.getOrder(testconn, 8, 5);
    displayOrder(r);
    r = db.getOrder(testconn, 9, 7);
    displayOrder(r);
    r = db.getOrder(testconn, 10, 6);
    displayOrder(r);
}

void DatabaseTest::testHandleBuy() {
    /*
    db.saveOrder(23, "TF", -5, 116, STATUS_OPEN, 0, 3);  // sell
    db.saveOrder(24, "TF", -2, 114, STATUS_OPEN, 0, 3);
    db.saveOrder(25, "TF", -3, 113, STATUS_OPEN, 0, 3);
    db.saveOrder(26, "TF", 8, 115, STATUS_OPEN, 0, 4);
    db.saveOrder(27, "TF", -3, 112, STATUS_OPEN, 0, 4);
    pqxx::result r = db.getSellOrder(115, "TF", 4);
    displayOrder(r);
     */

    db.createAccount(testconn, 8, 10000);
    db.createAccount(testconn, 9, 10000);
    db.updatePosition(testconn, "STAR", 8, 16);
    db.placeOrder(testconn, 11, "STAR", 8, -5, 116); // sell
    db.placeOrder(testconn, 12, "STAR", 8, -2, 114);
    db.placeOrder(testconn, 13, "STAR", 8, -3, 113);
    db.placeOrder(testconn, 14, "STAR", 9, 8, 115); // buy

    assert(db.getAmount(testconn, "STAR", 8) == 16 - 5 -  2 - 3);
    assert(db.getBalance(testconn, 8) == 10000 + 113 * 3 + 114 * 2);
    assert(db.getAmount(testconn, "STAR", 9) == 5);
    assert(db.getBalance(testconn, 9) == 10000 - 115 * 8 + (115 -113) * 3 + (115 - 114) * 2);

    pqxx::result r = db.getOrder(testconn, 11, 8);
    displayOrder(r);
    r = db.getOrder(testconn, 12, 8);
    displayOrder(r);
    r = db.getOrder(testconn, 13, 8);
    displayOrder(r);
    r = db.getOrder(testconn, 14, 9);
    displayOrder(r);
}


void DatabaseTest::testMix() {
    db.createAccount(testconn, 13, 10000);
    db.createAccount(testconn, 12, 10000);
    db.updatePosition(testconn, "CHO", 13, 20);
    db.placeOrder(testconn, 17, "CHO", 13, -5, 116); // sell
    db.placeOrder(testconn, 18, "CHO", 13, -2, 114);
    db.placeOrder(testconn, 19, "CHO", 13, -3, 113);
    db.placeOrder(testconn, 20, "CHO", 12, 4, 115); // buy
    db.placeOrder(testconn, 21, "CHO", 13, -6, 110);
    db.placeOrder(testconn, 22, "CHO", 12, 9, 118);

    pqxx::result r = db.getOrder(testconn, 17, 13);
    displayOrder(r);
    r = db.getOrder(testconn, 18, 13);
    displayOrder(r);
    r = db.getOrder(testconn, 19, 13);
    displayOrder(r);
    r = db.getOrder(testconn, 20, 12);
    displayOrder(r);
    r = db.getOrder(testconn, 21, 13);
    displayOrder(r);
    r = db.getOrder(testconn, 22, 12);
    displayOrder(r);
}

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

/*
void DatabaseTest::testUpdateBalanceMuti() {
    pqxx::connection * conn1 = db.connect();
    pqxx::connection * conn2 = db.connect();
    db.updateBalance(conn1, 32, -10005);
    // add sleep before commit, work
    std::thread t1(&Database::updateBalance, this->db, conn1, 32, 10);
    std::thread t2(&Database::updateBalance, this->db, conn2, 32, 5);
    t1.join();
    t2.join();
    conn1->disconnect();
    conn2->disconnect();
}
 */

void DatabaseTest::testCancelOrderMuti() {
    pqxx::connection * conn1 = db.connect();
    pqxx::connection * conn2 = db.connect();
    db.createAccount(conn1, 33, 10000);
    db.saveOrder(conn1, 41, "SYM1", 5, 110, STATUS_OPEN, 0, 33);
    pqxx::result r = db.getOrder(conn1, 41, 33);
    displayOrder(r);
    r = db.cancelOrder(conn1, 41, 33);
    displayOrder(r);

    // add sleep after get open order, work
    /*
    std::thread t1(&Database::cancelOrder, this->db, conn1, 41, 33);
    std::thread t2(&Database::cancelOrder, this->db, conn2, 41, 33);
    t1.join();
    t2.join();
    conn1->disconnect();
    conn2->disconnect();
    r = db.getOrder(conn1, 41, 33);
    displayOrder(r);
     */
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

/*
void DatabaseTest::testOpenOrderMuti() {
    pqxx::connection *conn1 = db.connect();
    pqxx::connection *conn2 = db.connect();
    db.createAccount(conn1, 34, 1000);
    std::thread t1(&Database::placeOrder, this->db, conn1, 42, "SYM2", 34, 3, 200);
    std::thread t2(&Database::placeOrder, this->db, conn2, 43, "SYM2", 34, 4, 200);
    t1.join();
    t2.join();
    conn1->disconnect();
    conn2->disconnect();
    pqxx::result r = db.getOrder(conn1, 42, 34);
    displayOrder(r);
    r = db.getOrder(conn1, 43, 34);
    displayOrder(r);
}
 */

void DatabaseTest::testHandleSellMuti() {
    pqxx::connection *conn1 = db.connect();
    pqxx::connection *conn2 = db.connect();
    db.createAccount(conn1, 35, 10000);
    db.createAccount(conn1, 36, 10000);
    db.updatePosition(conn1, "SYM2", 36, 20);
    /*
    db.saveOrder(conn1, 44, "SYM2", 5, 110, STATUS_OPEN, 0, 35);
    db.saveOrder(conn1, 45, "SYM2", 6, 112, STATUS_OPEN, 0, 35);
    db.saveOrder(conn1, 46, "SYM2", -8, 108, STATUS_OPEN, 0, 36);
    db.handleSellOrder(conn1, 46, "SYM2", 36, -5, 108);
     */

    db.saveOrder(conn1, 47, "SYM3", 5, 110, STATUS_OPEN, 0, 35);
    db.saveOrder(conn1, 48, "SYM3", 6, 112, STATUS_OPEN, 0, 35);
    db.saveOrder(conn1, 49, "SYM4", 5, 110, STATUS_OPEN, 0, 35);
    db.saveOrder(conn1, 50, "SYM4", 6, 112, STATUS_OPEN, 0, 35);

    // t2 will not blocked by t1 if no row overlap
    // t3 will be block by t1
    std::thread t1(&DatabaseTest::testBuyOrderMulti, this, "SYM3", 36, 108);
    std::thread t2(&DatabaseTest::testBuyOrderMulti, this, "SYM4", 36, 108);
    std::thread t3(&DatabaseTest::testBuyOrderMulti, this, "SYM3", 36, 111);
    t1.join();
    t2.join();
    t3.join();

    pqxx::work w(*conn1);
    std::cout << db.getExecuteBuyOrderQuery(&w, 44, "SYM2", 35, 5, 0, 110, 110) << "\n\n";
    std::cout << db.getExecuteSellOrderQuery(&w, 44, "SYM2", 46, 5, 3, 110) << "\n";

    conn1->disconnect();
    conn2->disconnect();


}

void DatabaseTest::testBuyOrderMulti(std::string symbol, int accountId, double sellLimitPrice) {
    pqxx::connection *conn1 = db.connect();
    pqxx::work w(*conn1);
    std::stringstream ss;

    ss << db.getBuyOrderQuery(&w, sellLimitPrice, symbol, accountId);
    std::cout << ss.str() << '\n';
    pqxx::result r = w.exec(ss.str());
    if (symbol == "SYM3" && sellLimitPrice == 108) {
        std::cout << "delay 3s\n";
        w.exec("SELECT pg_sleep(3);");
    }
    w.commit();
    /*
    pqxx::result r = db.getBuyOrder(conn1, 108, "SYM3", 36);
    std::cout << "size " << symbol << ": " << r.size() << '\n';
     */
    displayOrder(r);
}

int main(int argc, char *argv[]) {
    DatabaseTest test;

    test.testSaveAccount();
    test.testHasAccount();
    test.testPosition();

    // test.testOrder();
    test.testException();

    test.testCancel();

    test.testHandleSell();

    test.testHandleBuy();
    test.testMix();

    // test.testUpdateAmountMulti();
    test.testCreateAccountMulti();
    test.testUpdatePositionMuti();
    // test.testUpdateBalanceMuti();
    test.testCancelOrderMuti();
    //test.testOpenOrderMuti();
    test.testHandleSellMuti();
    return EXIT_SUCCESS;
}