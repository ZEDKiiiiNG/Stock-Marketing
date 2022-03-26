DROP TABLE IF EXISTS account CASCADE;
DROP TABLE IF EXISTS position CASCADE;
DROP TABLE IF EXISTS trade_order CASCADE;

CREATE TABLE account (
    account_id INT,
    balance DOUBLE PRECISION DEFAULT 0,
    PRIMARY KEY (account_id)
);

CREATE TABLE position (
    symbol VARCHAR(256),
    amount DOUBLE PRECISION DEFAULT 0,
    account_id INT,
    FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE
);

CREATE TABLE trade_order (
    order_id INT,
    symbol VARCHAR(256),
    amount DOUBLE PRECISION,
    limit_price DOUBLE PRECISION,
    status VARCHAR(256) DEFAULT 'open',
    update_time INT,
    execute_price DOUBLE PRECISION,
    account_id INT,
    PRIMARY KEY (order_id),
    FOREIGN KEY (account_id) REFERENCES account(account_id) ON DELETE SET NULL ON UPDATE CASCADE
);
