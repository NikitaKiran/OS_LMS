#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "server_func.h"
#include "globals.h"
#include <string.h>

void connection(int nsd)
{
    int nsfd = nsd;
    int verify = 0;
    int choice;
    int uid;
    int mem_fd = open("./member.dat", O_RDWR);
    while(verify == 0){
        read(nsfd, &choice, sizeof(choice));
        if (choice == 2)
        {
            char username[100], password[100];
            read(nsfd, username, sizeof(username));
            read(nsfd, password, sizeof(password));
            uid = getMemberID(username, mem_fd);
            if (uid == -1)
            {
                verify = 0;
            }
            else
            {
                member temp_member;
                temp_member = getMemberByID(uid, mem_fd);
                if (strcmp(temp_member.password, password) == 0)
                {
                    verify = 1;
                }
                else
                {
                    verify = 0;
                }
            }
        }
        else if (choice == 1)
        {
            char password[100];
            read(nsfd, password, sizeof(password));
            if (strcmp(password, ADMIN_PASSWORD) == 0)
            {
                verify = 1;
            }
            else
            {
                verify = 0;
            }
        }
        else
        {
            return ;
        }
        write(nsfd, &verify, sizeof(verify));
    }
    int book_fd = open("./book.dat", O_RDWR);
    if (choice == 1)
    {
        int option = 0;
        while(option != 11){
            read(nsfd, &option, sizeof(int));
            switch (option)
            {
            case 1:
                //add member
                {
                member temp;
                read(nsfd, &temp, sizeof(temp));
                addMember(temp, nsfd, mem_fd);}
                break;  
            case 2:
                //delete member
                {int member_id;
                read(nsfd, &member_id, sizeof(member_id));   
                removeMember(member_id, nsfd, mem_fd, book_fd);}
                break;
            case 3:
                {
                member temp_members[100];
                int count = getAllMembers(temp_members, mem_fd);
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &temp_members[i], sizeof(temp_members[i]));
                }}
                break;
            case 4:
                {book temp_book;
                read(nsfd, &temp_book, sizeof(temp_book));
                addBook(temp_book, nsfd, book_fd);
                }
                break;
            case 5:
                {int book_id;
                read(nsfd, &book_id, sizeof(book_id));
                removeBook(book_id, nsfd, book_fd);}
                break;
            case 6:
                {
                book books[100];
                int count = getAllBooks(books, book_fd);
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &books[i], sizeof(books[i]));
                }}
                break;
            case 7:
                {int member_id;
                read(nsfd, &member_id, sizeof(member_id));
                member temp_member;
                temp_member = getMemberByID(member_id, mem_fd);
                int count = temp_member.num_books;
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &temp_member.books[i], sizeof(temp_member.books[i]));
                }}
                break;
            case 8:
                {char username[100];
                
                read(nsfd, username, sizeof(username));
                member temp_member;
                temp_member = searchMember(username, mem_fd);
                write(nsfd, &temp_member, sizeof(temp_member));}
                break;
            case 9: 
                {
                int search_option;
                read(nsfd, &search_option, sizeof(search_option));
                if (search_option != 1 && search_option != 2){
                    break;
                }
                char answer[100];
                read(nsfd, answer, sizeof(answer));
                book books[100];
                int count = 0;
                if (search_option == 1){
                    count = searchBook(answer, books, book_fd);
                }
                else if (search_option == 2){
                    count = searchBookByAuthor(answer, books, book_fd);
                }
                else{
                    break;
                }
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &books[i], sizeof(books[i]));
                }}
                break;
            case 10:
                {
                    int option;
                    read(nsfd, &option, sizeof(option));
                    if (option == 1){
                        int member_id;
                        read(nsfd, &member_id, sizeof(member_id));
                        int book_id;
                        read(nsfd, &book_id, sizeof(book_id));
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 2 * sizeof(int) + (member_id - 1)*sizeof(member);
                        lock.l_len = sizeof(member);
                        lock.l_pid = getpid();
                        if (fcntl(mem_fd, F_SETLK, &lock) != 0){
                            write(nsfd, "Member is logged in. Please try again later.", sizeof("Member is logged in. Please try again later."));    
                        }
                        else{
                        borrowBook(member_id, book_id, nsfd, mem_fd, book_fd);
                        lock.l_type = F_UNLCK;
                        fcntl(mem_fd, F_SETLK, &lock);
                        }
                    }
                    else if (option == 2){
                        int member_id;
                        read(nsfd, &member_id, sizeof(member_id));
                        int book_id;
                        read(nsfd, &book_id, sizeof(book_id));
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 2 * sizeof(int) + (member_id - 1)*sizeof(member);
                        lock.l_len = sizeof(member);
                        lock.l_pid = getpid();
                        if (fcntl(mem_fd, F_SETLK, &lock) != 0){
                            write(nsfd, "Member is logged in. Please try again later.", sizeof("Member is logged in. Please try again later."));    
                        }
                        else{
                        returnBook(member_id, book_id, nsfd, mem_fd, book_fd);
                        lock.l_type = F_UNLCK;
                        fcntl(mem_fd, F_SETLK, &lock);
                        }
                    }
                    else{
                        break;
                    }
                break;
                }
            case 11:
                break;
            default:
                break;
            
            }
        }
    }
    else{
            struct flock lock;
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = 2 * sizeof(int) + (uid - 1)*sizeof(member);
            lock.l_len = sizeof(member);
            lock.l_pid = getpid();

        
            if (fcntl(mem_fd, F_SETLKW, &lock) == -1){
                perror("fcntl");
                return;
            }
        int option = 0;
        while(option != 7){
            read(nsfd, &option, sizeof(int));
            switch (option)
            {
            case 1:
                {book books[100];
                int count = getAllBooks(books, book_fd);
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &books[i], sizeof(books[i]));
                }}
                break;
            case 2:
                
                {int book_id;
                
                read(nsfd, &book_id, sizeof(book_id));
                lock.l_type = F_WRLCK;
                fcntl(mem_fd, F_SETLKW, &lock);
                borrowBook(uid, book_id, nsfd, mem_fd, book_fd);
                lock.l_type = F_RDLCK;
                fcntl(mem_fd, F_SETLKW, &lock);
                }
                break;
            case 3:
                {int book_id;
                
                read(nsfd, &book_id, sizeof(book_id));
                lock.l_type = F_WRLCK;
                fcntl(mem_fd, F_SETLKW, &lock);
                returnBook(uid, book_id, nsfd, mem_fd, book_fd);
                lock.l_type = F_RDLCK;
                fcntl(mem_fd, F_SETLKW, &lock);}
                break;
            case 4:
                {
                member temp_member;
                temp_member = getMemberByID(uid, mem_fd);
                fcntl(mem_fd, F_SETLKW, &lock);
                int count = temp_member.num_books;
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &temp_member.books[i], sizeof(temp_member.books[i]));
                }
                }
                break;
            case 5:
                {int search_option;
                read(nsfd, &search_option, sizeof(search_option));
                char answer[100];
                read(nsfd, answer, sizeof(answer));
                book books[100];
                int count = 0;
                if (search_option == 1){
                    count = searchBook(answer, books, book_fd);
                }
                else if (search_option == 2){
                    count = searchBookByAuthor(answer, books, book_fd);
                }
                else{
                    break;
                }
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &books[i], sizeof(books[i]));
                }}
                break;
            case 6:
                {
                book books[100];
                int count = getAllBooks(books, book_fd);
                write(nsfd, &count, sizeof(count));
                for (int i = 0; i < count; i++){
                    write(nsfd, &books[i], sizeof(books[i]));
                }}
                break;
            case 7:
                lock.l_type = F_UNLCK;
                fcntl(mem_fd, F_SETLK, &lock);
                break;
            default:
                break;
            }
        }
    }
    close(mem_fd);
    close(book_fd);
    return;
}

int main()
{
    struct sockaddr_in server, client;
    int sd, nsd, clientLen;
    pthread_t threads;
    bool result;
    sd = socket(AF_INET, SOCK_STREAM, 0);

    // Keeping localhost as the server address, same as client for demo purpose
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(8080);

    bind(sd, (struct sockaddr *)&server, sizeof(server));
    if (listen(sd, 50) < 0)
    {
        perror("listen");
        exit(1);
    }

    write(1, "Waiting for the clients.....\n", sizeof("Waiting for the client.....\n"));

    while (1)
    {
        clientLen = sizeof(client);
        nsd = accept(sd, (struct sockaddr *)&client, &clientLen);
        write(1, "Connected to the client...\n", sizeof("Connected to the client...\n"));
        int r = fork();
        if (r == 0)
        {
            connection(nsd);
            close(nsd);
            break;
        }
        else
        {
            close(nsd);
        }
    }
    close(sd);
    return 0;
}