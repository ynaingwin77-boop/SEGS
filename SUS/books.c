/*
 * books.c — Book CRUD operations
 */

#include "SrcHeaders/sus.h"

/* ── Load books from CSV ─────────────────────────────────────────────────── */
int load_books(Book arr[], int *count) {
    *count = 0;
    FILE *fp = fopen(FILE_BOOKS, "r");
    if (!fp) return 0;
    char line[LINE_LEN];
    fgets(line, sizeof(line), fp);   /* skip header */
    while (fgets(line, sizeof(line), fp) && *count < MAX_BOOKS) {
        Book b;
        if (sscanf(line, "%d,%63[^,],%63[^,],%d,%d",
                   &b.id, b.title, b.author,
                   &b.total_copies, &b.available_copies) == 5) {
            trim(b.title); trim(b.author);
            arr[(*count)++] = b;
        }
    }
    fclose(fp);
    return 1;
}

/* ── Save books to CSV ───────────────────────────────────────────────────── */
int save_books(Book arr[], int count) {
    FILE *fp = fopen(FILE_BOOKS, "w");
    if (!fp) { printf("  ERROR: Cannot write %s\n", FILE_BOOKS); return 0; }
    fprintf(fp, "id,title,author,total_copies,available_copies\n");
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d,%s,%s,%d,%d\n",
                arr[i].id, arr[i].title, arr[i].author,
                arr[i].total_copies, arr[i].available_copies);
    fclose(fp);
    return 1;
}

/* ── Next book ID ────────────────────────────────────────────────────────── */
int next_book_id(Book arr[], int count) {
    int max = 2000;
    for (int i = 0; i < count; i++)
        if (arr[i].id >= max) max = arr[i].id + 1;
    return max;
}

/* ── Find book by ID ─────────────────────────────────────────────────────── */
Book *find_book_by_id(Book arr[], int count, int id) {
    for (int i = 0; i < count; i++)
        if (arr[i].id == id) return &arr[i];
    return NULL;
}

/* ── Add book ────────────────────────────────────────────────────────────── */
void add_book(Book arr[], int *count) {
    if (*count >= MAX_BOOKS) { printf("  Book list is full!\n"); return; }
    print_header("ADD NEW BOOK");
    Book b;
    b.id = next_book_id(arr, *count);
    get_string("  Title  : ", b.title,  NAME_LEN);
    get_string("  Author : ", b.author, NAME_LEN);
    b.total_copies     = get_int("  Total Copies : ", 1, 999);
    b.available_copies = b.total_copies;
    arr[(*count)++] = b;
    save_books(arr, *count);
    printf("\n  Book added successfully! ID: %d\n", b.id);
}

/* ── View all books ──────────────────────────────────────────────────────── */
void view_all_books(Book arr[], int count) {
    print_header("ALL BOOKS");
    if (count == 0) { printf("  No books found.\n"); return; }
    printf("  %-6s %-24s %-20s %6s %9s\n",
           "ID", "Title", "Author", "Total", "Available");
    printf("  %s\n", DIV2);
    for (int i = 0; i < count; i++) {
        printf("  %-6d %-24s %-20s %6d %9d",
               arr[i].id, arr[i].title, arr[i].author,
               arr[i].total_copies, arr[i].available_copies);
        if (arr[i].available_copies == 0) printf("  [OUT OF STOCK]");
        printf("\n");
    }
    printf("  %s\n  Total: %d book(s)\n", DIV2, count);
}

/* ── Update book ─────────────────────────────────────────────────────────── */
void update_book(Book arr[], int count) {
    print_header("UPDATE BOOK");
    int id = get_int("  Enter Book ID to update: ", 1, 99999);
    Book *b = find_book_by_id(arr, count, id);
    if (!b) { printf("  Book ID %d not found.\n", id); return; }
    printf("  Current — Title: %s | Author: %s | Copies: %d\n",
           b->title, b->author, b->total_copies);
    get_string("  New Title  : ", b->title,  NAME_LEN);
    get_string("  New Author : ", b->author, NAME_LEN);
    int new_total = get_int("  New Total Copies : ", 1, 999);
    int diff = new_total - b->total_copies;
    b->total_copies = new_total;
    b->available_copies += diff;
    if (b->available_copies < 0) b->available_copies = 0;
    save_books(arr, count);
    printf("  Book updated successfully!\n");
}

/* ── Delete book ─────────────────────────────────────────────────────────── */
void delete_book(Book arr[], int *count) {
    print_header("DELETE BOOK");
    int id = get_int("  Enter Book ID to delete: ", 1, 99999);
    for (int i = 0; i < *count; i++) {
        if (arr[i].id == id) {
            printf("  Delete '%s'? (y/n): ", arr[i].title);
            char c[4]; fgets(c, sizeof(c), stdin); trim(c);
            if (tolower(c[0]) != 'y') { printf("  Cancelled.\n"); return; }
            arr[i] = arr[--(*count)];
            save_books(arr, *count);
            printf("  Book deleted.\n");
            return;
        }
    }
    printf("  Book ID %d not found.\n", id);
}

/* ── Search book ─────────────────────────────────────────────────────────── */
void search_book(Book arr[], int count) {
    print_header("SEARCH BOOK");
    printf("  1. Search by ID\n  2. Search by Title\n  3. Search by Author\n");
    int opt = get_int("  Choice: ", 1, 3);
    int found = 0;

    if (opt == 1) {
        int id = get_int("  Enter Book ID: ", 1, 99999);
        for (int i = 0; i < count; i++) {
            if (arr[i].id == id) {
                printf("\n  ID: %d | Title: %s | Author: %s | Copies: %d/%d\n",
                       arr[i].id, arr[i].title, arr[i].author,
                       arr[i].available_copies, arr[i].total_copies);
                found++;
            }
        }
    } else {
        char kw[NAME_LEN];
        get_string(opt == 2 ? "  Enter Title (partial ok): "
                             : "  Enter Author (partial ok): ", kw, NAME_LEN);
        for (char *p = kw; *p; p++) *p = (char)tolower((unsigned char)*p);
        printf("\n  %-6s %-24s %-20s %6s %9s\n",
               "ID", "Title", "Author", "Total", "Avail");
        printf("  %s\n", DIV2);
        for (int i = 0; i < count; i++) {
            char lower[NAME_LEN];
            strncpy(lower, opt == 2 ? arr[i].title : arr[i].author, NAME_LEN);
            for (char *p = lower; *p; p++) *p = (char)tolower((unsigned char)*p);
            if (strstr(lower, kw)) {
                printf("  %-6d %-24s %-20s %6d %9d\n",
                       arr[i].id, arr[i].title, arr[i].author,
                       arr[i].total_copies, arr[i].available_copies);
                found++;
            }
        }
    }
    if (!found) printf("  No matching books found.\n");
}