#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "member.h"
#include <string.h>

int getMaxMemberID(int fd);

int getMemberCount(int fd);

int getBookCount(int fd);

int getMaxBookID(int fd);

book getBookByID(int id, int fd);

member getMemberByID(int id, int fd);

member searchMember(char username[], int fd);

int getMemberID(char username[], int fd);

int borrowBook(int member_id, int book_id, int sd, int fd, int fd2);

int returnBook(int member_id, int book_id, int sd, int fd, int fd2);

int searchBook(char title[], book books[], int fd);
int searchBookByAuthor(char author[], book books[], int fd);
int getAllBooks(book books[], int fd);

int getAllMembers(member members[], int fd);
void addBook(book b, int sd, int fd);

void addMember(member m, int sd, int fd);

void removeBook(int id, int sd, int fd);
void removeMember(int id, int sd, int fd, int fd2);
