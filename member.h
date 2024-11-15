
#ifndef MEMBER_H
#include "book.h"
#define MEMBER_H
typedef struct member
{
    int member_id;
    char username[100];
    char password[100];
    int num_books;
    book books[5];
}member;
#endif