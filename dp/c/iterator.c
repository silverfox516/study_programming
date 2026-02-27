#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Forward declarations
typedef struct Iterator Iterator;
typedef struct Collection Collection;

// Iterator interface
typedef struct Iterator {
    bool (*has_next)(struct Iterator* self);
    void* (*next)(struct Iterator* self);
    void (*reset)(struct Iterator* self);
    void (*destroy)(struct Iterator* self);
} Iterator;

// Collection interface
typedef struct Collection {
    Iterator* (*create_iterator)(struct Collection* self);
    void (*add)(struct Collection* self, void* item);
    int (*size)(struct Collection* self);
    void (*destroy)(struct Collection* self);
} Collection;

// Book structure for our example
typedef struct {
    char* title;
    char* author;
    int year;
    float price;
} Book;

Book* book_create(const char* title, const char* author, int year, float price) {
    Book* book = malloc(sizeof(Book));
    book->title = malloc(strlen(title) + 1);
    strcpy(book->title, title);
    book->author = malloc(strlen(author) + 1);
    strcpy(book->author, author);
    book->year = year;
    book->price = price;
    return book;
}

void book_destroy(Book* book) {
    if (book->title) free(book->title);
    if (book->author) free(book->author);
    free(book);
}

void book_print(Book* book) {
    printf("  \"%s\" by %s (%d) - $%.2f\n", book->title, book->author, book->year, book->price);
}

// Concrete Collection - Book Library
typedef struct {
    Collection base;
    Book** books;
    int count;
    int capacity;
} BookLibrary;

// Forward iterator for BookLibrary
typedef struct {
    Iterator base;
    BookLibrary* library;
    int current_index;
} BookIterator;

bool book_iterator_has_next(Iterator* self) {
    BookIterator* iter = (BookIterator*)self;
    return iter->current_index < iter->library->count;
}

void* book_iterator_next(Iterator* self) {
    BookIterator* iter = (BookIterator*)self;
    if (iter->current_index < iter->library->count) {
        return iter->library->books[iter->current_index++];
    }
    return NULL;
}

void book_iterator_reset(Iterator* self) {
    BookIterator* iter = (BookIterator*)self;
    iter->current_index = 0;
}

void book_iterator_destroy(Iterator* self) {
    free(self);
}

Iterator* book_library_create_iterator(Collection* self) {
    BookLibrary* library = (BookLibrary*)self;
    BookIterator* iter = malloc(sizeof(BookIterator));

    iter->base.has_next = book_iterator_has_next;
    iter->base.next = book_iterator_next;
    iter->base.reset = book_iterator_reset;
    iter->base.destroy = book_iterator_destroy;

    iter->library = library;
    iter->current_index = 0;

    return (Iterator*)iter;
}

void book_library_add(Collection* self, void* item) {
    BookLibrary* library = (BookLibrary*)self;

    if (library->count >= library->capacity) {
        library->capacity *= 2;
        library->books = realloc(library->books, library->capacity * sizeof(Book*));
    }

    library->books[library->count++] = (Book*)item;
}

int book_library_size(Collection* self) {
    BookLibrary* library = (BookLibrary*)self;
    return library->count;
}

void book_library_destroy(Collection* self) {
    BookLibrary* library = (BookLibrary*)self;

    for (int i = 0; i < library->count; i++) {
        book_destroy(library->books[i]);
    }
    free(library->books);
    free(library);
}

Collection* book_library_create() {
    BookLibrary* library = malloc(sizeof(BookLibrary));

    library->base.create_iterator = book_library_create_iterator;
    library->base.add = book_library_add;
    library->base.size = book_library_size;
    library->base.destroy = book_library_destroy;

    library->books = malloc(4 * sizeof(Book*));
    library->count = 0;
    library->capacity = 4;

    return (Collection*)library;
}

// Reverse iterator for BookLibrary
typedef struct {
    Iterator base;
    BookLibrary* library;
    int current_index;
} ReverseBookIterator;

bool reverse_book_iterator_has_next(Iterator* self) {
    ReverseBookIterator* iter = (ReverseBookIterator*)self;
    return iter->current_index >= 0;
}

void* reverse_book_iterator_next(Iterator* self) {
    ReverseBookIterator* iter = (ReverseBookIterator*)self;
    if (iter->current_index >= 0) {
        return iter->library->books[iter->current_index--];
    }
    return NULL;
}

void reverse_book_iterator_reset(Iterator* self) {
    ReverseBookIterator* iter = (ReverseBookIterator*)self;
    iter->current_index = iter->library->count - 1;
}

void reverse_book_iterator_destroy(Iterator* self) {
    free(self);
}

