/*
 * sus.h — Student Utility System
 * ─────────────────────────────────────────────────────────────────────────────
 * Shared header: structs, constants, and all function prototypes.
 * Included by every .c file in the project.
 * ─────────────────────────────────────────────────────────────────────────────
 */

#ifndef SUS_H
#define SUS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ── File paths ─────────────────────────────────────────────────────────── */
#define FILE_STUDENTS   "data/students.csv"
#define FILE_BOOKS      "data/books.csv"
#define FILE_ISSUES     "data/issues.csv"
#define FILE_ATTENDANCE "data/attendance.csv"

/* ── Limits ─────────────────────────────────────────────────────────────── */
#define MAX_STUDENTS    500
#define MAX_BOOKS       500
#define MAX_ISSUES      1000
#define MAX_ATTEND      5000
#define NAME_LEN        64
#define DATE_LEN        32   /* YYYY-MM-DD\0 */
#define LINE_LEN        256

/* ── Attendance rule ────────────────────────────────────────────────────── */
#define PASS_THRESHOLD  75.0   /* minimum attendance % to pass */
#define ABSENT_LIMIT    25.0   /* maximum absence % allowed    */

/* ── Overdue threshold ──────────────────────────────────────────────────── */
#define LOAN_DAYS       14     /* days before a book is overdue */

/* ── UI helpers ─────────────────────────────────────────────────────────── */
#define DIV  "============================================================"
#define DIV2 "------------------------------------------------------------"

/* ANSI colors (supported by modern macOS/Linux/Windows terminals) */
#define C_RESET ""
#define C_BOLD ""
#define C_CYAN ""
#define C_BLUE ""
#define C_GREEN ""
#define C_YELLOW ""
#define C_MAGENTA ""
#define C_RED ""
#define C_WHITE ""

/* ════════════════════════════════════════════════════════════════════════════
 * STRUCTS
 * ════════════════════════════════════════════════════════════════════════════ */

typedef struct {
    int  id;
    char name[NAME_LEN];
    char course[NAME_LEN];
    int  year;               /* year level 1-4 */
} Student;

typedef struct {
    int  id;
    char title[NAME_LEN];
    char author[NAME_LEN];
    int  total_copies;
    int  available_copies;
} Book;

typedef struct {
    int  issue_id;
    int  student_id;
    int  book_id;
    char issue_date[DATE_LEN];
    char return_date[DATE_LEN];  /* "N/A" if not yet returned */
    int  returned;               /* 0 = not returned, 1 = returned */
} Issue;

typedef struct {
    int  record_id;
    int  student_id;
    int  subject_id;            /* simple integer tag for a subject */
    char subject_name[NAME_LEN];
    int  total_periods;
    int  missed_periods;
} Attendance;

/* ════════════════════════════════════════════════════════════════════════════
 * FUNCTION PROTOTYPES
 * ════════════════════════════════════════════════════════════════════════════ */

/* ── utils.c ─────────────────────────────────────────────────────────────── */
void  trim(char *s);
void  flush_stdin(void);
int   get_int(const char *prompt, int min, int max);
void  get_string(const char *prompt, char *buf, int size);
void  today_str(char *buf);          /* fills buf with today as YYYY-MM-DD */
int   days_between(const char *d1, const char *d2); /* d2 - d1 in days    */
void  press_enter(void);
void  clear_screen(void);
void  print_header(const char *title);

/* ── students.c ──────────────────────────────────────────────────────────── */
int   load_students(Student arr[], int *count);
int   save_students(Student arr[], int count);
void  add_student(Student arr[], int *count);
void  view_all_students(Student arr[], int count);
void  update_student(Student arr[], int count);
void  delete_student(Student arr[], int *count);
void  search_student(Student arr[], int count);
Student *find_student_by_id(Student arr[], int count, int id);
int   next_student_id(Student arr[], int count);

/* ── books.c ─────────────────────────────────────────────────────────────── */
int   load_books(Book arr[], int *count);
int   save_books(Book arr[], int count);
void  add_book(Book arr[], int *count);
void  view_all_books(Book arr[], int count);
void  update_book(Book arr[], int count);
void  delete_book(Book arr[], int *count);
void  search_book(Book arr[], int count);
Book *find_book_by_id(Book arr[], int count, int id);
int   next_book_id(Book arr[], int count);

/* ── issues.c ────────────────────────────────────────────────────────────── */
int   load_issues(Issue arr[], int *count);
int   save_issues(Issue arr[], int count);
void  issue_book(Issue arr[], int *icount,
                 Student sarr[], int scount,
                 Book barr[], int *bcount);
void  return_book(Issue arr[], int icount,
                  Book barr[], int bcount);
void  view_all_issues(Issue arr[], int icount,
                      Student sarr[], int scount,
                      Book barr[], int bcount);
int   next_issue_id(Issue arr[], int count);

/* ── attendance.c ────────────────────────────────────────────────────────── */
int   load_attendance(Attendance arr[], int *count);
int   save_attendance(Attendance arr[], int count);
void  add_attendance(Attendance arr[], int *count,
                     Student sarr[], int scount);
void  view_attendance(Attendance arr[], int count,
                      Student sarr[], int scount);
void  update_attendance(Attendance arr[], int count);
void  delete_attendance(Attendance arr[], int *count);
double attendance_pct(int total, int missed);

/* ── reports.c ───────────────────────────────────────────────────────────── */
void  report_issued_books(Issue arr[], int icount,
                           Student sarr[], int scount,
                           Book barr[], int bcount);
void  report_returned_books(Issue arr[], int icount,
                             Student sarr[], int scount,
                             Book barr[], int bcount);
void  report_overdue_books(Issue arr[], int icount,
                            Student sarr[], int scount,
                            Book barr[], int bcount);
void  report_attendance_low(Attendance arr[], int acount,
                             Student sarr[], int scount);
void  report_attendance_high(Attendance arr[], int acount,
                             Student sarr[], int scount);
void  report_attendance_perfect(Attendance arr[], int acount,
                                 Student sarr[], int scount);

/* ── main.c ──────────────────────────────────────────────────────────────── */
/* entry point — menus defined there */

#endif /* SUS_H */