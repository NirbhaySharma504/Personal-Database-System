ALTER TABLE Bill 
ADD CONSTRAINT fk_payment_id FOREIGN KEY(Payment_ID) REFERENCES Payment(Payment_ID);

ALTER TABLE BillItem 
ADD CONSTRAINT fk_bill_id FOREIGN KEY(Bill_ID) REFERENCES Bill(Bill_ID) ON DELETE CASCADE,
ADD CONSTRAINT fk_billitem_product_id FOREIGN KEY(Product_ID) REFERENCES Product(Product_ID);

ALTER TABLE Payment 
ADD CONSTRAINT fk_payment_bill_id FOREIGN KEY(Bill_ID) REFERENCES Bill(Bill_ID);

ALTER TABLE CardPayment 
ADD CONSTRAINT fk_cardpayment_id FOREIGN KEY(CardPayment_ID) REFERENCES Payment(Payment_ID) ON DELETE CASCADE;

ALTER TABLE Product_Category 
ADD CONSTRAINT fk_productcategory_product_id FOREIGN KEY(Product_ID) REFERENCES Product(Product_ID),
ADD CONSTRAINT fk_productcategory_category_id FOREIGN KEY(Category_ID) REFERENCES Category(Category_ID),
ADD UNIQUE (Category_ID,Product_ID);

ALTER TABLE Stock
ADD CONSTRAINT fk_stock_store_id FOREIGN KEY(Store_ID) REFERENCES Store(Store_ID),
ADD CONSTRAINT fk_stock_product_id FOREIGN KEY(Product_ID) REFERENCES Product(Product_ID),
ADD UNIQUE (Store_ID,Product_ID);

ALTER TABLE ShelfLocation 
ADD CONSTRAINT fk_shelflocation_store_id FOREIGN KEY(Store_ID) REFERENCES Store(Store_ID),
ADD CONSTRAINT fk_shelflocation_product_id FOREIGN KEY(Product_ID) REFERENCES Product(Product_ID);
