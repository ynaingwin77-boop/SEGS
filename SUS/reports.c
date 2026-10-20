/*
 * reports.c — Report generation functions
 * Reports included:
 *   1. Currently issued books
 *   2. Returned books
 *   3. Overdue books
 *   4. Low attendance   (< 75%)
 *   5. High attendance  (75% - 99%)
 *   6. Perfect attendance (100%)
 */

#include "SrcHeaders/sus.h"

/* ── Report: currently issued (not yet returned) books ───────────────────── */
void report_issued_books(Issue arr[], int icount,
                          Student sarr[], int scount,
                          Book barr[], int bcount) {
    print_header("REPORT: CURRENTLY ISSUED BOOKS");
    int found = 0;
    char today[DATE_LEN]; today_str(today);

    printf("  %-6s %-22s %-22s %-12s %s\n",
           "IssID", "Student", "Book", "IssDate", "Days Out");
    printf("  %s\n", DIV2);

    for (int i = 0; i < icount; i++) {
        if (arr[i].returned) continue;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        Book    *b = find_book_by_id(barr, bcount, arr[i].book_id);
        int days = days_between(arr[i].issue_date, today);
        printf("  %-6d %-22s %-22s %-12s %d day(s)\n",
               arr[i].issue_id,
               s ? s->name  : "Unknown",
               b ? b->title : "Unknown",
               arr[i].issue_date, days);
        found++;
    }
    if (!found) printf("  No books currently issued.\n");
    else printf("  %s\n  Total: %d issued book(s)\n", DIV2, found);
}

/* ── Report: returned books ──────────────────────────────────────────────── */
void report_returned_books(Issue arr[], int icount,
                            Student sarr[], int scount,
                            Book barr[], int bcount) {
    print_header("REPORT: RETURNED BOOKS");
    int found = 0;
    printf("  %-6s %-22s %-22s %-12s %-12s\n",
           "IssID", "Student", "Book", "IssDate", "RetDate");
    printf("  %s\n", DIV2);
    for (int i = 0; i < icount; i++) {
        if (!arr[i].returned) continue;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        Book    *b = find_book_by_id(barr, bcount, arr[i].book_id);
        printf("  %-6d %-22s %-22s %-12s %-12s\n",
               arr[i].issue_id,
               s ? s->name  : "Unknown",
               b ? b->title : "Unknown",
               arr[i].issue_date, arr[i].return_date);
        found++;
    }
    if (!found) printf("  No returned books found.\n");
    else printf("  %s\n  Total: %d return(s)\n", DIV2, found);
}

/* ── Report: overdue books ───────────────────────────────────────────────── */
void report_overdue_books(Issue arr[], int icount,
                           Student sarr[], int scount,
                           Book barr[], int bcount) {
    print_header("REPORT: OVERDUE BOOKS");
    int found = 0;
    char today[DATE_LEN]; today_str(today);

    printf("  %-6s %-22s %-22s %-12s %s\n",
           "IssID", "Student", "Book", "IssDate", "Overdue By");
    printf("  %s\n", DIV2);

    for (int i = 0; i < icount; i++) {
        if (arr[i].returned) continue;
        int days = days_between(arr[i].issue_date, today);
        if (days <= LOAN_DAYS) continue;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        Book    *b = find_book_by_id(barr, bcount, arr[i].book_id);
        printf("  %-6d %-22s %-22s %-12s %d day(s)\n",
               arr[i].issue_id,
               s ? s->name  : "Unknown",
               b ? b->title : "Unknown",
               arr[i].issue_date, days - LOAN_DAYS);
        found++;
    }
    if (!found) printf("  No overdue books.\n");
    else printf("  %s\n  Total: %d overdue book(s)\n", DIV2, found);
}

/* ── Report: low attendance (> 0% and < 75%) ─────────────────────────────── */
void report_attendance_low(Attendance arr[], int acount,
                            Student sarr[], int scount) {
    print_header("REPORT: LOW ATTENDANCE (below 75%)");
    int found = 0;
    printf("  %-6s %-22s %-18s %8s %10s\n",
           "StID", "Student", "Subject", "Attend%", "Shortfall");
    printf("  %s\n", DIV2);
    for (int i = 0; i < acount; i++) {
        double pct = attendance_pct(arr[i].total_periods, arr[i].missed_periods);
        if (pct >= PASS_THRESHOLD || pct == 0.0) continue;
        double max_abs   = arr[i].total_periods * (ABSENT_LIMIT / 100.0);
        double shortfall = arr[i].missed_periods - max_abs;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        printf("  %-6d %-22s %-18s %7.1f%% +%.1f periods over limit\n",
               arr[i].student_id, s ? s->name : "Unknown",
               arr[i].subject_name, pct, shortfall);
        found++;
    }
    if (!found) printf("  No students with low attendance.\n");
    else printf("  %s\n  Total: %d record(s)\n", DIV2, found);
}

/* ── Report: high attendance (>= 75% and < 100%) ────────────────────────── */
void report_attendance_high(Attendance arr[], int acount,
                             Student sarr[], int scount) {
    print_header("REPORT: HIGH ATTENDANCE (75% - 99%)");
    int found = 0;
    printf("  %-6s %-22s %-18s %8s %12s\n",
           "StID", "Student", "Subject", "Attend%", "Can Still Miss");
    printf("  %s\n", DIV2);
    for (int i = 0; i < acount; i++) {
        double pct = attendance_pct(arr[i].total_periods, arr[i].missed_periods);
        if (pct < PASS_THRESHOLD || pct >= 100.0) continue;
        double max_abs   = arr[i].total_periods * (ABSENT_LIMIT / 100.0);
        double remaining = max_abs - arr[i].missed_periods;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        printf("  %-6d %-22s %-18s %7.1f%% %d period(s)\n",
               arr[i].student_id, s ? s->name : "Unknown",
               arr[i].subject_name, pct, (int)remaining);
        found++;
    }
    if (!found) printf("  No students in this range.\n");
    else printf("  %s\n  Total: %d record(s)\n", DIV2, found);
}

/* ── Report: perfect attendance (100%) ───────────────────────────────────── */
void report_attendance_perfect(Attendance arr[], int acount,
                                Student sarr[], int scount) {
    print_header("REPORT: PERFECT ATTENDANCE (100%)");
    int found = 0;
    printf("  %-6s %-22s %-18s %8s\n",
           "StID", "Student", "Subject", "Attend%");
    printf("  %s\n", DIV2);
    for (int i = 0; i < acount; i++) {
        double pct = attendance_pct(arr[i].total_periods, arr[i].missed_periods);
        if (pct < 100.0) continue;
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        printf("  %-6d %-22s %-18s  100.0%%\n",
               arr[i].student_id, s ? s->name : "Unknown",
               arr[i].subject_name);
        found++;
    }
    if (!found) printf("  No students with perfect attendance.\n");
    else printf("  %s\n  Total: %d record(s)\n", DIV2, found);
}