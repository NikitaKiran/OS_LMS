#ifndef BOOK_H
#define BOOK_H
typedef struct book{
    int book_id;
    char title[100];
    char author[100];
    int copies;
    int available;
} book;
#endif
