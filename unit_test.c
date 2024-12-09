#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <string.h>
#include "server_func.h"
#include "member.h"
#include "book.h"

void test_getMaxMemberID() {
    int mem_fd = open("member.dat", O_RDWR);
    CU_ASSERT(getMaxMemberID(mem_fd) == 10);
}

void test_getMemberCount(void){
    int fd = open("./member.dat", O_RDWR);
    CU_ASSERT(getMemberCount(fd) == 10);
}

void test_getBookCount(void){
    int fd = open("./book.dat", O_RDWR);
    CU_ASSERT(getBookCount(fd) == 10);
}

void test_getMaxBookID(void){
    int fd = open("./book.dat", O_RDWR);
    CU_ASSERT(getMaxBookID(fd) == 10);
}

int compare_book(book book1, book book2){
    return (book1.book_id == book2.book_id) && (strcmp(book1.title, book2.title) == 0) && (strcmp(book1.author, book2.author) == 0) && (book1.copies == book2.copies) && (book1.available == book2.available);
}

void test_getBookByID(void){
    int fd = open("./book.dat", O_RDWR);

    book book1 = getBookByID(1, fd);
    book book2;

    lseek(fd, 2 * sizeof(int), SEEK_SET);
    read(fd, &book2, sizeof(book2));

    CU_ASSERT(compare_book(book1, book2) == 1);
    CU_ASSERT(getBookByID(12, fd).book_id == -1);
}

int compare_member(member member1, member member2){
    if  (!((member1.member_id == member2.member_id) && (strcmp(member1.username, member2.username) == 0) && (strcmp(member1.password, member2.password) == 0) && (member1.num_books == member2.num_books)))
        return 0;
    
    for(int i = 0; i < member1.num_books; i++){
        if(compare_book(member1.books[i], member2.books[i]) == 0)
            return 0;
    return 1;
    }
}

void test_getMemberByID(void){
    int fd = open("./member.dat", O_RDWR);
    member member1 = getMemberByID(1, fd);
    member member2;

    lseek(fd, 2 * sizeof(int), SEEK_SET);
    read(fd, &member2, sizeof(member2));

    CU_ASSERT(compare_member(member1, member2) == 1);
    CU_ASSERT(getMemberByID(12, fd).member_id == -1);


}

void test_searchMember_found() {
    int mem_fd = open("member.dat", O_RDWR);
    member result = searchMember("user2", mem_fd);

    CU_ASSERT(result.member_id == 2);
    CU_ASSERT(strcmp(result.username, "user2") == 0);
    CU_ASSERT(strcmp(result.password, "pass2") == 0);
    CU_ASSERT(result.num_books == 0);
}

void test_searchMember_not_found() {
    int mem_fd = open("member.dat", O_RDWR);
    member result = searchMember("user11", mem_fd);

    CU_ASSERT(result.member_id == -1);
}

void test_getMemberID_found() {
    int mem_fd = open("member.dat", O_RDWR);
    member result = searchMember("user5", mem_fd);

    CU_ASSERT(result.member_id == 5);
}

void test_getMemberID_not_found() {
    int mem_fd = open("member.dat", O_RDWR);
    member result = searchMember("user11", mem_fd);

    CU_ASSERT(result.member_id == -1);
}

void test_borrowBook_invalid_member() {
    int mem_fd = open("member.dat", O_RDWR);
    int book_fd = open("book.dat", O_RDWR);
    int result = borrowBook(11, 2, -1, mem_fd, book_fd);

    CU_ASSERT(result == -1);
}

void test_returnBook_invalid_member() {
    int mem_fd = open("member.dat", O_RDWR);
    int book_fd = open("book.dat", O_RDWR);
    int result = returnBook(11, 2, -1, mem_fd, book_fd);

    CU_ASSERT(result == -1);
}

void test_return_without_borrow() {
    int mem_fd = open("member.dat", O_RDWR);
    int book_fd = open("book.dat", O_RDWR);
    int result = returnBook(2, 2, -1, mem_fd, book_fd);

    CU_ASSERT(result == -1);
}