Iterator* book_library_create_reverse_iterator(BookLibrary* library) {
    ReverseBookIterator* iter = malloc(sizeof(ReverseBookIterator));

    iter->base.has_next = reverse_book_iterator_has_next;
    iter->base.next = reverse_book_iterator_next;
    iter->base.reset = reverse_book_iterator_reset;
    iter->base.destroy = reverse_book_iterator_destroy;

    iter->library = library;
    iter->current_index = library->count - 1;

    return (Iterator*)iter;
}

// Filtered iterator (by year range)
typedef struct {
    Iterator base;
    BookLibrary* library;
    int current_index;
    int min_year;
    int max_year;
} FilteredBookIterator;

bool filtered_book_iterator_has_next(Iterator* self) {
    FilteredBookIterator* iter = (FilteredBookIterator*)self;

    // Find next book that matches the filter
    while (iter->current_index < iter->library->count) {
        Book* book = iter->library->books[iter->current_index];
        if (book->year >= iter->min_year && book->year <= iter->max_year) {
            return true;
        }
        iter->current_index++;
    }
    return false;
}

void* filtered_book_iterator_next(Iterator* self) {
    FilteredBookIterator* iter = (FilteredBookIterator*)self;

    while (iter->current_index < iter->library->count) {
        Book* book = iter->library->books[iter->current_index++];
        if (book->year >= iter->min_year && book->year <= iter->max_year) {
            return book;
        }
    }
    return NULL;
}

void filtered_book_iterator_reset(Iterator* self) {
    FilteredBookIterator* iter = (FilteredBookIterator*)self;
    iter->current_index = 0;
}

void filtered_book_iterator_destroy(Iterator* self) {
    free(self);
}

Iterator* book_library_create_filtered_iterator(BookLibrary* library, int min_year, int max_year) {
    FilteredBookIterator* iter = malloc(sizeof(FilteredBookIterator));

    iter->base.has_next = filtered_book_iterator_has_next;
    iter->base.next = filtered_book_iterator_next;
    iter->base.reset = filtered_book_iterator_reset;
    iter->base.destroy = filtered_book_iterator_destroy;

    iter->library = library;
    iter->current_index = 0;
    iter->min_year = min_year;
    iter->max_year = max_year;

    return (Iterator*)iter;
}

// Matrix example - 2D collection with different iteration strategies
typedef struct {
    int** data;
    int rows;
    int cols;
} Matrix;

Matrix* matrix_create(int rows, int cols) {
    Matrix* matrix = malloc(sizeof(Matrix));
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = malloc(rows * sizeof(int*));

    for (int i = 0; i < rows; i++) {
        matrix->data[i] = malloc(cols * sizeof(int));
        for (int j = 0; j < cols; j++) {
            matrix->data[i][j] = i * cols + j + 1; // Fill with sample data
        }
    }

    return matrix;
}

