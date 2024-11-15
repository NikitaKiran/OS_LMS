// Client side code for the OLMS
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "member.h"

#define PORT 8080

void printAdminMenu()
{
    printf("-----------MENU-----------\n");
    printf("1.Add member\n");
    printf("2.Delete member\n");
    printf("3.View all members\n");
    printf("4.Add book\n");
    printf("5.Delete book\n");
    printf("6.View all books\n");
    printf("7.View books borrowed by a member\n");
    printf("8.Search for a member by username\n");
    printf("9.Search for a book\n");
    printf("10.Perform member transactions\n");
    printf("11.Exit\n");
    printf("Enter your choice: ");
}

void printMemberMenu()
{
    printf("-----------MENU-----------\n");
    write(1, "1.View all books\n", sizeof("1.View all books\n"));
    write(1, "2.Borrow a book\n", sizeof("2.Borrow a book\n"));
    write(1, "3.Return a book\n", sizeof("3.Return a book\n"));
    write(1, "4.View borrowed books\n", sizeof("4.View borrowed books\n"));
    write(1, "5.Search for a book\n", sizeof("5.Search for a book\n"));
    write(1, "6.View all available books\n", sizeof("6.View all available books\n"));
    write(1, "7.Exit\n", sizeof("7.Exit\n"));
    printf("Enter your choice: ");
}

void displayBookDetails(book temp_book)
{
    if (temp_book.book_id == -1)
    {
        write(1, "Invalid book ID\n", sizeof("Invalid book ID\n"));
        return;
    }
    printf("Book ID: %d\n", temp_book.book_id);
    printf("Title: %s\n", temp_book.title);
    printf("Author: %s\n", temp_book.author);
    printf("Number of copies: %d\n", temp_book.copies);
    printf("Available copies: %d\n\n", temp_book.available);
}

void displayBorrowedBookDetails(book temp_book)
{
    if (temp_book.book_id == -1)
    {
        write(1, "Invalid book ID\n", sizeof("Invalid book ID\n"));
        return;
    }
    printf("Book ID: %d\n", temp_book.book_id);
    printf("Title: %s\n", temp_book.title);
    printf("Author: %s\n\n", temp_book.author);
}

void displayMemberDetails(member temp_member)
{
    if (temp_member.member_id == -1)
    {
        write(1, "Invalid member ID\n", sizeof("Invalid member ID\n"));
        return;
    }
    printf("Member ID: %d\n", temp_member.member_id);
    printf("Username: %s\n", temp_member.username);
    printf("Number of books borrowed: %d\n\n", temp_member.num_books);
}

