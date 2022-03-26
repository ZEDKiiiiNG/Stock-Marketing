DROP TABLE IF EXISTS account CASCADE;
DROP TABLE IF EXISTS position CASCADE;

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
