//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() {
    pqxx::connection * conn = connect();
    createTable(conn, "tables.sql");
    conn->disconnect();
}

pqxx::connection * Database::connect() {
    pqxx::connection * conn = new pqxx::connection(DB_INFO);
    if (!conn->is_open()) {
        throw std::invalid_argument("Can't open database\n");
    }
    return conn;
}

void Database::createTable(pqxx::connection * conn, const char *fileName) {
    std::ifstream ifs;
    ifs.open(fileName, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::invalid_argument("Can't open sql file\n");
    }
    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) {
        ss << line;
    }
    pqxx::work w(*conn);
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
    ifs.close();
}

bool Database::hasAccount(pqxx::connection * conn, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM account"
       << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}


void Database::createAccount(pqxx::connection * conn, int accountId, double balance) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
}


void Database::updateAmount(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    pqxx::work w(*conn);
    try {
        std::stringstream ss;
        ss << "UPDATE position"
           << " SET amount = amount + " << amount
           << " WHERE account_id = " << accountId
           << " AND symbol = " << w.quote(symbol) << ";";
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what();
        w.abort();
        throw std::invalid_argument(INSUFFICIENT_SHARE_ERROR);
    }
}

void Database::updateBalance(pqxx::connection * conn, int accountId, double amount) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE account"
       << " SET balance = balance +" << amount
       << " WHERE account_id = " << accountId << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what();
        w.abort();
        // throw std::invalid_argument(INSUFFICIENT_BALANCE_ERROR);
    }
}