void test_searchBook_found() {
    book books[50];
    int book_fd = open("book.dat", O_RDWR);
    int numMatches = searchBook("Book5", books, book_fd);

    CU_ASSERT(numMatches == 1);
    CU_ASSERT(books[0].book_id == 5);
    CU_ASSERT(strcmp(books[0].title, "Book5") == 0);
    CU_ASSERT(strcmp(books[0].author, "Author5") == 0);
    CU_ASSERT(books[0].copies == 5);
    CU_ASSERT(books[0].available == 5);
}

void test_searchBook_not_found() {
    book books[50];
    int book_fd = open("book.dat", O_RDWR);
    int numMatches = searchBook("Harry Potter and the Sorcerer's Stone", books, book_fd);

    CU_ASSERT(numMatches == 0);
}

void test_searchBookByAuthor_found() {
    book books[50];
    int book_fd = open("book.dat", O_RDWR);
    int numMatches = searchBookByAuthor("Author5", books, book_fd);

    CU_ASSERT(numMatches == 1);
    CU_ASSERT(books[0].book_id == 5);
    CU_ASSERT(strcmp(books[0].author, "Author5") == 0);
    close(book_fd);
}

void test_searchBookByAuthor_not_found() {
    book books[50];
    int book_fd = open("book.dat", O_RDWR);
    int numMatches = searchBookByAuthor("Unknown Author", books, book_fd);

    CU_ASSERT(numMatches == 0);
    close(book_fd);
}

void test_getAllBooks() {
    book books[50];
    int book_fd = open("book.dat", O_RDWR);
    int count = getAllBooks(books, book_fd);

    CU_ASSERT(count == 10);  // Verifying that the count matches initialized data

    for (int i = 0; i < count; i++) {
        book expected_book;
        expected_book.book_id = i + 1;
        sprintf(expected_book.title, "Book%d", i + 1);
        sprintf(expected_book.author, "Author%d", i + 1);
        expected_book.copies = 5;
        expected_book.available = 5;

        CU_ASSERT(compare_book(books[i], expected_book) == 1);  // Comparing each book
    }
    close(book_fd);
}

void test_addBook() {
    int book_fd = open("book.dat", O_RDWR);
    int sd = -1;  
    book newBook = {0, "New Book", "New Author", 10, 10};

    addBook(newBook, sd, book_fd);

    book books[50];
    int count = getAllBooks(books, book_fd);
    CU_ASSERT(count > 0);
    CU_ASSERT(books[count - 1].book_id != -1);
    CU_ASSERT(strcmp(books[count - 1].title, "New Book") == 0);
    close(book_fd);
}

void test_removeBook() {
    int book_fd = open("book.dat", O_RDWR);
    int sd = -1;  
    removeBook(1, sd, book_fd);

    book books[50];
    int count = getAllBooks(books, book_fd);
    for (int i = 0; i < count; i++) {
        CU_ASSERT(books[i].book_id != 1);
    }
    close(book_fd);
}

void test_borrowBook_success() {
    int book_fd = open("book.dat", O_RDWR);
    int member_fd = open("member.dat", O_RDWR);

    borrowBook(1, 1, -1, member_fd, book_fd);

    member members[50];
    getAllMembers(members, member_fd);
    CU_ASSERT(members[0].num_books == 1);
    CU_ASSERT(members[0].books[0].book_id == 1);

    book books[50];
    getAllBooks(books, book_fd);
    CU_ASSERT(books[0].available == 4);

    close(member_fd);
    close(book_fd);
}

void test_returnBook_success() {
    int book_fd = open("book.dat", O_RDWR);
    int member_fd = open("member.dat", O_RDWR);

    returnBook(1, 1, -1, member_fd, book_fd);

    member members[50];
    getAllMembers(members, member_fd);
    CU_ASSERT(members[0].num_books == 0);

    book books[50];
    getAllBooks(books, book_fd);
    CU_ASSERT(books[0].available == 5);

    close(member_fd);
    close(book_fd);
}


