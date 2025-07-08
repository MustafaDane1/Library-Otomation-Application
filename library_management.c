#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Yazar yapýsý tanýmý
typedef struct Author {
    int authorID;
    char name[50];
    char surname[50];
    struct Author* next;
} Author;

// Öðrenci yapýsý tanýmý
typedef struct Student {
    int ogrNo;
    char name[50];
    char surname[50];
    int puan;
    struct Student* next;
    struct Student* prev;
} Student;

// Kitap kopyasý yapýsý tanýmý
typedef struct BookCopy {
    char tag[25];
    int isAvailable;
    int studentID;
    struct BookCopy* next;
} BookCopy;

// Kitap struct tanýmý
typedef struct Book {
    char isbn[20];
    char title[100];
    BookCopy* copies;
    struct Book* next;
} Book;

//Kitap-Yazar iliþkisinin yapýsý
typedef struct BookAuthorRelation {
    char isbn[20];
    int authorID;
} BookAuthorRelation;

// Ýþlem kaydý struct
typedef struct Transaction {
    int transactionID;
    char book_isbn[20];
    char copy_tag[25];
    int studentID;
    char transactionType[20];
    char date[11];
    int penaltyPoints;
    struct Transaction* next;
} Transaction;

// Kütüphane struct
typedef struct Library {
    Author* authorList;
    Student* studentList;
    Book* bookList;
    Transaction* transactionList;
    BookAuthorRelation* bookAuthorRelationsArray;
    int bookAuthorCount;
    int bookAuthorCapacity;
} Library;

// Generic Function Prototypes
void generic_load_list(const char* filename, void** list_head_ptr, void (*parse_and_add_func)(char* line, void** list_head_ptr, Library* library), Library* library);
void generic_save_list(const char* filename, void* list_head, const char* header_line, void (*write_item_func)(FILE* file, void* item_data), void* (*get_next_func)(void* current_node));
void* generic_find_by_int_id(void* list_head, void* (*get_next_func)(void* current_node), int (*get_id_func)(void* item_data), int search_id);
void* generic_find_by_string_key(void* list_head, void* (*get_next_func)(void* current_node), const char* (*get_key_func)(void* item_data), const char* search_key);

// Callback function prototypes
void parse_and_add_author(char* line, void** list_head_ptr, Library* library);
void write_author_csv(FILE* file, void* item_data);
void* get_next_author_node(void* current_node);
int get_author_id_for_find(void* item_data);
void parse_and_add_student(char* line, void** list_head_ptr, Library* library);
void write_student_csv(FILE* file, void* item_data);
void* get_next_student_node(void* current_node);
int get_student_id_for_find(void* item_data);
void parse_and_add_book(char* line, void** list_head_ptr, Library* library);
void write_book_csv(FILE* file, void* item_data);
void* get_next_book_node(void* current_node);
const char* get_book_isbn_for_find(void* item_data);
void parse_and_add_book_copy(char* line, void** list_head_ptr_unused, Library* library);
const char* get_book_copy_tag_for_find(void* item_data);
void* get_next_book_copy_node(void* current_node);
void parse_and_add_transaction(char* line, void** list_head_ptr, Library* library);
void write_transaction_csv(FILE* file, void* item_data);
void* get_next_transaction_node(void* current_node);

// Yazar iþlemleri
void add_author(Library* library);
void delete_author(Library* library);
void update_author(Library* library);
void list_authors(Library* library);
void display_author_info(Library* library);
Author* find_author(Library* library, int authorID);
int get_max_author_id(Library* library);
void load_authors(Library* library);
void save_authors(Library* library);

// Öðrenci iþlemleri
void add_student(Library* library);
void delete_student(Library* library);
void update_student(Library* library);
void list_students(Library* library);
void display_student_info(Library* library);
void list_students_with_unreturned_books(Library* library);
Student* find_student(Library* library, int ogrNo);
void load_students(Library* library);
void save_students(Library* library);

// Kitap iþlemleri
void add_book(Library* library);
void delete_book(Library* library);
void update_book(Library* library);
void list_books(Library* library);
void display_book_info(Library* library);
void list_available_books(Library* library);
void list_overdue_books(Library* library);
Book* find_book_by_isbn(Library* library, const char* isbn);
BookCopy* find_book_copy(Book* book, const char* tag);
void load_books(Library* library);
void save_books(Library* library);
void load_book_copies(Library* library);
void save_book_copies(Library* library);

// Kitap-Yazar iliþisi
void load_book_author_relations(Library* library);
void save_book_author_relations(Library* library);
void assign_author_to_book(Library* library);
void update_book_authors(Library* library);
void list_books_by_author(Library* library);
void remove_book_author_relations_for_book(Library* library, const char* isbn);
void update_author_relations_on_author_delete(Library* library, int authorID);
void ensure_book_author_capacity(Library* library);

// Kitap ödünç/iade
void borrow_book(Library* library);
void return_book(Library* library);
void list_unreturned_books(Library* library);
void list_penalized_students(Library* library);
void log_transaction(Library* library, const char* book_isbn, const char* copy_tag, int studentID, const char* transactionType, int penaltyPoints);
char* get_current_date();
int calculate_days_difference(const char* date1, const char* date2);
int get_max_transaction_id(Library* library);
void load_transactions(Library* library);
void save_transactions(Library* library);

// Menü
void menu_loop(Library* library);
int get_menu_choice();

int main() {
    Library library;
    Author* currentAuthor, *tempAuthor;
    Student* currentStudent, *tempStudent;
    Book* currentBook, *tempBook;
    BookCopy* currentCopy, *tempCopy;
    Transaction* currentTransaction, *tempTransaction;

    library.authorList = NULL;
    library.studentList = NULL;
    library.bookList = NULL;
    library.transactionList = NULL;
    library.bookAuthorRelationsArray = NULL;
    library.bookAuthorCount = 0;
    library.bookAuthorCapacity = 10;

    library.bookAuthorRelationsArray = (BookAuthorRelation*)malloc(library.bookAuthorCapacity * sizeof(BookAuthorRelation));
    if (library.bookAuthorRelationsArray == NULL && library.bookAuthorCapacity > 0) {
        perror("Failed to allocate initial bookAuthorRelationsArray");
        return 1;}
    printf("KUTUPHANE OTOMASYON SISTEMINE HOSGELDINIZ!\n");
    load_authors(&library);
    load_students(&library);
    load_books(&library);
    load_book_copies(&library);
    load_book_author_relations(&library);
    load_transactions(&library);
    menu_loop(&library);
    currentAuthor = library.authorList;
    while (currentAuthor != NULL) {
        tempAuthor = currentAuthor;
        currentAuthor = currentAuthor->next;
        free(tempAuthor);
    }
    currentStudent = library.studentList;
    while (currentStudent != NULL) {
        tempStudent = currentStudent;
        currentStudent = currentStudent->next;
        free(tempStudent); }
    currentBook = library.bookList;
    while (currentBook != NULL) {
        tempBook = currentBook;
        currentCopy = tempBook->copies;
        while (currentCopy != NULL) {
            tempCopy = currentCopy;
            currentCopy = currentCopy->next;
            free(tempCopy);
    }
        currentBook = currentBook->next;
        free(tempBook);
    }
    if (library.bookAuthorRelationsArray != NULL) {
        free(library.bookAuthorRelationsArray);
    }
    currentTransaction = library.transactionList;
    while (currentTransaction != NULL) {
        tempTransaction = currentTransaction;
        currentTransaction = currentTransaction->next;
        free(tempTransaction);
    }

    printf("Program sonlandirildi. Iyi gunler!\n");
    return 0;
     }

// Generic Function Implementations
void generic_load_list(const char* filename, void** list_head_ptr, void (*parse_and_add_func)(char* line, void** list_head_ptr, Library* library), Library* library){
    FILE* file = fopen(filename, "r");
    char line_buffer[256];
    if (file == NULL) {
        return;
    }
    if (fgets(line_buffer, sizeof(line_buffer), file) == NULL && !feof(file)) {
        fclose(file);
        return;
    }
    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        line_buffer[strcspn(line_buffer, "\r\n")] = 0;
        if (strlen(line_buffer) > 0) {
            parse_and_add_func(line_buffer, list_head_ptr, library);
        }
    }
    fclose(file);
}

void generic_save_list(const char* filename, void* list_head, const char* header_line, void (*write_item_func)(FILE* file, void* item_data), void* (*get_next_func)(void* current_node)){
    FILE* file = fopen(filename, "w");
    void* current = list_head;
    if (file == NULL) {
        printf("%s dosyasi acilamadi!\n", filename);
        return;
    }
    fprintf(file, "%s\n", header_line);
    while (current != NULL) {
        write_item_func(file, current);
        current = get_next_func(current);
    }
    fclose(file);
}

void* generic_find_by_int_id(void* list_head, void* (*get_next_func)(void* current_node), int (*get_id_func)(void* item_data), int search_id){
    void* current = list_head;
    while (current != NULL) {
        if (get_id_func(current) == search_id) {
            return current;
        }
        current = get_next_func(current);
    }
    return NULL;
}

