CREATE DATABASE banktransferDB;
use banktransferDB;
CREATE TABLE Users(
    user_id INT AUTO_INCREMENT PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE,
    user_type ENUM('customer','admin')
);

CREATE TABLE Accounts(
    account_number INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    account_type ENUM('savings','checking'),
    balance INT DEFAULT 0
);

CREATE TABLE Transfers(
    transfer_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    source_account_id INT NOT NULL,
    destination_account_id INT NOT NULL,
    amount INT DEFAULT 0,
    completed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
CREATE TABLE Scheduled_Transfers(
    schedule_id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    source_account_id INT NOT NULL,
    destination_account_id INT NOT NULL,
    amount INT DEFAULT 0,
    scheduled_for DATE NOT NULL,
    status ENUM('scheduled', 'completed', 'cancelled'),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);