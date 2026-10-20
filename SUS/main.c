/*
 * main.c — Student Utility System (SUS)
 * ─────────────────────────────────────────────────────────────────────────────
 * Entry point. Loads all CSV data on startup, renders menus,
 * dispatches to student / book / issue / attendance / report modules.
 *
 * File layout:
 *   main.c          — menus + entry point
 *   students.c      — student CRUD
 *   books.c         — book CRUD
 *   issues.c        — issue / return
 *   attendance.c    — attendance CRUD
 *   reports.c       — 6 reports
 *   utils.c         — shared helpers
 *   SrcHeaders/sus.h— structs, constants, prototypes
 *   data/           — CSV files (auto-created)
 *
 * Compile:
 *   gcc main.c students.c books.c issues.c attendance.c reports.c utils.c \
 *       -I./SrcHeaders -o sus
 * Run:
 *   ./sus
 * ─────────────────────────────────────────────────────────────────────────────
 */

#include "SrcHeaders/sus.h"

/* ── In-memory data stores ───────────────────────────────────────────────── */
static Student    students[MAX_STUDENTS];
static Book       books[MAX_BOOKS];
static Issue      issues[MAX_ISSUES];
static Attendance attend[MAX_ATTEND];

static int s_count = 0, b_count = 0, i_count = 0, a_count = 0;

/* ── Ensure data/ directory exists ──────────────────────────────────────── */
static void ensure_data_dir(void) {
#ifdef _WIN32
    system("if not exist data mkdir data");
#else
    system("mkdir -p data");
#endif
}

/* ════════════════════════════════════════════════════════════════════════════
 *  SUB-MENUS
 * ════════════════════════════════════════════════════════════════════════════ */

/* ── Student menu ────────────────────────────────────────────────────────── */
static void menu_students(void) {
    while (1) {
        print_header("STUDENT MANAGEMENT");
        printf("  " C_GREEN "1. ➕ Add Student" C_RESET "\n");
        printf("  " C_CYAN  "2. 👥 View All Students" C_RESET "\n");
        printf("  " C_YELLOW"3. ✏️  Update Student" C_RESET "\n");
        printf("  " C_RED   "4. 🗑️  Delete Student" C_RESET "\n");
        printf("  " C_BLUE  "5. 🔎 Search Student (by ID or Name)" C_RESET "\n");
        printf("  " C_WHITE "0. ↩️  Back to Main Menu" C_RESET "\n");
        int ch = get_int("\n  Choice: ", 0, 5);
        switch (ch) {
            case 1: add_student(students, &s_count);      break;
            case 2: view_all_students(students, s_count); break;
            case 3: update_student(students, s_count);    break;
            case 4: delete_student(students, &s_count);   break;
            case 5: search_student(students, s_count);    break;
            case 0: return;
        }
        press_enter();
    }
}

/* ── Book menu ───────────────────────────────────────────────────────────── */
static void menu_books(void) {
    while (1) {
        print_header("BOOK MANAGEMENT");
        printf("  " C_GREEN "1. ➕ Add Book" C_RESET "\n");
        printf("  " C_CYAN  "2. 📚 View All Books" C_RESET "\n");
        printf("  " C_YELLOW"3. ✏️  Update Book" C_RESET "\n");
        printf("  " C_RED   "4. 🗑️  Delete Book" C_RESET "\n");
        printf("  " C_BLUE  "5. 🔎 Search Book (by ID, Title, or Author)" C_RESET "\n");
        printf("  " C_WHITE "0. ↩️  Back to Main Menu" C_RESET "\n");
        int ch = get_int("\n  Choice: ", 0, 5);
        switch (ch) {
            case 1: add_book(books, &b_count);      break;
            case 2: view_all_books(books, b_count); break;
            case 3: update_book(books, b_count);    break;
            case 4: delete_book(books, &b_count);   break;
            case 5: search_book(books, b_count);    break;
            case 0: return;
        }
        press_enter();
    }
}

/* ── Issue / Return menu ─────────────────────────────────────────────────── */
static void menu_issues(void) {
    while (1) {
        print_header("BOOK ISSUE & RETURN");
        printf("  " C_MAGENTA "1. 📖 Issue Book to Student" C_RESET "\n");
        printf("  " C_GREEN   "2. ↩️  Return Book" C_RESET "\n");
        printf("  " C_CYAN    "3. 🧾 View All Issue Records" C_RESET "\n");
        printf("  " C_WHITE   "0. ↩️  Back to Main Menu" C_RESET "\n");
        int ch = get_int("\n  Choice: ", 0, 3);
        switch (ch) {
            case 1:
                issue_book(issues, &i_count,
                           students, s_count,
                           books, &b_count);
                break;
            case 2:
                return_book(issues, i_count, books, b_count);
                break;
            case 3:
                view_all_issues(issues, i_count,
                                students, s_count,
                                books, b_count);
                break;
            case 0: return;
        }
        press_enter();
    }
}