void* generic_find_by_string_key(void* list_head, void* (*get_next_func)(void* current_node), const char* (*get_key_func)(void* item_data), const char* search_key){
    void* current = list_head;
    const char* current_key;
    while (current != NULL) {
        current_key = get_key_func(current);
        if (current_key != NULL && strcmp(current_key, search_key) == 0) {
            return current;
        }
        current = get_next_func(current);
    }
    return NULL;
}

//Callback Function Implementations
void parse_and_add_author(char* line, void** list_head_ptr, Library* library) {
    Author** author_list_hptr = (Author**)list_head_ptr;
    Author* newAuthor = (Author*)malloc(sizeof(Author));
    char* token;
    Author* current_insert;
    Author* prev_insert;
    if (newAuthor == NULL) { printf("Bellek yetersiz! (Author)\n"); return; }
    token = strtok(line, ","); newAuthor->authorID = token ? atoi(token) : 0;
    token = strtok(NULL, ","); strncpy(newAuthor->name, token ? token : "", sizeof(newAuthor->name) - 1); newAuthor->name[sizeof(newAuthor->name) - 1] = '\0';
    token = strtok(NULL, ","); strncpy(newAuthor->surname, token ? token : "", sizeof(newAuthor->surname) - 1); newAuthor->surname[sizeof(newAuthor->surname) - 1] = '\0';
    newAuthor->next = NULL;

    if (*author_list_hptr == NULL || (*author_list_hptr)->authorID >= newAuthor->authorID) {
        newAuthor->next = *author_list_hptr;
        *author_list_hptr = newAuthor;
    } else {
        current_insert = *author_list_hptr;
        prev_insert = NULL;
        while (current_insert != NULL && current_insert->authorID < newAuthor->authorID) {
            prev_insert = current_insert;
            current_insert = current_insert->next;
        }
        prev_insert->next = newAuthor;
        newAuthor->next = current_insert;
    }
}
void write_author_csv(FILE* file, void* item_data) {
    Author* author = (Author*)item_data;
    fprintf(file, "%d,%s,%s\n", author->authorID, author->name, author->surname);
}
void* get_next_author_node(void* current_node) {
    return current_node ? ((Author*)current_node)->next : NULL;
}
int get_author_id_for_find(void* item_data) {
    return ((Author*)item_data)->authorID;
}

void parse_and_add_student(char* line, void** list_head_ptr, Library* library) {
    Student** student_list_hptr = (Student**)list_head_ptr;
    Student* newStudent = (Student*)malloc(sizeof(Student));
    char* token;
    Student* current_insert;
    Student* prev_insert_unused; // prev_insert is not directly used for singly linked list style insertion logic here, but kept if logic was to be more complex.
                                // For doubly linked list, prev_insert (or current_insert->prev) logic will be different
    if (newStudent == NULL) {
        printf("Bellek yetersiz! (Student)\n");
        return;
    }
    token = strtok(line, ",");
    newStudent->ogrNo = token ? atoi(token) : 0;
    token = strtok(NULL, ",");
    strncpy(newStudent->name, token ? token : "", sizeof(newStudent->name) - 1);
    newStudent->name[sizeof(newStudent->name) - 1] = '\0';
    token = strtok(NULL, ",");
    strncpy(newStudent->surname, token ? token : "", sizeof(newStudent->surname) - 1);
    newStudent->surname[sizeof(newStudent->surname) - 1] = '\0';
    token = strtok(NULL, ",");
    newStudent->puan = token ? atoi(token) : 0;
    newStudent->next = NULL;
    newStudent->prev = NULL;

    if (*student_list_hptr == NULL || (*student_list_hptr)->ogrNo >= newStudent->ogrNo) {
        newStudent->next = *student_list_hptr;
        if (*student_list_hptr != NULL) {
            (*student_list_hptr)->prev = newStudent;
        }
        *student_list_hptr = newStudent;
    } else {
        current_insert = *student_list_hptr;
        while (current_insert->next != NULL && current_insert->next->ogrNo < newStudent->ogrNo) {
            current_insert = current_insert->next;
        }
        newStudent->next = current_insert->next;
        if (current_insert->next != NULL) {
            current_insert->next->prev = newStudent;
        }
        current_insert->next = newStudent;
        newStudent->prev = current_insert;
    }
}
void write_student_csv(FILE* file, void* item_data) {
    Student* student = (Student*)item_data;
    fprintf(file, "%d,%s,%s,%d\n", student->ogrNo, student->name, student->surname, student->puan);
}
void* get_next_student_node(void* current_node) {
    return current_node ? ((Student*)current_node)->next : NULL;
}
int get_student_id_for_find(void* item_data) {
    return ((Student*)item_data)->ogrNo;
}

void parse_and_add_book(char* line, void** list_head_ptr, Library* library) {
    Book** book_list_hptr = (Book**)list_head_ptr;
    Book* newBook = (Book*)malloc(sizeof(Book));
    char* token;
    Book* current_insert, *prev_insert;
    if (newBook == NULL) { printf("Bellek yetersiz! (Book)\n"); return; }
    newBook->copies = NULL;
    token = strtok(line, ","); strncpy(newBook->isbn, token ? token : "", sizeof(newBook->isbn) - 1); newBook->isbn[sizeof(newBook->isbn) - 1] = '\0';
    token = strtok(NULL, ","); strncpy(newBook->title, token ? token : "", sizeof(newBook->title) - 1); newBook->title[sizeof(newBook->title) - 1] = '\0';
    current_insert = *book_list_hptr; prev_insert = NULL;
    while (current_insert != NULL && (strcmp(current_insert->title, newBook->title) < 0 || (strcmp(current_insert->title, newBook->title) == 0 && strcmp(current_insert->isbn, newBook->isbn) < 0))) {
        prev_insert = current_insert;
        current_insert = current_insert->next;
    }
    if (prev_insert == NULL) {
        newBook->next = *book_list_hptr;
        *book_list_hptr = newBook;
    } else {
        prev_insert->next = newBook;
        newBook->next = current_insert;
           }
           }
void write_book_csv(FILE* file, void* item_data) {
    Book* book = (Book*)item_data;
    fprintf(file, "%s,%s\n", book->isbn, book->title);
    }
void* get_next_book_node(void* current_node) {
    return current_node ? ((Book*)current_node)->next : NULL;
    }
const char* get_book_isbn_for_find(void* item_data) {
    return ((Book*)item_data)->isbn;
}


void parse_and_add_book_copy(char* line, void** list_head_ptr_unused, Library* library) {
    char line_copy[256];
    char* token_isbn, *token_tag, *token_is_available_str, *token_student_id_str;
    int isAvailable, studentID;
    Book* book;
    BookCopy* newCopy, *current_copy_iter;
    strncpy(line_copy, line, sizeof(line_copy)-1);
    line_copy[sizeof(line_copy)-1] = '\0';
    token_isbn = strtok(line_copy, ",");
    if (!token_isbn) return;
    token_tag = strtok(NULL, ",");
    if (!token_tag) return;
    token_is_available_str = strtok(NULL, ",");
    if (!token_is_available_str) return;
    isAvailable = atoi(token_is_available_str);
    token_student_id_str = strtok(NULL, ",");
    if (!token_student_id_str) return;
    studentID = atoi(token_student_id_str);
    book = find_book_by_isbn(library, token_isbn);
    if (book == NULL) return;
    newCopy = (BookCopy*)malloc(sizeof(BookCopy));
    if (newCopy == NULL) {
        printf("Bellek yetersiz! (BookCopy)\n");
        return; }
    strncpy(newCopy->tag, token_tag, sizeof(newCopy->tag) - 1);
    newCopy->tag[sizeof(newCopy->tag) - 1] = '\0';
    newCopy->isAvailable = isAvailable;
    newCopy->studentID = studentID;
    newCopy->next = NULL;
    if (book->copies == NULL) {
        book->copies = newCopy;
    } else {
        current_copy_iter = book->copies;
        while (current_copy_iter->next != NULL) {
            current_copy_iter = current_copy_iter->next;
        }
        current_copy_iter->next = newCopy;
        }
        }
const char* get_book_copy_tag_for_find(void* item_data) {
    return ((BookCopy*)item_data)->tag;
}
void* get_next_book_copy_node(void* current_node) {
    return current_node ? ((BookCopy*)current_node)->next : NULL;
}
void parse_and_add_transaction(char* line, void** list_head_ptr, Library* library) {
    Transaction** trans_list_hptr = (Transaction**)list_head_ptr;
    Transaction* newTrans = (Transaction*)malloc(sizeof(Transaction));
    char* token;
    if (newTrans == NULL) { printf("Bellek yetersiz! (Transaction)\n"); return; }
    token = strtok(line, ","); newTrans->transactionID = token ? atoi(token) : 0;
    token = strtok(NULL, ","); strncpy(newTrans->book_isbn, token ? token : "", sizeof(newTrans->book_isbn) - 1); newTrans->book_isbn[sizeof(newTrans->book_isbn)-1] = '\0';
    token = strtok(NULL, ","); strncpy(newTrans->copy_tag, token ? token : "", sizeof(newTrans->copy_tag) - 1); newTrans->copy_tag[sizeof(newTrans->copy_tag)-1] = '\0';
    token = strtok(NULL, ","); newTrans->studentID = token ? atoi(token) : 0;
    token = strtok(NULL, ","); strncpy(newTrans->transactionType, token ? token : "", sizeof(newTrans->transactionType) - 1); newTrans->transactionType[sizeof(newTrans->transactionType)-1] = '\0';
    token = strtok(NULL, ","); strncpy(newTrans->date, token ? token : "", sizeof(newTrans->date) - 1); newTrans->date[sizeof(newTrans->date)-1] = '\0';
    token = strtok(NULL, ","); newTrans->penaltyPoints = token ? atoi(token) : 0;
    newTrans->next = *trans_list_hptr;
    *trans_list_hptr = newTrans;
}
void write_transaction_csv(FILE* file, void* item_data) {
    Transaction* trans = (Transaction*)item_data;
    fprintf(file, "%d,%s,%s,%d,%s,%s,%d\n", trans->transactionID, trans->book_isbn, trans->copy_tag, trans->studentID, trans->transactionType, trans->date, trans->penaltyPoints);
}
void* get_next_transaction_node(void* current_node) {
    return current_node ? ((Transaction*)current_node)->next : NULL;
}

