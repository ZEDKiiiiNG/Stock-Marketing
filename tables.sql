DROP TABLE IF EXISTS account CASCADE;

CREATE TABLE account (
    account_id INT NOT NULL,
    balance INT DEFAULT 0,
    PRIMARY KEY (account_id)
);