void Database::updatePosition(pqxx::connection * conn, std::string symbol, int accountId, double amount) {
    if (not hasAccount(conn, accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    pqxx::work w(*conn);
    try {
        std::string q = getUpdatePositionQuery(&w, symbol, accountId, amount);
        w.exec(q);
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
    }
}

pqxx::result Database::getOrder(pqxx::connection * conn, int orderId, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId
       << " ORDER BY status DESC;";
    return pqxx::result(n.exec(ss.str()));
}


pqxx::result Database::cancelOrder(pqxx::connection * conn, int orderId, int accountId) {
    pqxx::work w(*conn);
    std::string q = getOpenOrderQuery(&w, orderId, accountId);
    pqxx::result r = w.exec(q);
    if (r.size() == 0) {
        throw std::invalid_argument(NO_OPEN_ORDER_ERROR);
    }

    // atomic refund
    std::stringstream ss;
    std::string symbol = r.begin()[1].as<std::string>();
    double amount = r.begin()[2].as<double>();
    double limitPrice = r.begin()[3].as<double>();
    if (amount < 0) {
        // negative amount, sell order, refund shares
        ss << getUpdateAmountQuery(&w, symbol, accountId, -amount);
    }
    else {
        // buy order, refund price
        ss << "\n" << getUpdateBalanceQuery(&w, accountId, limitPrice * amount);
    }
    ss << "\n" << getUpdateCancelOrderQuery(&w, orderId, accountId);
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
    }
    return getOrder(conn, orderId, accountId);
}


void Database::placeOrder(pqxx::connection * conn, int orderId, std::string symbol, int accountId,
                          double amount, double limitPrice) {
    if (not hasAccount(conn, accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    // atomically adjust and open order
    pqxx::work w(*conn);
    std::stringstream ss;
    if (amount < 0) {
        ss << getUpdateAmountQuery(&w, symbol, accountId, amount); // negative amount, sell order, deduct shares
    }
    else {
        ss << getUpdateBalanceQuery(&w, accountId, -limitPrice * amount);
        // ss << "\nSELECT pg_sleep(3);";
    }
    ss << getSaveOrderQuery(&w, orderId, symbol, amount, limitPrice, STATUS_OPEN, 0, accountId);
    std::cout << ss.str() << '\n';
    try {
        //ss << "\nSELECT pg_sleep(3);";
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        std::cout << e.what() << '\n';
        w.abort();
    }
    /*
    if (amount < 0) {
        handleSellOrder(orderId, symbol, accountId, amount, limitPrice);
    } else {
        handleBuyOrder(orderId, symbol, accountId, amount, limitPrice);
    }
     */

}

void Database::handleSellOrder(pqxx::connection * conn, int sellOrderId, std::string symbol, int sellerAccountId,
                               double sellAmount, double sellLimit) {
    pqxx::work w(*conn);
    std::string q = getBuyOrderQuery(&w, sellLimit, symbol, sellerAccountId);
    pqxx::result r = w.exec(q);
    std::cout << getBuyOrderQuery(&w, sellLimit, symbol, sellerAccountId) << '\n';
    pqxx::result::const_iterator c = r.begin();
    /*
    while (sellAmount != 0 && c != r.end()) {
        int buyOrderId = c[0].as<int>();
        double buyAmount = c[2].as<double>();
        double executeAmount = std::min(-sellAmount, buyAmount);
        double buyLimit = c[3].as<double>();
        double executePrice = c[3].as<double>();
        int buyerAccountId = c[7].as<int>();
        executeBuyOrder(buyOrderId, symbol, buyerAccountId, executeAmount,
                        buyAmount - executeAmount, buyLimit, executePrice);
        executeSellOrder(sellOrderId, symbol, sellerAccountId, executeAmount,
                         sellAmount + executeAmount, executePrice);
        sellAmount += executeAmount;
        ++c;
    }
     */
    w.commit();

}

// get query
std::string Database::getUpdateBalanceQuery(pqxx::work * w, int accountId, double amount) {
    std::stringstream ss;
    ss << "UPDATE account"
       << " SET balance = balance + " << amount
       << " WHERE account_id = " << accountId << ";";
    return ss.str();
}

std::string Database::getUpdateAmountQuery(pqxx::work * w, std::string symbol, int accountId, double amount) {
    std::stringstream ss;
    ss << "UPDATE position"
       << " SET amount = amount + " << amount
       << " WHERE account_id = " << accountId
       << " AND symbol = " << w->quote(symbol) << ";";
    return  ss.str();
}

std::string Database::getUpdateCancelOrderQuery(pqxx::work * w, int orderId, int accountId) {
    std::stringstream ss;
    ss << "UPDATE trade_order"
       << " SET status = " << w->quote(STATUS_CANCELLED)
       << ", update_time = " << time(NULL)
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId
       << " AND status = " << w->quote(STATUS_OPEN) << ";";
    return ss.str();
}

std::string Database::getOpenOrderQuery(pqxx::work * w, int orderId, int accountId) {
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId
       << " AND status = " << w->quote(STATUS_OPEN)
       << " FOR UPDATE" << ";";
    return ss.str();
}

std::string Database::getSaveOrderQuery(pqxx::work *w, int orderId, std::string symbol, double amount, double limitPrice,
                                        std::string status, double executePrice, int accountId) {
    std::stringstream ss;
    ss << "INSERT INTO trade_order (order_id, symbol, amount, limit_price, status, update_time, execute_price, account_id) VALUES ("
       << orderId << "," << w->quote(symbol) << "," << amount << "," << limitPrice << ","
       << w->quote(status) << "," << time(NULL) << "," << executePrice << "," << accountId << ");";
    return ss.str();
}

std::string Database::getBuyOrderQuery(pqxx::work *w, double sellLimit, std::string symbol, int sellerAccountId) {
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE symbol = " << w->quote(symbol) << " AND amount > 0 AND limit_price >= " << sellLimit
       << " AND status = " << w->quote(STATUS_OPEN) << " AND account_id != " << sellerAccountId
       << " ORDER BY limit_price DESC, update_time ASC, order_id ASC"
       << " FOR UPDATE;";
    return ss.str();
}

/*
std::string Database::getExecuteBuyOrderQuery(pqxx::work *w, int buyOrderId, std::string symbol, int buyerAccountId, double executeAmount,
                               double remainAmount, double buyLimit, double executePrice) {
    std::stringstream ss;
    ss << getUpdatePositionQuery(w, symbol, buyerAccountId, executeAmount)
        << "\n" << getUpdateBalanceQuery(w, executeAmount * (buyLimit - executePrice));
    // refund if buyer's limit price is higher than execution price
    updateBalance(buyerAccountId, executeAmount * (buyLimit - executePrice));
    updateOpenOrder(buyOrderId, buyerAccountId, remainAmount);
    saveOrder(buyOrderId, symbol, executeAmount, 0, STATUS_EXECUTED, executePrice, buyerAccountId);
}
 */

std::string Database::getUpdatePositionQuery(pqxx::work *w, std::string symbol, int accountId, double amount) {
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, amount, account_id) VALUES ("
       << w->quote(symbol) << "," << amount << "," << accountId << ")"
       << " ON CONFLICT (symbol, account_id) DO UPDATE"
       << " SET amount = position.amount + " << amount << ";";
    return ss.str();
}



// for test
void Database::saveOrder(pqxx::connection * conn, int orderId, std::string symbol, double amount, double limitPrice,
                         std::string status, double executePrice, int accountId) {
    pqxx::work w(*conn);
    std::string q = getSaveOrderQuery(&w, orderId, symbol, amount, limitPrice, status, executePrice, accountId);
    w.exec(q);
    w.commit();
}

/*
pqxx::result Database::getBuyOrder(pqxx::connection * conn, double sellLimit, std::string symbol, int sellerAccountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE symbol = " << n.quote(symbol) << " AND amount > 0 AND limit_price >= " << sellLimit
       << " AND status = " << n.quote(STATUS_OPEN) << " AND account_id != " << sellerAccountId
       << " ORDER BY limit_price DESC, update_time ASC, order_id ASC";
    return pqxx::result(n.exec(ss.str()));
}
*/