//Menu
void menu_loop(Library* library) {
    int choice;
    int subChoice;
    int borrowChoice;
    while (1) {
        choice = get_menu_choice();
        switch (choice) {
            case 1:
                printf("\n=== OGRENCI ISLEMLERI ===\n");
                printf("1. Ogrenci Ekle\n2. Ogrenci Sil\n3. Ogrenci Guncelle\n");
                printf("4. Ogrenci Bilgisi Goruntuleme\n5. Kitap Teslim Etmemis Ogrencileri Listele\n");
                printf("6. Cezali Ogrencileri Listele\n7. Tum Ogrencileri Listele\n");
                printf("8. Kitap Odunc Al/Teslim Et\n0. Ana Menu\nSeciminiz: ");
                scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: add_student(library); break;
                    case 2: delete_student(library); break;
                    case 3: update_student(library); break;
                    case 4: display_student_info(library); break;
                    case 5: list_students_with_unreturned_books(library); break;
                    case 6: list_penalized_students(library); break;
                    case 7: list_students(library); break;
                    case 8:
                        printf("\n1. Kitap Odunc Al\n2. Kitap Teslim Et\nSeciminiz: ");
                        scanf("%d", &borrowChoice);
                        if (borrowChoice == 1) borrow_book(library);
                        else if (borrowChoice == 2) return_book(library);
                        else printf("Gecersiz secim!\n");
                        break;
                    case 0: break;
                    default: printf("Gecersiz secim!\n");
                }
                break;
            case 2:
                printf("\n=== KITAP ISLEMLERI ===\n");
                printf("1. Kitap Ekle\n2. Kitap Sil\n3. Kitap Guncelle\n");
                printf("4. Kitap Bilgisi Goruntuleme\n5. Raftaki Kitaplari Listele\n");
                printf("6. Zamaninda Teslim Edilmeyen Kitaplari Listele\n7. Kitap-Yazar Eslestir\n");
                printf("8. Kitabin Yazarini Guncelle\n0. Ana Menu\nSeciminiz: ");
                scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: add_book(library); break;
                    case 2: delete_book(library); break;
                    case 3: update_book(library); break;
                    case 4: display_book_info(library); break;
                    case 5: list_available_books(library); break;
                    case 6: list_overdue_books(library); break;
                    case 7: assign_author_to_book(library); break;
                    case 8: update_book_authors(library); break;
                    case 0: break;
                    default: printf("Gecersiz secim!\n");
                }
                break;
            case 3:
                printf("\n=== YAZAR ISLEMLERI ===\n");
                printf("1. Yazar Ekle\n2. Yazar Sil\n3. Yazar Guncelle\n");
                printf("4. Yazar Bilgisi Goruntuleme\n5. Yazarlari Listele\n0. Ana Menu\nSeciminiz: ");
                scanf("%d", &subChoice);
                switch (subChoice) {
                    case 1: add_author(library); break;
                    case 2: delete_author(library); break;
                    case 3: update_author(library); break;
                    case 4: display_author_info(library); break;
                    case 5: list_authors(library); break;
                    case 0: break;
                    default: printf("Gecersiz secim!\n");
                }
                break;
            case 0:
                printf("Program sonlandiriliyor...\n");
                return;
            default:
                printf("Gecersiz secim!\n");
        }
        }
        }
int get_menu_choice() {
    int choice;
    printf("\n=== KUTUPHANE YONETIM SISTEMI ===\n");
    printf("1. OGRENCI ISLEMLERI\n2. KITAP ISLEMLERI\n3. YAZAR ISLEMLERI\n0. CIKIS\nSeciminiz: ");
    scanf("%d", &choice);
    return choice;
}
// -Yazar iþlemlemleri
int get_max_author_id(Library* library) {
    Author* current = library->authorList;
    int maxID = 0;
    if (library->authorList == NULL) {
        return 0;
    }
    while (current != NULL) {
        if (current->authorID > maxID) {
            maxID = current->authorID;
        }
        current = current->next;
    }
    return maxID;}
void load_authors(Library* library) {
    Author* current = library->authorList;
    Author* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    library->authorList = NULL;
    generic_load_list("Yazarlar.csv", (void**)&library->authorList, parse_and_add_author, library);
}

void save_authors(Library* library) {
    generic_save_list("Yazarlar.csv", library->authorList, "authorID,name,surname", write_author_csv, get_next_author_node);
  }

Author* find_author(Library* library, int authorID) {
    return (Author*)generic_find_by_int_id(library->authorList, get_next_author_node, get_author_id_for_find, authorID);
 }

void add_author(Library* library) {
    Author* newAuthor = (Author*)malloc(sizeof(Author));
    Author* current_insert;
    Author* prev_insert;
    if (newAuthor == NULL) {
        printf("Bellek yetersiz!\n");
        return;
    }
    newAuthor->authorID = get_max_author_id(library) + 1;
    printf("Yazar adi: ");
    getchar();
    fgets(newAuthor->name, sizeof(newAuthor->name), stdin);
    newAuthor->name[strcspn(newAuthor->name, "\n")] = '\0';
    printf("Yazar soyadi: ");
    fgets(newAuthor->surname, sizeof(newAuthor->surname), stdin);
    newAuthor->surname[strcspn(newAuthor->surname, "\n")] = '\0';
    newAuthor->next = NULL;

    if (library->authorList == NULL || library->authorList->authorID >= newAuthor->authorID) {
        newAuthor->next = library->authorList;
        library->authorList = newAuthor;
    } else {
        current_insert = library->authorList;
        prev_insert = NULL;
         while (current_insert != NULL && current_insert->authorID < newAuthor->authorID) {
            prev_insert = current_insert;
            current_insert = current_insert->next;
        }
        prev_insert->next = newAuthor;
        newAuthor->next = current_insert;
    }
    save_authors(library);
    printf("Yazar basariyla eklendi. Yazar ID: %d\n", newAuthor->authorID);
}

void update_author(Library* library) {
    int authorID;
    Author* author;
    if (library->authorList == NULL) {
        printf("Yazar listesi bos!\n");
        return;}
    printf("Guncellenecek yazar ID: ");
    scanf("%d", &authorID);
    author = find_author(library, authorID);
    if (author == NULL) {
        printf("Yazar bulunamadi!\n");
        return; }
    printf("Yeni ad: ");
    getchar();
    fgets(author->name, sizeof(author->name), stdin);
    author->name[strcspn(author->name, "\n")] = '\0';
    printf("Yeni soyad: ");
    fgets(author->surname, sizeof(author->surname), stdin);
    author->surname[strcspn(author->surname, "\n")] = '\0';
    save_authors(library);
    printf("Yazar bilgileri guncellendi.\n");
    }


void list_authors(Library* library) {
    Author* current;
    if (library->authorList == NULL) {
        printf("Yazar listesi bos!\n");
        return;
    }
    printf("\nYazar Listesi:\n%-10s %-20s %-20s\n", "ID", "Ad", "Soyad");
    printf("--------------------------------------------------------\n");
    current = library->authorList;
    while (current != NULL) {
        printf("%-10d %-20s %-20s\n", current->authorID, current->name, current->surname);
        current = current->next;
    }
    }

// Öðrenci iþlemleri