void test_getAllMembers() {
    member members[50];
    int member_fd = open("member.dat", O_RDWR);
    int count = getAllMembers(members, member_fd);

    CU_ASSERT(count == 10); 
    for (int i = 0; i < count; i++) {
        char expected_username[20], expected_password[20];
        sprintf(expected_username, "user%d", i + 1);
        sprintf(expected_password, "pass%d", i + 1);

        CU_ASSERT(members[i].member_id == i + 1);
        CU_ASSERT(strcmp(members[i].username, expected_username) == 0);
        CU_ASSERT(strcmp(members[i].password, expected_password) == 0);
        CU_ASSERT(members[i].num_books == 0);
    }
    close(member_fd);
}


void test_addMember() {
    int member_fd = open("member.dat", O_RDWR);
    int sd = -1;  

    member newMember = {0, "newuser", "newpass", 0, {}};
    addMember(newMember, sd, member_fd);

    member members[50];
    int count = getAllMembers(members, member_fd);

    CU_ASSERT(count > 0);
    CU_ASSERT(members[count - 1].member_id != -1);  // Ensure new member is added
    CU_ASSERT(strcmp(members[count - 1].username, "newuser") == 0);
    CU_ASSERT(strcmp(members[count - 1].password, "newpass") == 0);

    close(member_fd);
}

void test_removeMember() {
    int member_fd = open("member.dat", O_RDWR);
    int book_fd = open("book.dat", O_RDWR);
    int sd = -1; 
    removeMember(1, sd, member_fd, book_fd);

    member members[50];
    int count = getAllMembers(members, member_fd);

    for (int i = 0; i < count; i++) {
        CU_ASSERT(members[i].member_id != 1);  // Ensure member with ID 1 is removed
    }

    close(member_fd);
    close(book_fd);
}



int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("AddTestSuite", 0, 0);
    CU_add_test(suite, "test of getMaxMemberID()", test_getMaxMemberID);
    CU_add_test(suite, "test of getMemberCount()", test_getMemberCount);
    CU_add_test(suite, "test of getBookCount()", test_getBookCount);
    CU_add_test(suite, "test of getMaxBookID()", test_getMaxBookID);
    CU_add_test(suite, "test of getBookByID()", test_getBookByID);
    CU_add_test(suite, "test of getMemberByID()", test_getMemberByID);
    CU_add_test(suite, "test of searchMember() if found", test_searchMember_found);
    CU_add_test(suite, "test of searchMember() if not found", test_searchMember_not_found);
    CU_add_test(suite, "test of getMemberID() if found", test_getMemberID_found);
    CU_add_test(suite, "test of getMemberID() if not found", test_getMemberID_not_found);
    CU_add_test(suite, "test of getAllBooks()", test_getAllBooks);
    CU_add_test(suite, "test of getAllMembers()", test_getAllMembers);
    CU_add_test(suite, "test of borrowBook() if member not found", test_borrowBook_invalid_member);
    CU_add_test(suite, "test borrowBook() success scenario", test_borrowBook_success);
    CU_add_test(suite, "test returnBook() success scenario", test_returnBook_success);
    CU_add_test(suite, "test of returnBook() if member not found", test_returnBook_invalid_member);
    CU_add_test(suite, "test of returnBook() when the book is not borrowed", test_return_without_borrow);
    CU_add_test(suite, "test of searchBook() when the book is found", test_searchBook_found);
    CU_add_test(suite, "test of searchBook() when the book is found", test_searchBook_not_found);
    CU_add_test(suite, "test of searchBookByAuthor() when found", test_searchBookByAuthor_found);
    CU_add_test(suite, "test of searchBookByAuthor() when not found", test_searchBookByAuthor_not_found);
    CU_add_test(suite, "test of addBook()", test_addBook);
    CU_add_test(suite, "test of removeBook()", test_removeBook);
    CU_add_test(suite, "test of addMember()", test_addMember);
    CU_add_test(suite, "test of removeMember()", test_removeMember);

    CU_basic_run_tests();
    CU_cleanup_registry();
}
