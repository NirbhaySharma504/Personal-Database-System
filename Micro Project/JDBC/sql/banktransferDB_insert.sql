-- Insert sample users
INSERT INTO Users (name, email, user_type) VALUES
('Alice Johnson', 'alice@example.com', 'customer'),
('Bob Smith', 'bob@example.com', 'customer'),
('Admin User', 'admin@bank.com', 'admin');

-- Insert sample accounts
INSERT INTO Accounts (user_id, account_type, balance) VALUES
(1, 'savings', 5000),
(1, 'checking', 3000),
(2, 'savings', 7000),
(2, 'checking', 4000);