void load_students(Library* library) {
    Student* current = library->studentList;
    Student* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    library->studentList = NULL;
    generic_load_list("Ogrenciler.csv", (void**)&library->studentList, parse_and_add_student, library);
}
void save_students(Library* library) {
    generic_save_list("Ogrenciler.csv", library->studentList, "ogrNo,name,surname,puan", write_student_csv, get_next_student_node);
}
Student* find_student(Library* library, int ogrNo) {
    return (Student*)generic_find_by_int_id(library->studentList, get_next_student_node, get_student_id_for_find, ogrNo);
}
void add_student(Library* library) {
    Student* newStudent = (Student*)malloc(sizeof(Student));
    Student* current_insert;
    Student* existing_student_check;

    if (newStudent == NULL) {
        printf("Bellek yetersiz!\n");
        return;
    }
    printf("Ogrenci no (8 haneli): ");
    scanf("%d", &newStudent->ogrNo);
    if (newStudent->ogrNo < 10000000 || newStudent->ogrNo > 99999999) {
        printf("Ogrenci numarasi 8 haneli olmalidir!\n");
        free(newStudent);
        return;
    }
    existing_student_check = find_student(library, newStudent->ogrNo);
    if (existing_student_check != NULL) {
        printf("Bu ogrenci numarasi zaten kayitli!\n");
        free(newStudent);
        return;
    }
    printf("Ogrenci adi: ");
    getchar();
    fgets(newStudent->name, sizeof(newStudent->name), stdin);
    newStudent->name[strcspn(newStudent->name, "\n")] = '\0';
    printf("Ogrenci soyadi: ");
    fgets(newStudent->surname, sizeof(newStudent->surname), stdin);
    newStudent->surname[strcspn(newStudent->surname, "\n")] = '\0';
    newStudent->puan = 100;
    newStudent->next = NULL;
    newStudent->prev = NULL;

    if (library->studentList == NULL || library->studentList->ogrNo >= newStudent->ogrNo) {
        newStudent->next = library->studentList;
        if (library->studentList != NULL) {
            library->studentList->prev = newStudent;
        }
        library->studentList = newStudent;
    } else {
        current_insert = library->studentList;
        while (current_insert->next != NULL && current_insert->next->ogrNo < newStudent->ogrNo) {
            current_insert = current_insert->next;
        }
        newStudent->next = current_insert->next;
        if (current_insert->next != NULL) {
            current_insert->next->prev = newStudent;
        }
        current_insert->next = newStudent;
        newStudent->prev = current_insert;
    }
    save_students(library);
    printf("Ogrenci basariyla eklendi.\n");
}

void delete_student(Library* library) {
    int ogrNo;
    Student* studentToDelete;
    if (library->studentList == NULL) {
        printf("Ogrenci listesi bos!\n");
        return;
    }
    printf("Silinecek ogrenci no: ");
    scanf("%d", &ogrNo);
    studentToDelete = find_student(library, ogrNo);
    if (studentToDelete == NULL) {
        printf("Ogrenci bulunamadi!\n");
        return;}
    if (studentToDelete->prev != NULL) {
        studentToDelete->prev->next = studentToDelete->next;
    } else {
        library->studentList = studentToDelete->next;
    }

	if (studentToDelete->next != NULL) {
        studentToDelete->next->prev = studentToDelete->prev;
    }
    free(studentToDelete);
    save_students(library);
    printf("Ogrenci basariyla silindi.\n");
}
void update_student(Library* library) {
    int ogrNo;
    Student* student;
    int newOgrNo;
    if (library->studentList == NULL) {
        printf("Ogrenci listesi bos!\n");
        return;
    }
    printf("Guncellenecek ogrenci no: ");
    scanf("%d", &ogrNo);
    student = find_student(library, ogrNo);
    if (student == NULL) {
        printf("Ogrenci bulunamadi!\n");
        return;
    }
    printf("Yeni ogrenci no (8 haneli, 0: Degistirme): ");
    scanf("%d", &newOgrNo);
    getchar();
    if (newOgrNo != 0) {
        if (newOgrNo < 10000000 || newOgrNo > 99999999) {
            printf("Ogrenci numarasi 8 haneli olmalidir!\n");
            return;
        }
        if (find_student(library, newOgrNo) != NULL) {
            printf("Bu ogrenci numarasi zaten kayitli!\n");
            return;
        }
        student->ogrNo = newOgrNo;
        }
    printf("Yeni ad: ");
    fgets(student->name, sizeof(student->name), stdin);
    student->name[strcspn(student->name, "\n")] = '\0';
    printf("Yeni soyad: ");
    fgets(student->surname, sizeof(student->surname), stdin);
    student->surname[strcspn(student->surname, "\n")] = '\0';
    save_students(library);
    printf("Ogrenci bilgileri guncellendi.\n");
}


void list_students(Library* library) {
    Student* current;
    if (library->studentList == NULL) {
        printf("Ogrenci listesi bos!\n");
        return;
    }
    printf("\nOgrenci Listesi:\n%-10s %-20s %-20s %-10s\n", "No", "Ad", "Soyad", "Puan");
    printf("--------------------------------------------------------\n");
    current = library->studentList;
    while (current != NULL) {
        printf("%-10d %-20s %-20s %-10d\n", current->ogrNo, current->name, current->surname, current->puan);
        current = current->next;
    }}

//Kitap iþlem
void load_books(Library* library) {
    Book* current_book_to_delete = library->bookList;
    Book* temp_book;
    BookCopy* current_copy_to_delete, *temp_copy;
    while (current_book_to_delete != NULL) {
        temp_book = current_book_to_delete;
        current_book_to_delete = current_book_to_delete->next;
        current_copy_to_delete = temp_book->copies;
        while (current_copy_to_delete != NULL) {
            temp_copy = current_copy_to_delete;
            current_copy_to_delete = current_copy_to_delete->next;
            free(temp_copy);
        }
        free(temp_book);
        }
    library->bookList = NULL;
    generic_load_list("Kitaplar.csv", (void**)&library->bookList, parse_and_add_book, library);
}

void save_books(Library* library) {
    generic_save_list("Kitaplar.csv", library->bookList, "isbn,title", write_book_csv, get_next_book_node);
}

void load_book_copies(Library* library) {
    Book* book_iter = library->bookList;
    BookCopy* copy_iter, *temp_copy;
    while (book_iter != NULL) {
        copy_iter = book_iter->copies;
        while (copy_iter != NULL) {
            temp_copy = copy_iter;
            copy_iter = copy_iter->next;
            free(temp_copy);
        }
        book_iter->copies = NULL;
        book_iter = book_iter->next;
 }
    generic_load_list("KitapOrnekleri.csv", NULL, parse_and_add_book_copy, library);
}

void save_book_copies(Library* library) {
    FILE* file = fopen("KitapOrnekleri.csv", "w");
    Book* current_book;
    BookCopy* current_copy;
    if (file == NULL) {
        printf("Kitap kopyalari dosyasi acilamadi! (KitapOrnekleri.csv)\n");
        return;
    }
    fprintf(file, "isbn,tag,isAvailable,studentID\n");
    current_book = library->bookList;
    while (current_book != NULL) {
        current_copy = current_book->copies;
        while (current_copy != NULL) {
            fprintf(file, "%s,%s,%d,%d\n", current_book->isbn, current_copy->tag, current_copy->isAvailable, current_copy->studentID);
            current_copy = current_copy->next;
        }
        current_book = current_book->next;
    }
    fclose(file);
    }
Book* find_book_by_isbn(Library* library, const char* isbn) {
    return (Book*)generic_find_by_string_key(library->bookList, get_next_book_node, get_book_isbn_for_find, isbn);
}

BookCopy* find_book_copy(Book* book, const char* tag) {
    if (!book) {
        return NULL;
    }
    return (BookCopy*)generic_find_by_string_key(book->copies, get_next_book_copy_node, get_book_copy_tag_for_find, tag);
    }


