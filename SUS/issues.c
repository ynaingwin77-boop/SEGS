/*
 * issues.c — Book Issue and Return operations
 */

#include "SrcHeaders/sus.h"

/* ── Load issues from CSV ────────────────────────────────────────────────── */
int load_issues(Issue arr[], int *count) {
    *count = 0;
    FILE *fp = fopen(FILE_ISSUES, "r");
    if (!fp) return 0;
    char line[LINE_LEN];
    fgets(line, sizeof(line), fp);   /* skip header */
    while (fgets(line, sizeof(line), fp) && *count < MAX_ISSUES) {
        Issue is;
        if (sscanf(line, "%d,%d,%d,%11[^,],%11[^,],%d",
                   &is.issue_id, &is.student_id, &is.book_id,
                   is.issue_date, is.return_date, &is.returned) == 6) {
            trim(is.issue_date); trim(is.return_date);
            arr[(*count)++] = is;
        }
    }
    fclose(fp);
    return 1;
}

/* ── Save issues to CSV ──────────────────────────────────────────────────── */
int save_issues(Issue arr[], int count) {
    FILE *fp = fopen(FILE_ISSUES, "w");
    if (!fp) { printf("  ERROR: Cannot write %s\n", FILE_ISSUES); return 0; }
    fprintf(fp, "issue_id,student_id,book_id,issue_date,return_date,returned\n");
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d,%d,%d,%s,%s,%d\n",
                arr[i].issue_id, arr[i].student_id, arr[i].book_id,
                arr[i].issue_date, arr[i].return_date, arr[i].returned);
    fclose(fp);
    return 1;
}

/* ── Next issue ID ───────────────────────────────────────────────────────── */
int next_issue_id(Issue arr[], int count) {
    int max = 3000;
    for (int i = 0; i < count; i++)
        if (arr[i].issue_id >= max) max = arr[i].issue_id + 1;
    return max;
}

/* ── Issue a book to a student ───────────────────────────────────────────── */
void issue_book(Issue arr[], int *icount,
                Student sarr[], int scount,
                Book barr[], int *bcount) {
    print_header("ISSUE BOOK");

    int sid = get_int("  Enter Student ID: ", 1, 99999);
    Student *s = find_student_by_id(sarr, scount, sid);
    if (!s) { printf("  Student ID %d not found.\n", sid); return; }

    int bid = get_int("  Enter Book ID: ", 1, 99999);
    Book *b = find_book_by_id(barr, *bcount, bid);
    if (!b) { printf("  Book ID %d not found.\n", bid); return; }

    if (b->available_copies <= 0) {
        printf("  Sorry — '%s' is currently out of stock.\n", b->title);
        return;
    }

    /* Check student hasn't already issued this book and not returned */
    for (int i = 0; i < *icount; i++) {
        if (arr[i].student_id == sid &&
            arr[i].book_id   == bid &&
            arr[i].returned  == 0) {
            printf("  This student already has '%s' issued.\n", b->title);
            return;
        }
    }

    Issue is;
    is.issue_id    = next_issue_id(arr, *icount);
    is.student_id  = sid;
    is.book_id     = bid;
    today_str(is.issue_date);
    strcpy(is.return_date, "N/A");
    is.returned    = 0;

    arr[(*icount)++] = is;
    b->available_copies--;

    save_issues(arr, *icount);
    save_books(barr, *bcount);

    printf("\n  Book issued successfully!\n");
    printf("  Issue ID : %d\n", is.issue_id);
    printf("  Student  : %s\n", s->name);
    printf("  Book     : %s\n", b->title);
    printf("  Date     : %s\n", is.issue_date);
    printf("  Due back in %d days.\n", LOAN_DAYS);
}

/* ── Return a book ───────────────────────────────────────────────────────── */
void return_book(Issue arr[], int icount,
                 Book barr[], int bcount) {
    print_header("RETURN BOOK");

    int iid = get_int("  Enter Issue ID: ", 1, 99999);
    Issue *is = NULL;
    for (int i = 0; i < icount; i++) {
        if (arr[i].issue_id == iid) { is = &arr[i]; break; }
    }
    if (!is) { printf("  Issue ID %d not found.\n", iid); return; }
    if (is->returned) { printf("  This book has already been returned.\n"); return; }

    today_str(is->return_date);
    is->returned = 1;

    Book *b = find_book_by_id(barr, bcount, is->book_id);
    if (b) b->available_copies++;

    save_issues(arr, icount);
    save_books(barr, bcount);

    /* check overdue */
    int diff = days_between(is->issue_date, is->return_date);
    printf("\n  Book returned successfully!\n");
    printf("  Return Date : %s\n", is->return_date);
    if (diff > LOAN_DAYS)
        printf("  NOTE: Overdue by %d day(s)!\n", diff - LOAN_DAYS);
    else
        printf("  Returned on time. (%d/%d days used)\n", diff, LOAN_DAYS);
}

/* ── View all issues ─────────────────────────────────────────────────────── */
void view_all_issues(Issue arr[], int icount,
                     Student sarr[], int scount,
                     Book barr[], int bcount) {
    print_header("ALL ISSUE RECORDS");
    if (icount == 0) { printf("  No issue records found.\n"); return; }

    printf("  %-6s %-6s %-22s %-22s %-12s %-12s %s\n",
           "IssID", "StID", "Student", "Book", "IssDate", "RetDate", "Status");
    printf("  %s\n", DIV2);

    for (int i = 0; i < icount; i++) {
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        Book    *b = find_book_by_id(barr, bcount, arr[i].book_id);
        char sname[NAME_LEN]; strncpy(sname, s ? s->name  : "Unknown", NAME_LEN);
        char btitle[NAME_LEN]; strncpy(btitle, b ? b->title : "Unknown", NAME_LEN);
        /* truncate for display */
        if (strlen(sname)  > 20) { sname[19]  = '.'; sname[20]  = '\0'; }
        if (strlen(btitle) > 20) { btitle[19] = '.'; btitle[20] = '\0'; }

        printf("  %-6d %-6d %-22s %-22s %-12s %-12s %s\n",
               arr[i].issue_id, arr[i].student_id,
               sname, btitle,
               arr[i].issue_date, arr[i].return_date,
               arr[i].returned ? "Returned" : "Issued");
    }
    printf("  %s\n  Total: %d record(s)\n", DIV2, icount);
}