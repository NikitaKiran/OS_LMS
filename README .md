
# Library Management System (LMS)

This Library Management System uses Operating Systems concepts to build a command-line application developed in C, designed to facilitate efficient management of library books and user accounts. The LMS supports essential library functionalities such as user account management, book lending, and return operations. It operates in a distributed environment using socket-based client-server communication and is designed to handle concurrent users through threading.

## Table of Contents

- [Features](#features)
- [System Requirements](#system-requirements)
- [Installation](#installation)
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

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/NikitaKiran/OS_LMS.git
   cd OS_LMS
   ```

2. Compile the code:
   ```bash
   gcc init.c -o init
   gcc server.c -o server -lpthread
   gcc client.c -o client -lpthread
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

## Usage

The system is designed for command-line usage with separate interfaces for Admin and User roles. The user can choose the required option from the displayed menu to perform the necessary action.

## Design

### Product Perspective
The LMS consists of a server that manages all data operations and multiple clients that interact through TCP/IP socket communication. Using threads, the system allows concurrent access for multiple users while maintaining data integrity with mutex locks for safe read/write operations.

### Data Storage
All data, including user and book information, is stored in binary files to ensure quick access and recovery in case of failures.

### Concurrency and Locking
The LMS uses file locks to ensure secure and consistent data management across concurrent client interactions.