int main()
{
    struct sockaddr_in serv;
    int sd;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv.sin_port = htons(PORT);

    connect(sd, (struct sockaddr *)&serv, sizeof(serv));
    int verify = 0;
    int choice;
    while (verify == 0)
    {
        write(1, "Enter 1 to login as admin\nEnter 2 to login as user\n", sizeof("Enter 1 to login as admin\nEnter 2 to login as user\n"));
        scanf("%d", &choice);
        write(sd, &choice, sizeof(choice));
        if (choice == 2)
        {
            char username[100], password[100];
            write(1, "Enter username: \n", sizeof("Enter username: \n"));
            scanf("%s", username);
            write(sd, username, sizeof(username));
            write(1, "Enter password: \n", sizeof("Enter password: \n"));
            scanf("%s", password);
            write(sd, password, sizeof(password));
        }
        else if (choice == 1)
        {
            char password[100];
            write(1, "Enter admin password: \n", sizeof("Enter admin password: \n"));
            scanf("%s", password);
            write(sd, password, sizeof(password));
        }
        else
        {
            write(1, "Invalid choice\n", sizeof("Invalid choice\n"));
            close(sd);
            return 0;
        }
        read(sd, &verify, sizeof(verify));
        if (verify == 1)
        {
            write(1, "Login successful\n", sizeof("Login successful\n"));
        }
        else
        {
            write(1, "Login failed\n", sizeof("Login failed\n"));
        }
    }
    if (choice == 1)
    {
        int option = 0;
        while (option != 11)
        {
            printAdminMenu();
            scanf("%d", &option);
            write(sd, &option, sizeof(int));
            switch (option)
            {

            case 1:
            {
                // add member
                member temp_member;
                write(1, "Enter member username: \n", sizeof("Enter member username: \n"));
                scanf("%s", temp_member.username);
                write(1, "Enter member password: \n", sizeof("Enter member password: \n"));
                scanf("%s", temp_member.password);
                temp_member.num_books = 0;
                write(sd, &temp_member, sizeof(temp_member));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 2:
            {
                // delete member
                write(1, "Enter member ID: \n", sizeof("Enter member ID: \n"));
                int member_id;
                scanf("%d", &member_id);
                write(sd, &member_id, sizeof(member_id));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 3:
            {
                //view all members
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No members available\n", sizeof("No members available\n"));
                    break;
                }
                for (int i = 0; i < count; i++)
                {
                    member temp_member;
                    read(sd, &temp_member, sizeof(temp_member));
                    displayMemberDetails(temp_member);
                }
                break;
            }
            case 4:
            {
                // add book
                book temp_book;
                write(1, "Enter book title: \n", sizeof("Enter book title: \n"));
                scanf("%s", temp_book.title);
                write(1, "Enter book author: \n", sizeof("Enter book author: \n"));
                scanf("%s", temp_book.author);
                write(1, "Enter number of copies: \n", sizeof("Enter number of copies: \n"));
                scanf("%d", &temp_book.copies);
                temp_book.available = temp_book.copies;
                write(sd, &temp_book, sizeof(temp_book));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 5:
            {
                // delete book
                write(1, "Enter book ID: \n", sizeof("Enter book ID: \n"));
                int book_id;
                scanf("%d", &book_id);
                write(sd, &book_id, sizeof(book_id));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 6:
            {
                // view all books
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No books available\n", sizeof("No books available\n"));
                    break;
                }
                for (int i = 0; i < count; i++)
                {
                    book temp_book;
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBookDetails(temp_book);
                }
                break;
            }
            case 7:
            {
                // view books borrowed by a member
                int member_id;
                write(1, "Enter member ID: \n", sizeof("Enter member ID: \n"));
                scanf("%d", &member_id);
                write(sd, &member_id, sizeof(member_id));
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No books borrowed\n", sizeof("No books borrowed\n"));
                    break;
                }
                for (int i = 0; i < count; i++)
                {
                    book temp_book;
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBorrowedBookDetails(temp_book);
                }
                break;
            }
            case 8:
            {
                // search for a member by username
                write(1, "Enter member name: \n", sizeof("Enter member name: \n"));
                char username[100];
                scanf("%s", username);
                write(sd, username, sizeof(username));
                member temp_member;
                read(sd, &temp_member, sizeof(temp_member));
                if (temp_member.member_id == -1)
                {
                    write(1, "Member not found\n", sizeof("Member not found\n"));
                    break;
                }
                displayMemberDetails(temp_member);
                break;
            }
            case 9:
            {
                // search for a book
                printf("1. Search by Book Title\n2. Search by Book Author\nOption: ");
                int search_option;
                scanf("%d", &search_option);
                write(sd, &search_option, sizeof(search_option));
                if (search_option == 1)
                {
                    write(1, "Enter book title: \n", sizeof("Enter book title: \n"));
                }
                else if (search_option == 2)
                {
                    write(1, "Enter book author: \n", sizeof("Enter book author: \n"));
                }
                else
                {
                    write(1, "Invalid option\n", sizeof("Invalid option\n"));
                    break;
                }

                char answer[100];
                scanf("%s", answer);
                write(sd, answer, sizeof(answer));
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No matching books\n", sizeof("No matching books\n"));
                    break;
                }
                book temp_book;
                for (int i = 0; i < count; i++)
                {
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBookDetails(temp_book);
                }
                break;
            }
            case 10:
                // perform member transactions
                printf("1. Borrow a book\n2. Return a book\nOption: ");
                int transaction_option;
                scanf("%d", &transaction_option);
                write(sd, &transaction_option, sizeof(transaction_option));
                if (transaction_option == 1)
                {
                    write(1, "Enter member ID: \n", sizeof("Enter member ID: \n"));
                    int member_id;
                    scanf("%d", &member_id);
                    write(sd, &member_id, sizeof(member_id));
                    write(1, "Enter book ID: \n", sizeof("Enter book ID: \n"));
                    int book_id;
                    scanf("%d", &book_id);
                    write(sd, &book_id, sizeof(book_id));
                    char message[100];
                    read(sd, message, sizeof(message));
                    printf("%s\n", message);
                }
                else if (transaction_option == 2)
                {
                    write(1, "Enter member ID: \n", sizeof("Enter member ID: \n"));
                    int member_id;
                    scanf("%d", &member_id);
                    write(sd, &member_id, sizeof(member_id));
                    write(1, "Enter book ID: \n", sizeof("Enter book ID: \n"));
                    int book_id;
                    scanf("%d", &book_id);
                    write(sd, &book_id, sizeof(book_id));
                    char message[100];
                    read(sd, message, sizeof(message));
                    printf("%s\n", message);
                }
                else
                {
                    write(1, "Invalid option\n", sizeof("Invalid option\n"));
                    break;
                }
                break;
            case 11:    
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }
    }
    else
    {
        int option = 0;
        while (option != 7)
        {
            printMemberMenu();
            scanf("%d", &option);
            write(sd, &option, sizeof(int));
            switch (option)
            {
            case 1:
            {
                // view all books
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No books available\n", sizeof("No books available\n"));
                    break;
                }
                for (int i = 0; i < count; i++)
                {
                    book temp_book;
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBookDetails(temp_book);
                }
                break;
            }
            case 2:
            {
                // borrow a book
                write(1, "Enter book ID: \n", sizeof("Enter book ID: \n"));
                int book_id;
                scanf("%d", &book_id);
                write(sd, &book_id, sizeof(book_id));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 3:
            {
                // return a book
                write(1, "Enter book ID: \n", sizeof("Enter book ID: \n"));
                int book_id;
                scanf("%d", &book_id);
                write(sd, &book_id, sizeof(book_id));
                char message[100];
                read(sd, message, sizeof(message));
                printf("%s\n", message);
                break;
            }
            case 4:
            {
                // view borrowed books
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No books borrowed\n", sizeof("No books borrowed\n"));
                    break;
                }
                for (int i = 0; i < count; i++)
                {
                    book temp_book;
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBorrowedBookDetails(temp_book);
                }
                break;
            }
            case 5:
            {
                // search for a book
                printf("1. Search by Book Title\n2. Search by Book Author\nOption: ");
                int search_option;
                scanf("%d", &search_option);
                write(sd, &search_option, sizeof(search_option));
                if (search_option == 1)
                {
                    write(1, "Enter book title: \n", sizeof("Enter book title: \n"));
                }
                else if (search_option == 2)
                {
                    write(1, "Enter book author: \n", sizeof("Enter book author: \n"));
                }
                else
                {
                    write(1, "Invalid option\n", sizeof("Invalid option\n"));
                    break;
                }
                char answer[100];
                scanf("%s", answer);
                write(sd, answer, sizeof(answer));
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No matching books\n", sizeof("No matching books\n"));
                    break;
                }
                book temp_book;
                for (int i = 0; i < count; i++)
                {
                    read(sd, &temp_book, sizeof(temp_book));
                    displayBookDetails(temp_book);
                }
                break;
            }
            case 6:
            {
                // view all available books
                int count;
                read(sd, &count, sizeof(count));
                if (count == 0)
                {
                    write(1, "No books available\n", sizeof("No books available\n"));
                    break;
                }
                int displayed = 0;
                for (int i = 0; i < count; i++)
                {
                    book temp_book;
                    read(sd, &temp_book, sizeof(temp_book));
                    if (temp_book.available > 0)
                    {
                        displayBookDetails(temp_book);
                        displayed = 1;
                    }
                }
                if (displayed == 0)
                {
                    write(1, "No books available\n", sizeof("No books available\n"));
                }
                break;
            }
            case 7:
                break;
            default:
                printf("Invalid option\n");
                break;
            }
        }
    }
    close(sd);

    return 0;
}