void add_book(Library* library) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    int quantity;
    BookCopy* newCopy, *lastCopy = NULL;
    Book* current, *prev;
    int i;

    if (newBook == NULL) {
        printf("Bellek yetersiz!\n");
        return;
    }
    printf("Kitap adi: ");
    getchar();
    fgets(newBook->title, sizeof(newBook->title), stdin);
    newBook->title[strcspn(newBook->title, "\n")] = '\0';
    printf("ISBN (13 haneli): ");
    fgets(newBook->isbn, sizeof(newBook->isbn), stdin);
    newBook->isbn[strcspn(newBook->isbn, "\n")] = '\0';
    if (strlen(newBook->isbn) != 13) {
        printf("ISBN numarasi 13 haneli olmalidir!\n");
        free(newBook);
        return;
    }
    if (find_book_by_isbn(library, newBook->isbn) != NULL) {
        printf("Bu ISBN numarasi zaten kayitli!\n");
        free(newBook);
        return;
    }

	printf("Adet: ");
    scanf("%d", &quantity);
    getchar();
    if (quantity <= 0) {
        printf("Adet pozitif bir sayi olmalidir!\n");
        free(newBook);
        return;
    }
    newBook->copies = NULL;
    for (i = 0; i < quantity; i++) {
        newCopy = (BookCopy*)malloc(sizeof(BookCopy));
        if (newCopy == NULL) {
            printf("Kopya olusturulurken bellek yetersiz!\n");
            BookCopy* tempCopyClean = newBook->copies;
            BookCopy* toDel;
            while(tempCopyClean){
                toDel = tempCopyClean;
                tempCopyClean=tempCopyClean->next;
                free(toDel);
            }
            free(newBook);
            return;
        }
        sprintf(newCopy->tag, "%s_%d", newBook->isbn, i + 1);
        newCopy->isAvailable = 1;
        newCopy->studentID = 0;
        newCopy->next = NULL;
        if (newBook->copies == NULL) {
            newBook->copies = newCopy;
        } else {
            lastCopy->next = newCopy;
        }
        lastCopy = newCopy;
    }
    current = library->bookList;
    prev = NULL;
    while (current != NULL && (strcmp(current->title, newBook->title) < 0 || (strcmp(current->title, newBook->title) == 0 && strcmp(current->isbn, newBook->isbn) < 0))) {
        prev = current;
        current = current->next;
    }
    if (prev == NULL) {
        newBook->next = library->bookList;
        library->bookList = newBook;
    } else {
        prev->next = newBook;
        newBook->next = current;}
    save_books(library);
    save_book_copies(library);
    printf("Kitap basariyla eklendi. ISBN: %s\n", newBook->isbn);
}
void update_book(Library* library) {
    char isbn[20];
    char new_isbn[20];
    Book* book;
    char new_title[100];
    if (library->bookList == NULL) {
        printf("Kitap listesi bos!\n");
        return;
    }
    printf("Guncellenecek kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';
    book = find_book_by_isbn(library, isbn);
    if (book == NULL) {
        printf("Kitap bulunamadi!\n");
        return;}
    printf("Yeni ISBN (13 haneli, 0: Degistirme): ");
    fgets(new_isbn, sizeof(new_isbn), stdin);
    new_isbn[strcspn(new_isbn, "\n")] = '\0';
    if (strcmp(new_isbn, "0") != 0) {
        if (strlen(new_isbn) != 13) {
            printf("ISBN numarasi 13 haneli olmalidir!\n");
            return;
        }
        if (find_book_by_isbn(library, new_isbn) != NULL) {
            printf("Bu ISBN numarasi zaten kayitli!\n");
            return;
        }
        strncpy(book->isbn, new_isbn, sizeof(book->isbn) - 1);
        book->isbn[sizeof(book->isbn) - 1] = '\0';
    }
    printf("Yeni kitap adi (Mevcut: %s): ", book->title);
    fgets(new_title, sizeof(new_title), stdin);
    new_title[strcspn(new_title, "\n")] = '\0';
    if(strlen(new_title) > 0) {
        strncpy(book->title, new_title, sizeof(book->title)-1);
        book->title[sizeof(book->title)-1] = '\0';
    }
    save_books(library);
    printf("Kitap bilgileri guncellendi.\n");
}

void list_books(Library* library) {
    Book* current;
    if (library->bookList == NULL) {
        printf("Kitap listesi bos!\n");
        return;
    }
    printf("\nKitap Listesi:\n%-20s %s\n", "ISBN", "Ad");
    printf("--------------------------------------------------\n");
    current = library->bookList;
    while (current != NULL) {
        printf("%-20s %s\n", current->isbn, current->title);
        current = current->next;
    }
}

// --- Kitap-Yazar iliþkisi Fonksiyonlarý ---
void ensure_book_author_capacity(Library* library) {
    BookAuthorRelation* new_array;
    if (library->bookAuthorCount >= library->bookAuthorCapacity) {
        library->bookAuthorCapacity = (library->bookAuthorCapacity == 0) ? 10 : library->bookAuthorCapacity * 2;
        new_array = (BookAuthorRelation*)realloc(library->bookAuthorRelationsArray, library->bookAuthorCapacity * sizeof(BookAuthorRelation));
        if (new_array == NULL) {
            perror("Failed to reallocate bookAuthorRelationsArray");
            if(library->bookAuthorCapacity > 0) {
                library->bookAuthorCapacity /=2;
            } else {
                library->bookAuthorCapacity = 0;
            }
            return;
        }
        library->bookAuthorRelationsArray = new_array;
        }
        }
void load_book_author_relations(Library* library) {
    FILE* file = fopen("KitapYazar.csv", "r");
    char line[100];
    BookAuthorRelation* currentRelation;
    char* token;

    if (file == NULL) {
        return;
    }
    library->bookAuthorCount = 0;
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
        ensure_book_author_capacity(library);
        if(library->bookAuthorCount < library->bookAuthorCapacity) {
            currentRelation = &library->bookAuthorRelationsArray[library->bookAuthorCount];
            line[strcspn(line, "\r\n")] = 0;

            token = strtok(line, ",");
            if (token) {
                strncpy(currentRelation->isbn, token, sizeof(currentRelation->isbn) - 1);
                currentRelation->isbn[sizeof(currentRelation->isbn) - 1] = '\0';
            } else {
                strcpy(currentRelation->isbn, "");
            }

            token = strtok(NULL, ",");
            if (token) {
                currentRelation->authorID = atoi(token);
            } else {
                currentRelation->authorID = -2;
            }
            library->bookAuthorCount++;
        } else {
             printf("Kapasite yetersiz, KitapYazar iliskisi yuklenemedi (satir atlandi): %s", line);
        }
    }
    fclose(file);
    }

void save_book_author_relations(Library* library) {
    FILE* file = fopen("KitapYazar.csv", "w");
    int i;
    if (file == NULL) {
        printf("Kitap-yazar iliskileri dosyasi acilamadi! (KitapYazar.csv)\n");
        return;
    }
    fprintf(file, "isbn,authorID\n");
    for (i = 0; i < library->bookAuthorCount; i++) {
        if (library->bookAuthorRelationsArray[i].authorID != -2) {
            fprintf(file, "%s,%d\n", library->bookAuthorRelationsArray[i].isbn, library->bookAuthorRelationsArray[i].authorID);
        }
    }
    fclose(file);
}


