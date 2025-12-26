create database storedb;
use storedb;
create table Bill(
    Bill_ID INT,
    BillDate DATE NOT NULL,
    TotalBill INT NOT NULL,
    Payment_ID INT,
    CONSTRAINT pk_bill PRIMARY KEY(Bill_ID)   
);
create table BillItem(
    BillItem_ID INT,
    Quantity INT NOT NULL,
    TotalPrice INT NOT NULL,
    Bill_ID INT NOT NULL,
    Product_ID INT NOT NULL,
    CONSTRAINT pk_billitem PRIMARY KEY(BillItem_ID)
);
create table Payment(
    Payment_ID INT,
    PaymentDate DATE NOT NULL,
    PaymentType VARCHAR(30) NOT NULL,
    PaymentAmount INT NOT NULL,
    Bill_ID INT NOT NULL,
    CONSTRAINT pk_payment PRIMARY KEY(Payment_ID)
);
create table CardPayment(
    CardPayment_ID INT,
    CardNumber INT NOT NULL UNIQUE,
    ExpiryDate DATE NOT NULL,
    AuthReferenceNumber INT NOT NULL,
    CONSTRAINT pk_cardpayment PRIMARY KEY(CardPayment_ID)
);
create table Product(
    Product_ID INT,
    SKU INT NOT NULL UNIQUE,
    ProdName VARCHAR(30) NOT NULL,
    Price VARCHAR(30) NOT NULL,
    CONSTRAINT pk_product PRIMARY KEY(Product_ID)
);
create table Category(
    Category_ID INT,
    CategoryName VARCHAR(30) NOT NULL,
    CategoryDesc VARCHAR(30),
    CONSTRAINT pk_category PRIMARY KEY(Category_ID)
);
create table Product_Category(
    Product_Category_ID INT,
    Product_ID INT NOT NULL,
    Category_ID INT NOT NULL,
    CONSTRAINT pk_product_category PRIMARY KEY(Product_Category_ID)
);
create table Store(
    Store_ID INT,
    StoreName VARCHAR(30) NOT NULL,
    Address VARCHAR(30),
    CONSTRAINT pk_store PRIMARY KEY(Store_ID)
);
create table Stock(
    Stock_ID INT,
    Store_ID INT NOT NULL,
    Product_ID INT NOT NULL,
    ShelfQty INT NOT NULL,
    StorageQty INT NOT NULL,
    ReorderThreshold INT,
    StockDate DATE NOT NULL,
    CONSTRAINT pk_stock PRIMARY KEY(Stock_ID)
);
create table ShelfLocation(
    ShelfLocation_ID INT,
    Store_ID INT,
    Product_ID INT NOT NULL,
    FloorNumber INT NOT NULL,
    AisleName VARCHAR(30) NOT NULL,
    ShelfNumber INT NOT NULL,
    CONSTRAINT pk_shelflocation PRIMARY KEY(ShelfLocation_ID)
);