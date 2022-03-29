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

class DatabaseTest {
private:
    Database db;
public:
    void testSaveAccount();
    void testHasAccount();
    void testPosition();
    void testOrder();
    void testException();
    void testHandleSell();
private:
    void displayOrder(pqxx::result & r);

};


#endif
