DROP TABLE IF EXISTS account CASCADE;

CREATE TABLE account (
    account_id INT NOT NULL,
    balance DOUBLE DEFAULT 0,
    PRIMARY KEY (account_id)
);

CREATE TABLE position (
    symbol VARCHAR(256),
    amount DOUBLE DEFAULT 0,
    FOREIGN KEY (account_id) REFERENCES STATE(account_id) ON DELETE SET NULL ON UPDATE CASCADE,
);