void assign_author_to_book(Library* library) {
    char isbn[20];
    Book* book;
    int authorID;
    Author* author;
    int i;
    BookAuthorRelation* newRelation;
    char add_more;
    int already_assigned_flag;

    if (library->bookList == NULL) { printf("Kitap listesi bos!\n"); return; }
    if (library->authorList == NULL) { printf("Yazar listesi bos!\n"); return; }
    printf("Kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';

    book = find_book_by_isbn(library, isbn);
    if (book == NULL) { printf("Kitap bulunamadi!\n"); return; }

    do {
    printf("Yazar ID: ");
    scanf("%d", &authorID);
    getchar();
    author = find_author(library, authorID);

    if (author != NULL) {
        already_assigned_flag = 0;
        for (i = 0; i < library->bookAuthorCount && !already_assigned_flag; i++) {
            if (strcmp(library->bookAuthorRelationsArray[i].isbn, isbn) == 0 &&
                library->bookAuthorRelationsArray[i].authorID == authorID) {
                printf("Bu yazar zaten bu kitaba atanmis!\n");
                already_assigned_flag = 1;
            }
        }

        if (!already_assigned_flag) {
            ensure_book_author_capacity(library);
            if (library->bookAuthorCount < library->bookAuthorCapacity) {
                newRelation = &library->bookAuthorRelationsArray[library->bookAuthorCount];
                strncpy(newRelation->isbn, isbn, sizeof(newRelation->isbn) - 1);
                newRelation->isbn[sizeof(newRelation->isbn) - 1] = '\0';
                newRelation->authorID = authorID;
                library->bookAuthorCount++;
                printf("Yazar ID: %d kitaba eklendi.\n", authorID);
            } else {
                printf("Iliski eklenemedi, kapasite sorunu.\n");
            }
            }
    } else {
        printf("Yazar bulunamadi!\n");

    }

    printf("\nBaska bir yazar eklemek ister misiniz? (e/h): ");
    scanf(" %c", &add_more);
    getchar();
} while (add_more == 'e' || add_more == 'E');

save_book_author_relations(library);
printf("\nKitap yazarlarla eslestirilmistir.\n");
}

void update_author_relations_on_author_delete(Library* library, int authorID_to_delete) {
    int relations_updated = 0;
    int i;
    for (i = 0; i < library->bookAuthorCount; i++) {
        if (library->bookAuthorRelationsArray[i].authorID == authorID_to_delete) {
            library->bookAuthorRelationsArray[i].authorID = -1;
            relations_updated = 1;
        }
        }
    if (relations_updated) {
        save_book_author_relations(library);
    }
    }

void delete_author(Library* library) {
    int authorID;
    Author* current_auth, *prev_auth;
    if (library->authorList == NULL) { printf("Yazar listesi bos!\n"); return; }
    printf("Silinecek yazar ID: ");
    scanf("%d", &authorID);
    getchar();
    current_auth = library->authorList;
    prev_auth = NULL;
    while (current_auth != NULL && current_auth->authorID != authorID) {
        prev_auth = current_auth;
        current_auth = current_auth->next;
    }
    if (current_auth == NULL) { printf("Yazar bulunamadi!\n"); return; }

    if (prev_auth == NULL) {
        library->authorList = current_auth->next;
    } else {
        prev_auth->next = current_auth->next;
    }
    free(current_auth);

    update_author_relations_on_author_delete(library, authorID);
    save_authors(library);
    printf("Yazar basariyla silindi. Ilgili kitap-yazar eslestirmelerinde yazar -1 olarak guncellendi.\n");
}

void remove_book_author_relations_for_book(Library* library, const char* isbn_to_delete) {
    int new_count = 0;
    int relations_changed = 0;
    int i;
    for (i = 0; i < library->bookAuthorCount; i++) {
        if (strcmp(library->bookAuthorRelationsArray[i].isbn, isbn_to_delete) != 0) {
            if (new_count != i) {
                library->bookAuthorRelationsArray[new_count] = library->bookAuthorRelationsArray[i];
            }
            new_count++;
        } else {
            relations_changed = 1;
        }
    }
    if(relations_changed){
        library->bookAuthorCount = new_count;
        save_book_author_relations(library);
    }
    }
void delete_book(Library* library) {
    char isbn[20];
    Book* current_book_node, *prev_book_node;
    BookCopy* copy, *temp;

    if (library->bookList == NULL) { printf("Kitap listesi bos!\n"); return; }
    printf("Silinecek kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';

    current_book_node = library->bookList;
    prev_book_node = NULL;
    while (current_book_node != NULL && strcmp(current_book_node->isbn, isbn) != 0) {
        prev_book_node = current_book_node;
        current_book_node = current_book_node->next;
    }
    if (current_book_node == NULL) { printf("Kitap bulunamadi!\n"); return; }

    copy = current_book_node->copies;
    while (copy != NULL) {
        temp = copy;
        copy = copy->next;
        free(temp);
    }
    if (prev_book_node == NULL) {
        library->bookList = current_book_node->next;
    } else {
        prev_book_node->next = current_book_node->next;}
    free(current_book_node);

    remove_book_author_relations_for_book(library, isbn);
    save_books(library);
    save_book_copies(library);
    printf("Kitap ve ilgili tum iliskileri basariyla silindi.\n");}

// --- Kitap-Yazar Ýliþkisi Fonksiyonlar ---
void update_book_authors(Library* library) {
    char isbn[20];
    Book* book;
    int current_author_count_for_book;
    int i;
    Author* author;
    int author_id_to_delete;
    int found_to_delete, new_idx;
    char add_more;
    int author_id_to_add;
    Author* author_to_add;
    int already_exists_flag;
    BookAuthorRelation* newRel;

    if (library->bookList == NULL) {
        printf("Kitap listesi bos!\n");
        return;
    }
    printf("Yazarlarini guncellemek istediginiz kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';

    book = find_book_by_isbn(library, isbn);
    if (book == NULL) {
        printf("Kitap bulunamadi!\n");
        return;
    }
    printf("\nKitabin mevcut yazarlari (ISBN: %s):\n", isbn);
    current_author_count_for_book = 0;
    for (i = 0; i < library->bookAuthorCount; i++) {
        if (strcmp(library->bookAuthorRelationsArray[i].isbn, isbn) == 0 &&
            library->bookAuthorRelationsArray[i].authorID != -1) {
            author = find_author(library, library->bookAuthorRelationsArray[i].authorID);
            if (author) {
                printf("%d. Yazar ID: %d - Ad: %s %s\n", ++current_author_count_for_book, author->authorID, author->name, author->surname);
            }
        }
    }
    if (current_author_count_for_book == 0) {
        printf("Bu kitabin atanmis (veya silinmemis) bir yazari bulunmamaktadir.\n");
    }

    if (current_author_count_for_book > 0) {
        printf("\nBu kitaptan silmek istediginiz bir yazar var mi? (Yazar ID'sini girin, 0: Hayir): ");
        scanf("%d", &author_id_to_delete);
        getchar();
        if (author_id_to_delete != 0) {
            found_to_delete = 0;
            new_idx = 0;
            for (i = 0; i < library->bookAuthorCount; i++) {
                if (strcmp(library->bookAuthorRelationsArray[i].isbn, isbn) == 0 &&
                    library->bookAuthorRelationsArray[i].authorID == author_id_to_delete) {
                    found_to_delete = 1;
                } else {
                    if (new_idx != i) {
                        library->bookAuthorRelationsArray[new_idx] = library->bookAuthorRelationsArray[i];
                    }
                    new_idx++;
                }
            }
            if(found_to_delete) {
                library->bookAuthorCount = new_idx;
                printf("Yazar iliskisi kitaptan kaldirildi.\n");
            } else {
                printf("Belirtilen Yazar ID ile bu kitabin bir iliskisi bulunamadi.\n");
            }
        }
    }

    do {
        printf("\nBu kitaba yeni bir yazar eklemek ister misiniz? (e/h): ");
        scanf(" %c", &add_more);
        getchar();
        if (add_more == 'e' || add_more == 'E') {
            printf("Eklenecek Yazar ID: ");
            scanf("%d", &author_id_to_add);
            getchar();

            author_to_add = find_author(library, author_id_to_add);
            if (author_to_add == NULL) {
                printf("Yazar ID: %d bulunamadi.\n", author_id_to_add);
            } else {
                already_exists_flag = 0;
                for (i = 0; i < library->bookAuthorCount && !already_exists_flag; i++) {
                    if (strcmp(library->bookAuthorRelationsArray[i].isbn, isbn) == 0 &&
                        library->bookAuthorRelationsArray[i].authorID == author_id_to_add) {
                        already_exists_flag = 1;
                    }
                }
                if (already_exists_flag) {
                    printf("Yazar ID: %d zaten bu kitaba atanmis.\n", author_id_to_add);
                } else {
                    ensure_book_author_capacity(library);
                    if(library->bookAuthorCount >= library->bookAuthorCapacity) {
                        printf("Iliski eklenemedi, kapasite sorunu.\n");
                    } else {
                        newRel = &library->bookAuthorRelationsArray[library->bookAuthorCount];
                        strncpy(newRel->isbn, isbn, sizeof(newRel->isbn) - 1);
                        newRel->isbn[sizeof(newRel->isbn) - 1] = '\0';
                        newRel->authorID = author_id_to_add;
                        library->bookAuthorCount++;
                        printf("Yazar ID: %d kitaba eklendi.\n", author_id_to_add);
                    }
                }
            }
        }
    } while (add_more == 'e' || add_more == 'E');

    save_book_author_relations(library);
    printf("\nKitabin yazarlari guncellenmistir.\n");
}

void list_books_by_author(Library* library) {
    int authorID;
    Author* author;
    int found;
    int i;
    Book* book;

    if (library->authorList == NULL) { printf("Yazar listesi bos!\n"); return; }

    printf("Yazar ID: ");
    scanf("%d", &authorID);
    getchar();

    author = find_author(library, authorID);
    if (author == NULL) { printf("Yazar bulunamadi!\n"); return; }

    printf("\n%s %s'nin kitaplari:\nISBN\t\tKitap Adi\n------------------------\n", author->name, author->surname);
    found = 0;
    for (i = 0; i < library->bookAuthorCount; i++) {
        if (library->bookAuthorRelationsArray[i].authorID == authorID) {
            book = find_book_by_isbn(library, library->bookAuthorRelationsArray[i].isbn);
            if (book != NULL) {
                printf("%s\t%s\n", book->isbn, book->title);
                found = 1;
            }
        }
    }
    if (!found) {
        printf("Bu yazara ait kitap bulunamadi.\n");
    }
}

// --- Transaction iþlemleri ---
int get_max_transaction_id(Library* library) {
    Transaction* current = library->transactionList;
    int maxID = 0;
    if (library->transactionList == NULL) {
        return 0;
    }
    while (current != NULL) {
        if (current->transactionID > maxID) {
            maxID = current->transactionID;
        }
        current = current->next;
        }
    return maxID;
}

void load_transactions(Library* library) {
    Transaction* current = library->transactionList;
    Transaction* temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    library->transactionList = NULL;
    generic_load_list("KitapOdunc.csv", (void**)&library->transactionList, parse_and_add_transaction, library);
}

void save_transactions(Library* library) {
    generic_save_list("KitapOdunc.csv", library->transactionList, "transactionID,book_isbn,copy_tag,studentID,transactionType,date,penaltyPoints", write_transaction_csv, get_next_transaction_node);
}

void log_transaction(Library* library, const char* book_isbn, const char* copy_tag, int studentID, const char* transactionType, int penaltyPoints) {
    Transaction* newTransaction = (Transaction*)malloc(sizeof(Transaction));
    char* currentDate;
    if (newTransaction == NULL) {
        printf("Bellek yetersiz!\n");
        return;
    }

    newTransaction->transactionID = get_max_transaction_id(library) + 1;
    strncpy(newTransaction->book_isbn, book_isbn, sizeof(newTransaction->book_isbn) - 1);
    newTransaction->book_isbn[sizeof(newTransaction->book_isbn) - 1] = '\0';
    strncpy(newTransaction->copy_tag, copy_tag, sizeof(newTransaction->copy_tag) - 1);
    newTransaction->copy_tag[sizeof(newTransaction->copy_tag) - 1] = '\0';
    newTransaction->studentID = studentID;
    strncpy(newTransaction->transactionType, transactionType, sizeof(newTransaction->transactionType) - 1);
    newTransaction->transactionType[sizeof(newTransaction->transactionType) - 1] = '\0';
    currentDate = get_current_date();
    if (currentDate) {
        strncpy(newTransaction->date, currentDate, sizeof(newTransaction->date) - 1);
        newTransaction->date[sizeof(newTransaction->date) - 1] = '\0';
        free(currentDate);
    } else {
        strcpy(newTransaction->date, "0000-00-00");
    }
    newTransaction->penaltyPoints = penaltyPoints;
    newTransaction->next = library->transactionList;
    library->transactionList = newTransaction;
    save_transactions(library);
}

// --- Kitap Ödünç/iade ---
void borrow_book(Library* library) {
    char isbn[20];
    int studentID_input;
    Book* book;
    Student* student;
    BookCopy* availableCopy = NULL;
    BookCopy* currentCopy;
    int found_copy_flag = 0;

    printf("Odunc verilecek kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';
    book = find_book_by_isbn(library, isbn);
    if (book == NULL) {
        printf("Bu ISBN'e sahip bir kitap bulunamadi.\n");
        return;
    }
    printf("Odunc alacak ogrenci no: ");
    scanf("%d", &studentID_input);
    getchar();
    student = find_student(library, studentID_input);
    if (student == NULL) {
        printf("Bu ID'ye sahip bir ogrenci bulunamadi.\n");
        return;
    }

    if (student->puan < 0) {
        printf("Bu ogrenci cezali oldugu icin kitap odunc alamaz.\n");
        return;
    }
    currentCopy = book->copies;
    while (currentCopy != NULL && !found_copy_flag) {
        if (currentCopy->isAvailable) {
            availableCopy = currentCopy;
            found_copy_flag = 1;
        }
        if (!found_copy_flag) {
             currentCopy = currentCopy->next;
        }
    }
    if (availableCopy == NULL) {
        printf("Bu kitabin musait kopyasi yok.\n");
        return;
    }
    availableCopy->isAvailable = 0;
    availableCopy->studentID = studentID_input;
    log_transaction(library, book->isbn, availableCopy->tag, studentID_input, "borrow", 0);
    save_book_copies(library);
    printf("Kitap basariyla odunc verildi.\n");
}

void return_book(Library* library) {
    char isbn[20], tag[25];
    Book* book;
    BookCopy* copyToReturn;
    int studentID_of_borrower;
    Student* student;
    Transaction* borrowTransaction = NULL, *currentTransaction, *latestBorrowTransactionForThisCopy = NULL;
    int penaltyPoints = 0;
    char* currentDate_str;
    int daysDiff;

    printf("Iade edilecek kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = '\0';
    book = find_book_by_isbn(library, isbn);
    if (book == NULL) {
        printf("Bu ISBN'e sahip bir kitap bulunamadi.\n");
        return;
    }
    printf("Iade edilecek kopya etiketi: ");
    fgets(tag, sizeof(tag), stdin);
    tag[strcspn(tag, "\n")] = '\0';
    copyToReturn = find_book_copy(book, tag);
    if (copyToReturn == NULL) {
        printf("Bu etikete sahip bir kopya bulunamadi.\n");
        return;
    }
    if (copyToReturn->isAvailable) {
        printf("Bu kopya zaten kutuphanede.\n");
        return;
    }
    studentID_of_borrower = copyToReturn->studentID;
    student = find_student(library, studentID_of_borrower);
    currentTransaction = library->transactionList;
    while (currentTransaction != NULL) {
        if (strcmp(currentTransaction->copy_tag, tag) == 0 &&
            currentTransaction->studentID == studentID_of_borrower &&
            strcmp(currentTransaction->transactionType, "borrow") == 0) {
            if (latestBorrowTransactionForThisCopy == NULL ||
                currentTransaction->transactionID > latestBorrowTransactionForThisCopy->transactionID) {
                latestBorrowTransactionForThisCopy = currentTransaction;
            }
        }
        currentTransaction = currentTransaction->next;
    }
    borrowTransaction = latestBorrowTransactionForThisCopy;
    currentDate_str = get_current_date();
    if (borrowTransaction != NULL && currentDate_str != NULL) {
        daysDiff = calculate_days_difference(borrowTransaction->date, currentDate_str);
        if (daysDiff > 15) {
            penaltyPoints = 10;
        }
    } else if (currentDate_str == NULL) {
        printf("Uyari: Guncel tarih alinamadigi icin ceza hesaplanamadi.\n");
    } else if (borrowTransaction == NULL) {
        printf("Uyari: Kitabin odunc alma kaydi bulunamadi, ceza hesaplanamadi.\n");
    }
    if(currentDate_str) {
        free(currentDate_str);
    }
    copyToReturn->isAvailable = 1;
    copyToReturn->studentID = 0;
    if (student != NULL) {
        student->puan -= penaltyPoints;
    }
    log_transaction(library, book->isbn, copyToReturn->tag, studentID_of_borrower, "return", penaltyPoints);
    save_book_copies(library);

    if (student != NULL) {
        save_students(library);
    }
    printf("Kitap basariyla iade alindi.\n");
    if (penaltyPoints > 0) {
        printf("15 gunden fazla gecikme icin %d puan ceza uygulandi.\n", penaltyPoints);
    }
}

// --- Dier Listeleme ve Yardmc Fonksiyonlar ---

void list_unreturned_books(Library* library) {
    Book* currentBook;
    BookCopy* currentCopy;
    Student* student;
    int found;
    if (library->bookList == NULL) { printf("Kitap listesi bos!\n"); return; }
    printf("\nIade Edilmemis Kitaplar:\n%-20s %-30s %-10s %-15s %-15s\n", "ISBN", "Kitap Adi", "Ogr No", "Ogr Adi", "Ogr Soyadi");
    printf("--------------------------------------------------------------------------------------------------\n");
    found = 0;
    currentBook = library->bookList;
    while (currentBook != NULL) {
        currentCopy = currentBook->copies;
        while (currentCopy != NULL) {
            if (!currentCopy->isAvailable) {
                student = find_student(library, currentCopy->studentID);
                if (student != NULL) {
                    printf("%-20s %-30s %-10d %-15s %-15s\n", currentBook->isbn, currentBook->title, student->ogrNo, student->name, student->surname);
                    found = 1;
                }
            }
            currentCopy = currentCopy->next;
        }
        currentBook = currentBook->next;
    }
    if (!found) {
        printf("Iade edilmemis kitap bulunamadi.\n");
    }
}

void list_penalized_students(Library* library) {
    Student* current;
    int found;
    if (library->studentList == NULL) { printf("Ogrenci listesi bos!\n"); return; }
    printf("\nCezali Ogrenciler (Puan < 0):\n%-10s %-20s %-20s %s\n", "Ogr No", "Ad", "Soyad", "Puan");
    printf("------------------------------------------------------------\n");
    found = 0;
    current = library->studentList;
    while (current != NULL) {
        if (current->puan < 0) {
            printf("%-10d %-20s %-20s %d\n", current->ogrNo, current->name, current->surname, current->puan);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("Cezali ogrenci bulunamadi.\n");
    }
    }


char* get_current_date() {
    time_t t = time(NULL);
    struct tm tm_info = *localtime(&t);
    char* date_str = (char*)malloc(11 * sizeof(char));
    if (date_str == NULL) {
        return NULL;
    }
    sprintf(date_str, "%04d-%02d-%02d", tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday);
    return date_str;
}
int calculate_days_difference(const char* date1_str, const char* date2_str) {
    struct tm tm1 = {0}, tm2 = {0};
    time_t t1, t2;
    if (date1_str == NULL || date2_str == NULL) return -99999;
    if (sscanf(date1_str, "%d-%d-%d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday) != 3) return -99999;
    tm1.tm_year -= 1900;
    tm1.tm_mon -= 1;
    if (sscanf(date2_str, "%d-%d-%d", &tm2.tm_year, &tm2.tm_mon, &tm2.tm_mday) != 3) return -99999;
    tm2.tm_year -= 1900;
    tm2.tm_mon -= 1;
    t1 = mktime(&tm1);
    t2 = mktime(&tm2);
    if (t1 == (time_t)-1 || t2 == (time_t)-1) return -99999;
    return (int)(difftime(t2, t1) / (60 * 60 * 24));
}
void display_student_info(Library* library) {
    int choice;
    Student* student = NULL;
    int ogrNo;
    char name[50], surname[50];
    Student* current_s;
    Transaction* transaction;
    int found_trans;
    Book* book;
    int searching_student_flag = 1;

    printf("\nOgrenci arama yontemi:\n1. ID ile ara\n2. Ad-Soyad ile ara\nSeciminiz: ");
    scanf("%d", &choice);
    if (choice == 1) {
        printf("Ogrenci ID: ");
        scanf("%d", &ogrNo);
        student = find_student(library, ogrNo);
    } else if (choice == 2) {
        printf("Ad: ");
        getchar();
        fgets(name, sizeof(name), stdin);
        name[strcspn(name, "\n")] = '\0';
        printf("Soyad: ");
        fgets(surname, sizeof(surname), stdin);
        surname[strcspn(surname, "\n")] = '\0';
        current_s = library->studentList;
        while (current_s != NULL && searching_student_flag) {
            if (strcmp(current_s->name, name) == 0 && strcmp(current_s->surname, surname) == 0) {
                student = current_s;
                searching_student_flag = 0;
            }
            if (searching_student_flag) {
                 current_s = current_s->next;
            }
        }
    } else {
        printf("Gecersiz secim!\n");
        return;
    }
    if (student == NULL) {
        printf("Ogrenci bulunamadi!\n");
        return;
    }
    printf("\n=== OGRENCI BILGILERI ===\nID: %d\nAd: %s\nSoyad: %s\nPuan: %d\n", student->ogrNo, student->name, student->surname, student->puan);
    printf("\n=== KITAP HAREKETLERI ===\n");
    transaction = library->transactionList;
    found_trans = 0;
    while (transaction != NULL) {
        if (transaction->studentID == student->ogrNo) {
            found_trans = 1;
            book = find_book_by_isbn(library, transaction->book_isbn);
            printf("Kitap: %-25s | Kopya: %-15s | Islem: %-8s | Tarih: %s",
                   book ? book->title : transaction->book_isbn,
                   transaction->copy_tag,
                   transaction->transactionType,
                   transaction->date);
            if (transaction->penaltyPoints > 0) {
                printf(" | Ceza: %d puan", transaction->penaltyPoints);
            }
            printf("\n");
        }
        transaction = transaction->next;
    }
    if (!found_trans) {
        printf("Bu ogrenciye ait kitap hareketi bulunamadi.\n");
    }
}

void list_students_with_unreturned_books(Library* library) {
    Student* student_iter;
    Book* book_iter;
    BookCopy* copy_iter;
    int found_any;

    printf("\n=== KITAP TESLIM ETMEMIS OGRENCILER ===\n");
    student_iter = library->studentList;
    found_any = 0;
    printf("%-10s %-20s %-20s %-30s %s\n", "Ogr ID", "Ogr Ad", "Ogr Soyad", "Kitap Adi", "Kopya Etiketi");
    printf("------------------------------------------------------------------------------------------------------------\n");
    while (student_iter != NULL) {
        book_iter = library->bookList;
        while (book_iter != NULL) {
            copy_iter = book_iter->copies;
            while (copy_iter != NULL) {
                if (!copy_iter->isAvailable && copy_iter->studentID == student_iter->ogrNo) {
                    found_any = 1;
                    printf("%-10d %-20s %-20s %-30s %s\n",
                           student_iter->ogrNo, student_iter->name, student_iter->surname,
                           book_iter->title, copy_iter->tag);
                }
                copy_iter = copy_iter->next;
            }
            book_iter = book_iter->next;
        }
        student_iter = student_iter->next;
    }
    if (!found_any) {
        printf("Kitap teslim etmemis ogrenci bulunamadi.\n");
    }
}

void display_book_info(Library* library) {
    char isbn[20];
    Book* book;
    int authorFound;
    int i;
    Author* author;
    BookCopy* copy;
    Student* student_holder;
    printf("Kitap ISBN: ");
    getchar();
    fgets(isbn, sizeof(isbn), stdin);
    isbn[strcspn(isbn, "\n")] = 0;
    book = find_book_by_isbn(library, isbn);
    if (book != NULL) {
        printf("\n=== KITAP BILGILERI ===\nISBN: %s\nBaslik: %s\n", book->isbn, book->title);
        printf("Yazarlar: ");
        authorFound = 0;
        for(i=0; i < library->bookAuthorCount; i++){
            if(strcmp(library->bookAuthorRelationsArray[i].isbn, book->isbn) == 0 &&
               library->bookAuthorRelationsArray[i].authorID != -1){
                author = find_author(library, library->bookAuthorRelationsArray[i].authorID);
                if(author){
                    if(authorFound) printf(", ");
                    printf("%s %s", author->name, author->surname);
                    authorFound = 1;
                }
                }
                }
        if (!authorFound) printf("Bilinmeyen");
        printf("\n");
        printf("\n=== KITAP KOPYALARI ===\n");
        copy = book->copies;
        if (copy == NULL) printf("Bu kitaba ait kopya bulunmamaktadir.\n");
        while (copy != NULL) {
            printf("Kopya Etiketi: %-15s | Durum: %s", copy->tag, copy->isAvailable ? "Musait" : "Odunc Verilmis");
            if (!copy->isAvailable) {
                student_holder = find_student(library, copy->studentID);
                if (student_holder) {
                    printf(" | Ogrenci: %s %s (ID: %d)", student_holder->name, student_holder->surname, student_holder->ogrNo);
                }
                }
            printf("\n");
            copy = copy->next;
        }
        printf("\n");
    } else {
        printf("Kitap bulunamadi!\n");
    }
}

void list_available_books(Library* library) {
    Book* book_iter;
    int found_any;
    int availableCount;
    BookCopy* copy_iter;

    printf("\n=== RAFTAKI KITAPLAR ===\n%-20s %-30s %s\n", "ISBN", "Baslik", "Musait Kopya Sayisi");
    printf("----------------------------------------------------------------------\n");
    book_iter = library->bookList;
    found_any = 0;
    while (book_iter != NULL) {
        availableCount = 0;
        copy_iter = book_iter->copies;
        while (copy_iter != NULL) {
            if (copy_iter->isAvailable) {
                availableCount++;
            }
            copy_iter = copy_iter->next;
            }
        if (availableCount > 0) {
            found_any = 1;
            printf("%-20s %-30s %d\n", book_iter->isbn, book_iter->title, availableCount);
        }
        book_iter = book_iter->next;
    }
    if (!found_any) {
        printf("Rafta musait kitap bulunamadi.\n");
}
}

void list_overdue_books(Library* library) {
    char* currentDate_val;
    int found_any;
    Book* current_book_iter;
    BookCopy* current_copy_iter;
    Transaction* borrow_trans, *trans_iter, *latestBorrow;
    int daysDiff;
    Student* student;

    printf("\n=== ZAMANINDA TESLIM EDILMEYEN KITAPLAR (15 Gunden Fazla) ===\n");
    currentDate_val = get_current_date();
    if (!currentDate_val) {
        printf("Tarih alinamadi!\n");
        return;
    }
    found_any = 0;
    printf("%-20s %-15s %-20s %-20s %-10s %s\n", "Kitap ISBN", "Kopya Etiketi", "Ogr Ad", "Ogr Soyad", "Ogr ID", "Gecikme (Gun)");
    printf("------------------------------------------------------------------------------------------------------------------\n");
    current_book_iter = library->bookList;
    while(current_book_iter != NULL) {
        current_copy_iter = current_book_iter->copies;
        while(current_copy_iter != NULL) {
            if(!current_copy_iter->isAvailable) {
                borrow_trans = NULL;
                trans_iter = library->transactionList;
                latestBorrow = NULL;
                while(trans_iter != NULL) {
                    if(strcmp(trans_iter->copy_tag, current_copy_iter->tag) == 0 &&
                       trans_iter->studentID == current_copy_iter->studentID &&
                       strcmp(trans_iter->transactionType, "borrow") == 0) {
                        if(latestBorrow == NULL || trans_iter->transactionID > latestBorrow->transactionID) {
                            latestBorrow = trans_iter;
                        }
                }
                    trans_iter = trans_iter->next;
                }
                borrow_trans = latestBorrow;
                if(borrow_trans) {
                    daysDiff = calculate_days_difference(borrow_trans->date, currentDate_val);
                    if (daysDiff > 15) {
                        found_any = 1;
                        student = find_student(library, borrow_trans->studentID);
                        printf("%-20s %-15s %-20s %-20s %-10d %d\n",
                               borrow_trans->book_isbn, borrow_trans->copy_tag,
                               student ? student->name : "Bilinmeyen",
                               student ? student->surname : "",
                               borrow_trans->studentID,
                               daysDiff - 15);
                    }
                }
                }
            current_copy_iter = current_copy_iter->next;
    }
        current_book_iter = current_book_iter->next;
    }
    if (!found_any) {
        printf("Zamaninda teslim edilmeyen kitap bulunamadi.\n");
    }
    free(currentDate_val);
}
void display_author_info(Library* library) {
    char name[50], surname[50];
    int bookFound, found;
    Author* author_iter;
    int i;
    Book* book;
    printf("Yazar adi (veya bir kismi): ");
    getchar();
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';
    printf("Yazar soyadi (veya bir kismi): ");
    fgets(surname, sizeof(surname), stdin);
    surname[strcspn(surname, "\n")] = '\0';
    author_iter = library->authorList;
    found = 0;
    while (author_iter != NULL) {
        if ((strlen(name) == 0 || strstr(author_iter->name, name) != NULL) &&
            (strlen(surname) == 0 || strstr(author_iter->surname, surname) != NULL)) {
            found = 1;
            printf("\n=== YAZAR BILGILERI ===\nID: %d\nAd: %s\nSoyad: %s\n",
                   author_iter->authorID, author_iter->name, author_iter->surname);
            printf("\n=== YAZARIN KITAPLARI ===\n");
            bookFound = 0;
            for(i=0; i < library->bookAuthorCount; i++) {
                if(library->bookAuthorRelationsArray[i].authorID == author_iter->authorID &&
                   library->bookAuthorRelationsArray[i].authorID != -1) {
                    book = find_book_by_isbn(library, library->bookAuthorRelationsArray[i].isbn);
                    if(book) {
                        if(!bookFound) {
                            printf("%-20s %s\n", "ISBN", "Baslik");
                            printf("------------------------------------------\n");
                            bookFound = 1;
                        }
                        printf("%-20s %s\n", book->isbn, book->title);
                    }
                    }
                    }
            if (!bookFound) {
                printf("Bu yazara ait kitap bulunamadi.\n");
            }
            printf("\n");
        }
        author_iter = author_iter->next;
    }
    if (!found) {
        printf("Belirtilen kriterlere uyan yazar bulunamadi!\n");
    }
}
