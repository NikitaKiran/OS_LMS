#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "server_func.h"
#include "book.h"
#include "member.h"

void test_getMaxMemberID(void){
    int fd = open("./member.dat", O_RDWR);
    CU_ASSERT(getMaxMemberID(fd) == 10);
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

int main()
{
    CU_initialize_registry();

    CU_pSuite suite = CU_add_suite("AddTestSuite", 0, 0);
    CU_add_test(suite, "test of getMaxMemberID()", test_getMaxMemberID);
    CU_add_test(suite, "test of getMemberCount()", test_getMemberCount);
    CU_add_test(suite, "test of getBookCount()", test_getBookCount);
    CU_add_test(suite, "test of getMaxBookID()", test_getMaxBookID);
    CU_add_test(suite, "test of getBookByID()", test_getBookByID);
    CU_add_test(suite, "test of getMemberByID()", test_getMemberByID);


    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
