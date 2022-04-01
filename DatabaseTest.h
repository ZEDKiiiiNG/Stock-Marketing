//
// Created by Tong on 3/24/22.
//

#ifndef _DATABASETEST_H_
#define _DATABASETEST_H_

#include <stdlib.h>
#include <iostream>
#include "Database.h"
#include <assert.h>
#include "Constant.h"
#include <thread>

class DatabaseTest {
private:
    Database db;
public:
    void testSaveAccount();
    void testHasAccount();
    void testPosition();
    void testOrder();
    void testException();
    void testCancel();
    void testHandleSell();
    void testHandleBuy();
    void testMix();
    void testUpdateAmountMulti();
    void testCreateAccountMulti();
    void testUpdatePositionMuti();
    void testUpdateBalanceMuti();
    void testCancelOrderMuti();
    void testOpenOrderMuti();
    void testHandleSellMuti();
private:
    void displayOrder(pqxx::result & r);
    void handleCreateAccount(pqxx::connection * conn, int accountId, double balance);
    void testBuyOrderMulti();

};


#endif
