#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "member.h"
#include <string.h>
#include <pthread.h>

struct argument{
    int fd;
    book b;
    int position;
};

void * write__book_into_file (void * arg){ 
    struct argument  args = *(struct argument *) arg;
    int fd = args.fd;
    book b = args.b;
    int position = args.position;
    lseek(fd, position, SEEK_SET);
    write(fd, &b, sizeof(book));
    return NULL;

}
int getMaxMemberID(int fd){
    int count = -1;
    lseek(fd, 0, SEEK_SET);
    read(fd, &count, sizeof(count));
    read(fd, &count, sizeof(count));
    return count;
}

int getMemberCount(int fd){
    int count = -1;
    lseek(fd, 0, SEEK_SET);
    read(fd, &count, sizeof(count));
    return count;
}

int getBookCount(int fd){
    int count = -1;
    lseek(fd, 0, SEEK_SET);
    read(fd, &count, sizeof(count));
    return count;
}

int getMaxBookID(int fd){
    int count = -1;
    lseek(fd, 0, SEEK_SET);
    read(fd, &count, sizeof(count));
    read(fd, &count, sizeof(count));
    return count;
}

book getBookByID(int id, int fd){
    book temp_book;
    temp_book.book_id = -1;
    int count = getMaxBookID(fd);
    if (id < 1 || id > count){
        return temp_book;
    }
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 2 * sizeof(int) + (id - 1) * sizeof(book);
    lock.l_len = sizeof(book);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd, 2 * sizeof(int) + (id - 1) * sizeof(book), SEEK_SET);
    read(fd, &temp_book, sizeof(temp_book));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return temp_book;
}


member getMemberByID(int id, int fd){
    member temp_member;
    temp_member.member_id = -1;
    int count = getMaxMemberID(fd);
    if (id < 1 || id > count){
        return temp_member;
    }
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 2 * sizeof(int) + (id - 1) * sizeof(member);
	lock.l_len = sizeof(member);
	lock.l_pid = getpid();
	fcntl(fd, F_SETLKW, &lock);
	lseek(fd, 2 * sizeof(int) + (id - 1) * sizeof(member), SEEK_SET);
    read(fd, &temp_member, sizeof(temp_member));
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
    return temp_member;
}