/* ── Attendance menu ─────────────────────────────────────────────────────── */
static void menu_attendance(void) {
    while (1) {
        print_header("ATTENDANCE MANAGEMENT");
        printf("  " C_GREEN "1. ➕ Add Attendance Record" C_RESET "\n");
        printf("  " C_CYAN  "2. 📋 View All Records" C_RESET "\n");
        printf("  " C_YELLOW"3. ✏️  Update Record" C_RESET "\n");
        printf("  " C_RED   "4. 🗑️  Delete Record" C_RESET "\n");
        printf("  " C_WHITE "0. ↩️  Back to Main Menu" C_RESET "\n");
        int ch = get_int("\n  Choice: ", 0, 4);
        switch (ch) {
            case 1:
                add_attendance(attend, &a_count, students, s_count);
                break;
            case 2:
                view_attendance(attend, a_count, students, s_count);
                break;
            case 3:
                update_attendance(attend, a_count);
                break;
            case 4:
                delete_attendance(attend, &a_count);
                break;
            case 0: return;
        }
        press_enter();
    }
}

/* ── Reports menu ────────────────────────────────────────────────────────── */
static void menu_reports(void) {
    while (1) {
        print_header("REPORTS");
        printf("\n  " C_MAGENTA C_BOLD "📚 BOOK REPORTS" C_RESET "\n");
        printf("  " C_CYAN   "1. 📖 Currently Issued Books" C_RESET "\n");
        printf("  " C_GREEN  "2. ↩️  Returned Books" C_RESET "\n");
        printf("  " C_RED    "3. ⚠️  Overdue Books" C_RESET "\n");
        printf("\n  " C_GREEN C_BOLD "📋 ATTENDANCE REPORTS" C_RESET "\n");
        printf("  " C_RED    "4. 📉 Low Attendance   (below 75%%)" C_RESET "\n");
        printf("  " C_CYAN   "5. 📈 High Attendance  (75%% - 99%%)" C_RESET "\n");
        printf("  " C_GREEN  "6. ⭐ Perfect Attendance (100%%)" C_RESET "\n");
        printf("\n  " C_WHITE "0. ↩️  Back to Main Menu" C_RESET "\n");

        int ch = get_int("\n  Choice: ", 0, 6);
        switch (ch) {
            case 1:
                report_issued_books(issues, i_count,
                                    students, s_count,
                                    books, b_count);
                break;
            case 2:
                report_returned_books(issues, i_count,
                                      students, s_count,
                                      books, b_count);
                break;
            case 3:
                report_overdue_books(issues, i_count,
                                     students, s_count,
                                     books, b_count);
                break;
            case 4:
                report_attendance_low(attend, a_count,
                                      students, s_count);
                break;
            case 5:
                report_attendance_high(attend, a_count,
                                       students, s_count);
                break;
            case 6:
                report_attendance_perfect(attend, a_count,
                                          students, s_count);
                break;
            case 0: return;
        }
        press_enter();
    }
}

/* ════════════════════════════════════════════════════════════════════════════
 *  MAIN
 * ════════════════════════════════════════════════════════════════════════════ */
int main(void) {
    ensure_data_dir();

    load_students(students, &s_count);
    load_books(books, &b_count);
    load_issues(issues, &i_count);
    load_attendance(attend, &a_count);

    while (1) {
        clear_screen();
        printf("\n%s\n", DIV);
        printf("         " C_CYAN C_BOLD "🎓 STUDENT UTILITY SYSTEM (SUS)" C_RESET "\n");
        printf("         " C_MAGENTA "📚 Library & " C_GREEN "📋 Attendance Management" C_RESET "\n");
        printf("%s\n", DIV);
        printf("\n  " C_CYAN C_BOLD "🏠 MAIN MENU" C_RESET "\n");
        printf("  %s\n", DIV2);
        printf("  " C_BLUE    "1. 👨‍🎓 Student Management" C_RESET "\n");
        printf("  " C_MAGENTA "2. 📚 Book Management" C_RESET "\n");
        printf("  " C_MAGENTA "3. 📖 Book Issue & Return" C_RESET "\n");
        printf("  " C_GREEN   "4. 📋 Attendance Management" C_RESET "\n");
        printf("  " C_YELLOW  "5. 📊 Reports" C_RESET "\n");
        printf("  " C_RED     "0. 🚪 Exit" C_RESET "\n");
        printf("  %s\n", DIV2);

        int ch = get_int("\n  Choice: ", 0, 5);
        switch (ch) {
            case 1: menu_students();   break;
            case 2: menu_books();      break;
            case 3: menu_issues();     break;
            case 4: menu_attendance(); break;
            case 5: menu_reports();    break;
            case 0:
                printf("\n  Thank you for using the Student Utility System!\n\n");
                return 0;
        }
    }
}