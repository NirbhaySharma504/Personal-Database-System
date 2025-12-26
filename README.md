# Personal Database System (PDS)

A comprehensive implementation of a **Personal Database System** built from scratch in C, developed as part of the Database Management Systems (DBMS) course at IIIT Bangalore (Sem 4).

**Author:** IMT2023103  
**Course:** DBMS Lab

---

## 📋 Overview

This project implements a file-based database system that progressively evolves from simple binary I/O operations to a full-fledged multi-table database with BST-based indexing. The system demonstrates core database concepts including:

- Binary file I/O operations
- Index management (Array → BST)
- CRUD operations
- Record deletion with soft-delete support
- Multi-table database architecture
- Schema file implementation

---

## 🏗️ Project Structure

```
├── Lab 01 - Binary IO/           # Foundation: Binary file operations
├── Lab 01 - PDS v01/             # Basic PDS with append-mode storage
├── Lab 03 - PDS v02/             # Array-based index implementation
├── Lab 04 - PDS v3/              # Function pointers & generic operations
├── Lab 05 - PDS v4/              # BST-based index integration
├── Lab 06 - PDS v5/              # DELETE operation with soft-delete
├── Lab 07 - PDS v6/              # Multi-table database support
├── Bonus PDS Activity/           # Schema file implementation (v7)
├── ReIndex/                      # Index reconstruction utility
├── Lab 10 - SQL DDL/             # SQL Data Definition Language
├── LAB 11 - SQL DML/             # SQL Data Manipulation Language
├── Lab 12/                       # Additional SQL exercises
├── Micro Project/JDBC/           # Bank Transfer Application (Java/JDBC)
└── My JDBC/                      # JDBC experimentation
```

---

## 🔄 PDS Version Evolution

### Version 1 (v1) - Basic Storage
- Simple append-mode binary file storage
- Basic `pds_open()` and `pds_close()` operations
- Sequential record storage in `.dat` files

### Version 2 (v2) - Array Index
- Introduces separate index file (`.ndx`)
- Array-based index with `PDS_NdxInfo` structure (key, offset)
- Index loaded into memory on open
- Binary search for record lookup

### Version 3 (v3) - Function Pointers
- Generic record handling using function pointers
- Decoupled record serialization/deserialization
- Support for different record types (Contact, etc.)

### Version 4 (v4) - BST Index
- Replaces array index with Binary Search Tree
- Efficient O(log n) lookups
- BST operations: `bst_add_node()`, `bst_search()`, `bst_free()`
- Pre-order traversal for index persistence

### Version 5 (v5) - Delete Operation
- Soft-delete support with `is_deleted` flag
- `pds_delete()` function implementation
- Records marked as deleted but not physically removed

### Version 6 (v6) - Multi-Table Support
- Database-level operations (`pds_create_db()`, `pds_open_db()`)
- Multiple tables per database
- Folder-based database organization
- `PDS_DBInfo` and `PDS_TableInfo` structures

### Version 7 (v7) - Schema Files
- Persistent schema information in `.info` files
- Auto-recovery of table metadata on database open
- Enhanced database management

---

## 📁 Core Components

### Data Structures

```c
// Index Entry
struct PDS_NdxInfo {
    int key;
    int offset;
    int is_deleted;  // REC_DELETED or REC_NOT_DELETED
};

// Repository Info (Single Table)
struct PDS_RepoInfo {
    char pds_name[30];
    FILE *pds_data_fp;
    FILE *pds_ndx_fp;
    int repo_status;
    int rec_size;
    int rec_count;
    struct BST_Node *ndx_root;
};

// Database Info (Multi-Table)
struct PDS_DBInfo {
    char pds_repo_name[30];
    int repo_status;
    int num_tables;
    struct PDS_TableInfo tables[MAX_TABLES];
};
```

### API Functions

| Function | Description |
|----------|-------------|
| `pds_create()` | Create new data and index files |
| `pds_open()` | Open repository and load index |
| `pds_close()` | Save index and close files |
| `pds_store()` | Insert new record |
| `pds_search()` | Find record by key |
| `pds_delete()` | Soft-delete a record |
| `pds_create_db()` | Create multi-table database |
| `pds_open_db()` | Open database folder |
| `pds_create_table()` | Create table within database |
| `pds_open_table()` | Open specific table |

### Error Codes

```c
#define PDS_SUCCESS         0
#define PDS_FILE_ERROR      1
#define PDS_ADD_FAILED      2
#define PDS_REC_NOT_FOUND  -1
#define PDS_REPO_ALREADY_OPEN  12
#define PDS_NDX_SAVE_FAILED    13
#define PDS_REPO_NOT_OPEN      14
#define PDS_DELETE_FAILED      15
```

---

## 🛠️ Compilation & Usage

### Compile PDS Library
```bash
gcc -c pdsv6.c bst.c IMT2023103_contact.c
gcc -o pds_tester pds_testerv6.c pdsv6.o bst.o IMT2023103_contact.o
```

### Run Tests
```bash
./pds_tester < testcasev6.in
```

### ReIndex Utility
```bash
gcc -o reindex reindex.c
./reindex <repository_name>
```

---

## 💾 SQL Components

### Lab 10 - DDL (Data Definition Language)
- Database creation (`storedb`)
- Table definitions with constraints
- Primary keys, foreign keys, NOT NULL, UNIQUE
- ALTER TABLE operations

### Lab 11 - DML (Data Manipulation Language)
- INSERT operations
- UPDATE statements
- Complex queries on `companydb`

---

## ☕ JDBC Micro Project - Bank Transfer Application

A complete **Bank Transfer System** implemented in Java using JDBC.

### Features
- Fund transfers between accounts
- Scheduled transfers
- Transaction management with rollback
- Admin view functionality
- Input validation

### Run Instructions
```bash
cd Micro\ Project/JDBC/src
export CLASSPATH='../lib/mysql-connector-j-8.0.32.jar:.'
javac BankTransferApp.java
java BankTransferApp
```

### Database Setup
```sql
CREATE DATABASE banktransferDB;
-- Create required tables: users, accounts, transactions, scheduled_transfers
```

---

## 📚 Key Learnings

1. **Binary I/O**: Never use pointers in structs stored in files - the pointer address is saved, not the data
2. **File Operations**: `fopen()`, `fread()`, `fwrite()`, `fseek()` for binary file manipulation
3. **Index Management**: BST provides O(log n) lookups vs O(n) for linear search
4. **Soft Delete**: Mark records as deleted without physical removal for data recovery
5. **Transaction Management**: JDBC transactions with commit/rollback for data integrity

---

## 🔧 Technologies Used

- **C** - Core PDS implementation
- **Binary Search Tree** - Index data structure
- **Java/JDBC** - Database connectivity
- **MySQL** - Relational database for SQL labs
- **SQL** - DDL and DML operations

---

## 📂 File Formats

| Extension | Description |
|-----------|-------------|
| `.dat` | Binary data file containing records |
| `.ndx` | Index file with key-offset mappings |
| `.info` | Database schema/metadata file |

---

## 🧪 Testing

Each PDS version includes:
- `pds_tester.c` - Test driver program
- `testcase.in` - Input test cases
- Expected output validation

Test commands:
```
CREATEDB <dbname>
OPENDB <dbname>
CREATETABLE <tablename>
INSERT <tablename> <key> <data>
SEARCH <tablename> <key>
DELETE <tablename> <key>
CLOSEDB <dbname>
```

---

## 📝 License

Academic project - IIIT Bangalore DBMS Course