member searchMember(char username[], int fd){
    int count = getMaxMemberID(fd);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    member temp_member;
    temp_member.member_id = -1;
    lseek(fd, 2 * sizeof(int), SEEK_SET);

    for (int i = 0; i < count; i++){
        read(fd, &temp_member, sizeof(temp_member));
        if (temp_member.member_id != -1 && strcmp(temp_member.username, username) == 0){
            lock.l_type = F_UNLCK;
            fcntl(fd, F_SETLK, &lock);
            return temp_member;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    member nullmember;
    nullmember.member_id = -1;
    return nullmember;
}

int getMemberID(char username[], int fd){
    member temp = searchMember(username, fd);
    return temp.member_id;
}

int borrowBook(int member_id, int book_id, int sd, int fd, int fd2){
    member temp_member = getMemberByID(member_id, fd);
    struct flock lock2;
    if (temp_member.member_id == -1){
        write(sd, "Invalid member ID\n", sizeof("Invalid member ID\n"));
        return -1;
    }
    if (temp_member.num_books == 5){
        write(sd, "Cannot borrow more than 5 books\n", sizeof("Cannot borrow more than 5 books\n"));
        return -1;
    }
    book temp_book = getBookByID(book_id, fd2);
    lock2.l_type = F_WRLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 2 * sizeof(int) + (book_id - 1) * sizeof(book);
    lock2.l_len = sizeof(book);
    lock2.l_pid = getpid();
    fcntl(fd2, F_SETLKW, &lock2);
    if (temp_book.book_id == -1){
        write(sd, "Invalid book ID\n", sizeof("Invalid book ID\n"));
    }
    else if (temp_book.available == 0){
        write(sd, "Book not available\n", sizeof("Book not available\n"));
    }
    else{
        temp_book.available--;
        pthread_t thread;
        struct argument arg;
        arg.fd = fd2;
        arg.b = temp_book;
        arg.position = 2 * sizeof(int) + (book_id - 1) * sizeof(book);
        pthread_create(&thread, NULL, write__book_into_file, &arg);
        temp_member.num_books++;
        temp_member.books[temp_member.num_books - 1] = temp_book;
        temp_member.books[temp_member.num_books - 1].available = 1;
        temp_member.books[temp_member.num_books - 1].copies = 1;
        lseek(fd, 2 * sizeof(int) + (member_id - 1) * sizeof(member), SEEK_SET);
        write(fd, &temp_member, sizeof(temp_member));
        pthread_join(thread, NULL); 
        write(sd, "Book borrowed successfully\n", sizeof("Book borrowed successfully\n"));
    }
    lock2.l_type = F_UNLCK;
    fcntl(fd2, F_SETLK, &lock2);
    return 0;
}

int returnBook(int member_id, int book_id, int sd, int fd, int fd2){
    struct flock lock;
    member temp_member = getMemberByID(member_id, fd);
    if (temp_member.member_id == -1){
        write(sd, "Invalid member ID\n", sizeof("Invalid member ID\n"));
        return -1;
    }
    if (temp_member.num_books == 0){
        write(sd, "No books borrowed\n", sizeof("No books borrowed\n"));
        return -1;
    }
    int book_position = 0;
    for (book_position = 0; book_position < temp_member.num_books; book_position++){
        if (temp_member.books[book_position].book_id == book_id){
            break;
        }
    }
    if (book_position == temp_member.num_books){
        write(sd, "Book not borrowed\n", sizeof("Book not borrowed\n"));
        return -1;
    }
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 2 * sizeof(int) + (book_id - 1) * sizeof(book);
    lock.l_len = sizeof(book);
    lock.l_pid = getpid();
    fcntl(fd2, F_SETLKW, &lock);
    book temp_book = getBookByID(book_id, fd2);
    temp_book.available++;
    pthread_t thread;
    struct argument arg;
    arg.fd = fd2;
    arg.b = temp_book;

    arg.position = 2 * sizeof(int) + (book_id - 1) * sizeof(book);
    pthread_create(&thread, NULL, write__book_into_file, &arg);
    book nullbook;
    nullbook.book_id = -1;
    temp_member.books[book_position] = temp_member.books[temp_member.num_books - 1];
    temp_member.books[temp_member.num_books - 1] = nullbook;
    
    temp_member.num_books--;
    lseek(fd, 2 * sizeof(int) + (member_id - 1) * sizeof(member), SEEK_SET);
    write(fd, &temp_member, sizeof(temp_member));
    pthread_join(thread, NULL);
    lock.l_type = F_UNLCK;
    fcntl(fd2, F_SETLK, &lock);
    write(sd, "Book returned successfully\n", sizeof("Book returned successfully\n"));
    return 0;
}
int searchBook(char title[], book books[], int fd){
    int count = getMaxBookID(fd);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    book temp_book;
    temp_book.book_id = -1;
    int matching = 0;
    lseek(fd, 2 * sizeof(int), SEEK_SET);
    for (int i = 0; i < count; i++){
        read(fd, &temp_book, sizeof(temp_book));
        if (temp_book.book_id != -1 && strcmp(temp_book.title, title) == 0){
            books[matching] = temp_book;
            matching++;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return matching;
}

int searchBookByAuthor(char author[], book books[], int fd){
    int count = getMaxBookID(fd);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    book temp_book;
    temp_book.book_id = -1;
    int matching = 0;
    lseek(fd, 2 * sizeof(int), SEEK_SET);
    for (int i = 0; i < count; i++){
        read(fd, &temp_book, sizeof(temp_book));
        if (temp_book.book_id != -1 && strcmp(temp_book.author, author) == 0){
            books[matching] = temp_book;
            matching++;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return matching;
}

int getAllBooks(book books[], int fd){
    int count = getBookCount(fd);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd, 2 * sizeof(int), SEEK_SET);
    for (int i = 0; i < count; i++){
        read(fd, &books[i], sizeof(books[i]));
        if (books[i].book_id == -1){
            i--;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return count;
}

int getAllMembers(member members[], int fd){
    int count = getMemberCount(fd);
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    fcntl(fd, F_SETLKW, &lock);
    lseek(fd, 2 * sizeof(int), SEEK_SET);
    for (int i = 0; i < count; i++){
        read(fd, &members[i], sizeof(members[i]));
        if (members[i].member_id == -1){
            i--;
        }
    }
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    return count;
}

void addBook(book b, int sd, int fd){
    int count = getBookCount(fd);
    int maxcount = getMaxBookID(fd);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 2 * sizeof(int);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    b.book_id = maxcount + 1;
    count++;
    maxcount++;
    lseek(fd, 0, SEEK_SET);
    write(fd, &count, sizeof(count));
    write(fd, &maxcount, sizeof(maxcount));
    lseek(fd, 2*sizeof(int) + (maxcount - 1) * sizeof(book), SEEK_SET);
    write(fd, &b, sizeof(b));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    write(sd, "Book added successfully\n", sizeof("Book added successfully\n"));
}

void addMember(member m, int sd, int fd){
    member temp_member = searchMember(m.username, fd);
    if (temp_member.member_id != -1){
        write(sd, "Username already exists\n", sizeof("Username already exists\n"));
        return;
    }
    int count = getMemberCount(fd);
    int maxcount = getMaxMemberID(fd);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 2 * sizeof(int);
    lock.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock);
    m.member_id = maxcount + 1;
    count++;
    maxcount++;
    lseek(fd, 0, SEEK_SET);
    write(fd, &count, sizeof(count));
    write(fd, &maxcount, sizeof(maxcount));
    lseek(fd, 2*sizeof(int) + (maxcount - 1) * sizeof(member), SEEK_SET);
    write(fd, &m, sizeof(m));
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    write(sd, "Member added successfully\n", sizeof("Member added successfully\n"));
}

void removeBook(int id, int sd, int fd){
    book delbook = getBookByID(id, fd);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 2 * sizeof(int);
    lock.l_pid = getpid();  
    fcntl(fd, F_SETLKW, &lock);
    if (delbook.book_id == -1){
        write(sd, "Book does not exist\n", sizeof("Book does not exist\n"));
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        return;
    }
    if (delbook.available != delbook.copies){
        write(sd, "Book not available for deletion\n", sizeof("Book not available for deletion\n"));
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        return;
    }
    int count = getBookCount(fd);
    struct flock lock2;
    lock2.l_type = F_WRLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 2 * sizeof(int) + (id - 1) * sizeof(book);
    lock2.l_len = sizeof(book);
    lock2.l_pid = getpid();
    fcntl(fd, F_SETLKW, &lock2);
    book nullbook;
    nullbook.book_id = -1;
    count--;
    lseek(fd, 0, SEEK_SET);
    write(fd, &count, sizeof(count));
    lseek(fd, 2 * sizeof(int) + (id - 1) * sizeof(book), SEEK_SET);
    write(fd, &nullbook, sizeof(nullbook));
    lock.l_type = F_UNLCK;
    lock2.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock2);
    fcntl(fd, F_SETLK, &lock);
    write(sd, "Book removed successfully\n", sizeof("Book removed successfully\n"));
}

void removeMember(int id, int sd, int fd, int fd2){
    int count = getMemberCount(fd);
    member delmember = getMemberByID(id, fd);
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 2 * sizeof(int);
    fcntl(fd, F_SETLKW, &lock);
    if (delmember.member_id == -1){
        write(sd, "Member does not exist\n", sizeof("Member does not exist\n"));
        lock.l_type = F_UNLCK;
        fcntl(fd, F_SETLK, &lock);
        return;
    }
    // If member is logged in, then return;
    struct flock lock2;
    lock2.l_type = F_WRLCK;
    lock2.l_whence = SEEK_SET;
    lock2.l_start = 2 * sizeof(int) + (id - 1) * sizeof(member);
    lock2.l_len = sizeof(member);
    lock2.l_pid = getpid();
    if (fcntl(fd, F_SETLK, &lock2) != 0) {
        write(sd, "Member is logged in! Can't proceed with delete!\n", sizeof("Member is logged in! Can't proceed with delete!\n"));
        return;
    }
    for (int i = 0; i < delmember.num_books; i++){
        returnBook(id, delmember.books[i].book_id, sd, fd, fd2);
    }
    member nullmember;
    nullmember.member_id = -1;
    count--;
    lseek(fd, 0, SEEK_SET);
    write(fd, &count, sizeof(count));
    lseek(fd, 2 * sizeof(int) + (id - 1) * sizeof(member), SEEK_SET);
    write(fd, &nullmember, sizeof(nullmember));
    lock.l_type = F_UNLCK;
    lock2.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock2);
    fcntl(fd, F_SETLK, &lock);
    write(sd, "Member removed successfully\n", sizeof("Member removed successfully\n"));
}

