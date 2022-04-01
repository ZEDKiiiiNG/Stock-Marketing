//
// Created by Tong on 3/24/22.
//

#include "Database.h"

Database::Database() : conn(new pqxx::connection(DB_INFO)) {
    createTable("tables.sql");
}

void Database::createTable(const char *fileName) {
    std::ifstream ifs;
    ifs.open(fileName, std::ifstream::in);
    if (!ifs.is_open()) {
        throw std::invalid_argument("ERROR: open sql file\n");
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

void Database::saveAccount(int accountId, double balance) {
    if (hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_EXIST_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO account (account_id, balance) VALUES (" << accountId << "," << balance << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

bool Database::hasAccount(int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM account"
        << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}

void Database::savePosition(std::string symbol, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO position (symbol, account_id) VALUES (" << w.quote(symbol) << "," << accountId << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

double Database::getAmount(std::string symbol, int accountId) {
    /*
    if (not hasPosition(symbol, accountId)) {
        return 0;
    }
     */
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "SELECT amount FROM position"
        << " WHERE account_id = " << accountId << "AND symbol = " << w.quote(symbol) << ";";
    pqxx::result r(w.exec(ss.str()));
    w.commit();
    return r.begin()[0].as<double>();
}

void Database::updateAmount(std::string symbol, int accountId, double amount) {
    double curr = getAmount(symbol, accountId);
    if (curr + amount < 0) {
        throw std::invalid_argument(INSUFFICIENT_SHARE_ERROR);
    }
    sleep(2);
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE position"
        << " SET amount = " << curr + amount
        << " WHERE account_id = " << accountId << "AND symbol = " << w.quote(symbol) << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

bool Database::hasPosition(std::string symbol, int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM position"
       << " WHERE account_id = " << accountId << "AND symbol = " << n.quote(symbol) << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.size() > 0;
}

void Database::updatePosition(std::string symbol, int accountId, double amount) {
    if (not hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    if (not hasPosition(symbol, accountId)) {
        savePosition(symbol, accountId);
    }
    updateAmount(symbol, accountId, amount);
}

void Database::placeOrder(int orderId, std::string symbol, int accountId, double amount, double limitPrice) {
    if (not hasAccount(accountId)) {
        throw std::invalid_argument(ACCOUNT_NOT_EXIST_ERROR);
    }
    if (amount < 0) {
        updateAmount(symbol, accountId, amount);  // negative amount, sell order, deduct shares
    }
    else {
        updateBalance(accountId, -limitPrice * amount); // buy order, deduct total cost
    }
    saveOrder(orderId, symbol, amount, limitPrice, STATUS_OPEN, 0, accountId);
    if (amount < 0) {
        handleSellOrder(orderId, symbol, accountId, amount, limitPrice);
    } else {
        handleBuyOrder(orderId, symbol, accountId, amount, limitPrice);
    }
}

void Database::updateBalance(int accountId, double amount) {
    double curr = getBalance(accountId);
    if (curr + amount < 0) {
        throw std::invalid_argument(INSUFFICIENT_BALANCE_ERROR);
    }
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE account"
       << " SET balance = " << curr + amount
       << " WHERE account_id = " << accountId  << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

double Database::getBalance(int accountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT balance FROM account"
       << " WHERE account_id = " << accountId << ";";
    pqxx::result r(n.exec(ss.str()));
    return r.begin()[0].as<double>();
}

pqxx::result Database::cancelOrder(int orderId, int accountId) {
    pqxx::result r = getOrderByStatus(orderId, accountId, STATUS_OPEN);
    if (r.size() == 0) {
        throw std::invalid_argument(NO_OPEN_ORDER_ERROR);
    }

    // refund
    std::string symbol = r.begin()[1].as<std::string>();
    double amount = r.begin()[2].as<double>();
    double limitPrice = r.begin()[3].as<double>();
    if (amount < 0) {
        updateAmount(symbol, accountId, -amount);  // negative amount, sell order, refund shares
    }
    else {
        updateBalance(accountId, limitPrice * amount); // buy order, refund price
    }

    updateCancelOrder(orderId, accountId);
    return getOrder(orderId, accountId);
}

pqxx::result Database::getOrderByStatus(int orderId, int accountId, std::string status) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId;
    if (not status.empty()) {
        ss << " AND status = " << n.quote(status);
    }
    ss << " ORDER BY status DESC;";
    return pqxx::result(n.exec(ss.str()));
}

pqxx::result Database::getOrder(int orderId, int accountId) {
    return getOrderByStatus(orderId, accountId, "");
}

void Database::updateCancelOrder(int orderId, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE trade_order"
       << " SET status = " << w.quote(STATUS_CANCELLED)
       << ", update_time = " << time(NULL)
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId
       << " AND status = " << w.quote(STATUS_OPEN) << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

pqxx::result Database::getBuyOrder(double sellLimit, std::string symbol, int sellerAccountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE symbol = " << n.quote(symbol) << " AND amount > 0 AND limit_price >= " << sellLimit
       << " AND status = " << n.quote(STATUS_OPEN) << " AND account_id != " << sellerAccountId
       << " ORDER BY limit_price DESC, update_time ASC, order_id ASC";
    return pqxx::result(n.exec(ss.str()));
}

pqxx::result Database::getSellOrder(double buyLimit, std::string symbol, int buyerAccountId) {
    pqxx::nontransaction n(*conn);
    std::stringstream ss;
    ss << "SELECT * FROM trade_order"
       << " WHERE symbol = " << n.quote(symbol) << " AND amount < 0 AND limit_price <= " << buyLimit
       << " AND status = " << n.quote(STATUS_OPEN) << " AND account_id != " << buyerAccountId
       << " ORDER BY limit_price ASC, update_time ASC, order_id ASC";
    return pqxx::result(n.exec(ss.str()));

}

void Database::executeBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double executeAmount,
                               double remainAmount, double buyLimit, double executePrice) {
    updatePosition(symbol, buyerAccountId, executeAmount);
    // refund if buyer's limit price is higher than execution price
    updateBalance(buyerAccountId, executeAmount * (buyLimit - executePrice));
    updateOpenOrder(buyOrderId, buyerAccountId, remainAmount);
    saveOrder(buyOrderId, symbol, executeAmount, 0, STATUS_EXECUTED, executePrice, buyerAccountId);
}

void Database::executeSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double executeAmount,
                                double remainAmount, double executePrice) {
    updateBalance(sellerAccountId, executeAmount * executePrice);
    updateOpenOrder(sellOrderId, sellerAccountId, remainAmount);
    saveOrder(sellOrderId, symbol, -executeAmount, 0, STATUS_EXECUTED, executePrice, sellerAccountId);
}

void Database::saveOrder(int orderId, std::string symbol, double amount, double limitPrice, std::string status,
                         double executePrice, int accountId) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "INSERT INTO trade_order (order_id, symbol, amount, limit_price, status, update_time, execute_price, account_id) VALUES ("
       << orderId << "," << w.quote(symbol) << "," << amount << "," << limitPrice << ","
       << w.quote(status) << "," << time(NULL) << "," << executePrice << "," << accountId << ");";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

void Database::updateOpenOrder(int orderId, int accountId, double remainAmount) {
    pqxx::work w(*conn);
    std::stringstream ss;
    ss << "UPDATE trade_order"
       << " SET amount = " << remainAmount
       << " WHERE account_id = " << accountId << " AND order_id = " << orderId
       << " AND status = " << w.quote(STATUS_OPEN) << ";";
    try {
        w.exec(ss.str());
        w.commit();
    } catch (pqxx::sql_error &e) {
        w.abort();
    }
}

void Database::handleSellOrder(int sellOrderId, std::string symbol, int sellerAccountId, double sellAmount,
                               double sellLimit) {
    pqxx::result r = getBuyOrder(sellLimit, symbol, sellerAccountId);
    pqxx::result::const_iterator c = r.begin();
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
}

void Database::handleBuyOrder(int buyOrderId, std::string symbol, int buyerAccountId, double buyAmount,
                              double buyLimit) {
    pqxx::result r = getSellOrder(buyLimit, symbol, buyerAccountId);
    pqxx::result::const_iterator c = r.begin();
    while (buyAmount != 0 && c != r.end()) {
        int sellOrderId = c[0].as<int>();
        double sellAmount = c[2].as<double>();
        double executeAmount = std::min(-sellAmount, buyAmount);
        double sellLimit = c[3].as<double>();
        double executePrice = c[3].as<double>();
        int sellerAccountId = c[7].as<int>();
        executeBuyOrder(buyOrderId, symbol, buyerAccountId, executeAmount,
                        buyAmount - executeAmount, buyLimit, executePrice);
        executeSellOrder(sellOrderId, symbol, sellerAccountId, executeAmount,
                         sellAmount + executeAmount, executePrice);
        buyAmount -= executeAmount;
        ++c;
    }
}

Database::~Database() {
    conn->disconnect();
    delete conn;
}

