#include <stdio.h>
#include <fcntl.h>
#include "member.h"
#include <unistd.h>

// Run this on the server side to initialise the member.dat and book.dat files with some dummy values
int main()
{
    int fd = open("./member.dat", O_CREAT | O_WRONLY, 0644);
    if (fd == -1)
    {
        printf("Failed to open file for writing.\n");
        return 1;
    }
    int count = 10;
    write(fd, &count, sizeof(int));
    write(fd, &count, sizeof(int));
    member temp_member;
    for (int i = 0; i < 10; i++)
    {
        temp_member.member_id = i + 1;
        sprintf(temp_member.username, "user%d", i + 1);
        sprintf(temp_member.password, "pass%d", i + 1);
        temp_member.num_books = 0;
        write(fd, &temp_member, sizeof(temp_member));
    }
    close(fd);

    fd = open("./book.dat", O_CREAT | O_WRONLY, 0644);

    if (fd == -1)
    {
        printf("Failed to open file for writing.\n");
        return 1;
    }
    book temp_book;
    write(fd, &count, sizeof(int));
    write(fd, &count, sizeof(int));
    for (int i = 0; i < 10; i++)
    {
        temp_book.book_id = i + 1;
        sprintf(temp_book.title, "Book%d", i + 1);
        sprintf(temp_book.author, "Author%d", i + 1);
        temp_book.copies = 5;
        temp_book.available = 5;
        write(fd, &temp_book, sizeof(temp_book));
    }
    close(fd);
    return 0;
}