void matrix_destroy(Matrix* matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

void matrix_print(Matrix* matrix) {
    printf("Matrix %dx%d:\n", matrix->rows, matrix->cols);
    for (int i = 0; i < matrix->rows; i++) {
        printf("  ");
        for (int j = 0; j < matrix->cols; j++) {
            printf("%3d ", matrix->data[i][j]);
        }
        printf("\n");
    }
}

// Row-wise iterator for Matrix
typedef struct {
    Iterator base;
    Matrix* matrix;
    int row;
    int col;
} RowWiseIterator;

bool row_wise_iterator_has_next(Iterator* self) {
    RowWiseIterator* iter = (RowWiseIterator*)self;
    return iter->row < iter->matrix->rows;
}

void* row_wise_iterator_next(Iterator* self) {
    RowWiseIterator* iter = (RowWiseIterator*)self;

    if (iter->row < iter->matrix->rows) {
        int* value = &iter->matrix->data[iter->row][iter->col];

        iter->col++;
        if (iter->col >= iter->matrix->cols) {
            iter->col = 0;
            iter->row++;
        }

        return value;
    }
    return NULL;
}

void row_wise_iterator_reset(Iterator* self) {
    RowWiseIterator* iter = (RowWiseIterator*)self;
    iter->row = 0;
    iter->col = 0;
}

void row_wise_iterator_destroy(Iterator* self) {
    free(self);
}

Iterator* matrix_create_row_wise_iterator(Matrix* matrix) {
    RowWiseIterator* iter = malloc(sizeof(RowWiseIterator));

    iter->base.has_next = row_wise_iterator_has_next;
    iter->base.next = row_wise_iterator_next;
    iter->base.reset = row_wise_iterator_reset;
    iter->base.destroy = row_wise_iterator_destroy;

    iter->matrix = matrix;
    iter->row = 0;
    iter->col = 0;

    return (Iterator*)iter;
}

// Column-wise iterator for Matrix
typedef struct {
    Iterator base;
    Matrix* matrix;
    int row;
    int col;
} ColumnWiseIterator;

bool column_wise_iterator_has_next(Iterator* self) {
    ColumnWiseIterator* iter = (ColumnWiseIterator*)self;
    return iter->col < iter->matrix->cols;
}

void* column_wise_iterator_next(Iterator* self) {
    ColumnWiseIterator* iter = (ColumnWiseIterator*)self;

    if (iter->col < iter->matrix->cols) {
        int* value = &iter->matrix->data[iter->row][iter->col];

        iter->row++;
        if (iter->row >= iter->matrix->rows) {
            iter->row = 0;
            iter->col++;
        }

        return value;
    }
    return NULL;
}

void column_wise_iterator_reset(Iterator* self) {
    ColumnWiseIterator* iter = (ColumnWiseIterator*)self;
    iter->row = 0;
    iter->col = 0;
}

void column_wise_iterator_destroy(Iterator* self) {
    free(self);
}

Iterator* matrix_create_column_wise_iterator(Matrix* matrix) {
    ColumnWiseIterator* iter = malloc(sizeof(ColumnWiseIterator));

    iter->base.has_next = column_wise_iterator_has_next;
    iter->base.next = column_wise_iterator_next;
    iter->base.reset = column_wise_iterator_reset;
    iter->base.destroy = column_wise_iterator_destroy;

    iter->matrix = matrix;
    iter->row = 0;
    iter->col = 0;

    return (Iterator*)iter;
}

// Client code
int main() {
    printf("=== Iterator Pattern Demo - Book Library ===\n\n");

    // Create a book library
    Collection* library = book_library_create();

    // Add some books
    library->add(library, book_create("The Great Gatsby", "F. Scott Fitzgerald", 1925, 12.99));
    library->add(library, book_create("To Kill a Mockingbird", "Harper Lee", 1960, 14.50));
    library->add(library, book_create("1984", "George Orwell", 1949, 13.25));
    library->add(library, book_create("The Catcher in the Rye", "J.D. Salinger", 1951, 11.75));
    library->add(library, book_create("Lord of the Flies", "William Golding", 1954, 10.99));

    printf("Library contains %d books\n\n", library->size(library));

    // Forward iteration
    printf("=== Forward Iteration ===\n");
    Iterator* forward_iter = library->create_iterator(library);
    while (forward_iter->has_next(forward_iter)) {
        Book* book = (Book*)forward_iter->next(forward_iter);
        book_print(book);
    }
    printf("\n");

    // Reverse iteration
    printf("=== Reverse Iteration ===\n");
    BookLibrary* book_lib = (BookLibrary*)library;
    Iterator* reverse_iter = book_library_create_reverse_iterator(book_lib);
    while (reverse_iter->has_next(reverse_iter)) {
        Book* book = (Book*)reverse_iter->next(reverse_iter);
        book_print(book);
    }
    printf("\n");

    // Filtered iteration (books from 1940-1960)
    printf("=== Filtered Iteration (1940-1960) ===\n");
    Iterator* filtered_iter = book_library_create_filtered_iterator(book_lib, 1940, 1960);
    while (filtered_iter->has_next(filtered_iter)) {
        Book* book = (Book*)filtered_iter->next(filtered_iter);
        book_print(book);
    }
    printf("\n");

    // Matrix iteration example
    printf("=== Matrix Iteration Example ===\n");
    Matrix* matrix = matrix_create(3, 4);
    matrix_print(matrix);
    printf("\n");

    printf("Row-wise iteration:\n");
    Iterator* row_iter = matrix_create_row_wise_iterator(matrix);
    while (row_iter->has_next(row_iter)) {
        int* value = (int*)row_iter->next(row_iter);
        printf("%d ", *value);
    }
    printf("\n\n");

    printf("Column-wise iteration:\n");
    Iterator* col_iter = matrix_create_column_wise_iterator(matrix);
    while (col_iter->has_next(col_iter)) {
        int* value = (int*)col_iter->next(col_iter);
        printf("%d ", *value);
    }
    printf("\n\n");

    // Reset and iterate again
    printf("=== Testing Reset Functionality ===\n");
    printf("Forward iterator reset and iterate first 3 books:\n");
    forward_iter->reset(forward_iter);
    int count = 0;
    while (forward_iter->has_next(forward_iter) && count < 3) {
        Book* book = (Book*)forward_iter->next(forward_iter);
        book_print(book);
        count++;
    }

    // Cleanup
    forward_iter->destroy(forward_iter);
    reverse_iter->destroy(reverse_iter);
    filtered_iter->destroy(filtered_iter);
    row_iter->destroy(row_iter);
    col_iter->destroy(col_iter);

    library->destroy(library);
    matrix_destroy(matrix);

    printf("\n=== Iterator Pattern Benefits ===\n");
    printf("1. Uniform interface: Same way to traverse different collections\n");
    printf("2. Multiple iterators: Can have multiple traversal algorithms\n");
    printf("3. Encapsulation: Internal structure of collection is hidden\n");
    printf("4. Flexibility: Easy to add new iteration strategies\n");

    return 0;
}