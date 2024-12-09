
# Library Management System (LMS)

This Library Management System uses Operating Systems concepts to build a command-line application developed in C, designed to facilitate efficient management of library books and user accounts. The LMS supports essential library functionalities such as user account management, book lending, and return operations. It operates in a distributed environment using socket-based client-server communication and is designed to handle concurrent users through threading.

## Table of Contents

- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Testing](#testing)
- [Usage](#usage)
- [Design](#design)

## Features

### User Functionalities
- **View Books**: Browse all available books in the library.
- **Borrow/Return Books**: Borrow available books and return borrowed books.
- **View Borrowed Books**: See a list of books currently borrowed by the user.
- **Search Books**: Search for books by title or author.

### Admin Functionalities
- **User Management**: Add or remove members.
- **Book Management**: Add new books, delete books, and view all books in the library.
- **Transaction Management**: Lend books to members and accept book returns.
- **View User Details**: Display information on all members or search by username.

## System Requirements

- **Operating System**: Linux
- **Compiler**: GCC for compiling C programs
- **Libraries**: 
  - `pthreads` for concurrent operations
  - `CUnit` for unit tests

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/NikitaKiran/OS_LMS.git
   cd OS_LMS
   ```

2. Compile the code:
   ```bash
   make all
   ```
3. Run the inititialisation
   ```bash
   ./init
   ```

4. Run the server:
   ```bash
   ./server
   ```

5. Run the client (in a separate terminal):
   ```bash
   ./client
   ```
## Testing

The unit tests for this project have been implemented using **CUnit**, a lightweight testing framework for C programs. These tests ensure the correctness of critical functions defined in server_main.c.

### Running the tests

Before running the tests, ensure that **CUnit** is installed on your system. Follow the steps below to install CUnit:

#### For Linux:
   ```bash
   sudo apt install libcunit1 libcunit1-doc libcunit1-dev
   ```
#### For mac:
   ```bash
   brew install cunit
   ```

Once installation is complete, run the following:
   ```bash
   make test
   ```

## Usage

The system is designed for command-line usage with separate interfaces for Admin and User roles. The user can choose the required option from the displayed menu to perform the necessary action.

## Design

### 1. Database Design

The Library Management System (LMS) employs binary files to manage data storage efficiently. The design ensures high performance, data integrity, and concurrency in a multi-threaded environment.

#### Data Storage

##### Binary Files:
- **Two separate binary files:**
  - `books.dat`: Stores details of all books.
  - `members.dat`: Stores user account details and their borrowed books.

##### Metadata:
- Each file begins with two integers:
  - `current_count`: Number of active (non-null) records in the file.
  - `max_count`: Total number of records (including null or deleted entries).
- This metadata allows efficient record management without scanning the entire file.

#### Data Structures

```c
typedef struct book {
    int book_id;          // Unique identifier for each book
    char title[100];      // Title of the book
    char author[100];     // Author of the book
    int copies;           // Total copies in the library
    int available;        // Available copies for borrowing
} book;

typedef struct member {
    int member_id;        // Unique identifier for each member
    char username[100];   // Username for login
    char password[100];   // Password for login
    int num_books;        // Number of books currently borrowed
    book books[5];        // Array of up to 5 borrowed books
} member;
```

##### Sorted Storage:
- Records are stored in ascending order of IDs (`book_id` and `member_id`).
- Sorting allows for constant time lookups.

#### Operations

1. **Addition of Records:**
   - New records are inserted at their correct position to maintain sorted order.

2. **Deletion of Records:**
   - Logical deletion is implemented by replacing the record with a null equivalent struct.

3. **Updating Records:**
   - Locate the record by its ID, modify its values, and overwrite it in place while preserving sorted order.

4. **Retrieving Records:**
   - Constant time lookup by directly going to the position of the record in the file using its ID.

5. **File Initialization:** 
   - Files are initialized with the metadata.

---

### 2. Multi-Threading

The system uses threads to manage multiple concurrent client requests:
- **File Locks:** Ensure consistent access to shared resources like binary files.

---

### 3. Concurrency Management

#### Granular File Locks:
- Concurrency is handled at the file level using granular locks to ensure thread-safe operations.
- Locks are applied selectively to specific file regions (e.g., metadata or a particular record).
- This ensures better efficiency in performance as the entire file need not be locked for an operation on only one record.


