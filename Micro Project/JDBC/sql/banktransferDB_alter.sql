ALTER TABLE Accounts
ADD CONSTRAINT fk_acc_user_id FOREIGN KEY(user_id) REFERENCES Users(user_id);

ALTER TABLE Transfers
ADD CONSTRAINT fk_user_id FOREIGN KEY(user_id) REFERENCES Users(user_id),
ADD CONSTRAINT fk_source_id FOREIGN KEY(source_account_id) REFERENCES Accounts(account_number),
ADD CONSTRAINT fk_destination_id FOREIGN KEY(destination_account_id) REFERENCES Accounts(account_number);

ALTER TABLE Scheduled_Transfers
ADD CONSTRAINT fk_sched_user_id FOREIGN KEY(user_id) REFERENCES Users(user_id),
ADD CONSTRAINT fk_sched_source_id FOREIGN KEY(source_account_id) REFERENCES Accounts(account_number),
ADD CONSTRAINT fk_sched_destination_id FOREIGN KEY(destination_account_id) REFERENCES Accounts(account_number);