/*
 * students.c — Student CRUD operations
 */

#include "SrcHeaders/sus.h"

/* ── Load students from CSV ──────────────────────────────────────────────── */
int load_students(Student arr[], int *count) {
    *count = 0;
    FILE *fp = fopen(FILE_STUDENTS, "r");
    if (!fp) return 0;   /* file may not exist yet */
    char line[LINE_LEN];
    fgets(line, sizeof(line), fp);   /* skip header */
    while (fgets(line, sizeof(line), fp) && *count < MAX_STUDENTS) {
        Student s;
        if (sscanf(line, "%d,%63[^,],%63[^,],%d",
                   &s.id, s.name, s.course, &s.year) == 4) {
            trim(s.name); trim(s.course);
            arr[(*count)++] = s;
        }
    }
    fclose(fp);
    return 1;
}

/* ── Save students to CSV ────────────────────────────────────────────────── */
int save_students(Student arr[], int count) {
    FILE *fp = fopen(FILE_STUDENTS, "w");
    if (!fp) { printf("  ERROR: Cannot write %s\n", FILE_STUDENTS); return 0; }
    fprintf(fp, "id,name,course,year\n");
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d,%s,%s,%d\n",
                arr[i].id, arr[i].name, arr[i].course, arr[i].year);
    fclose(fp);
    return 1;
}

/* ── Next available student ID ───────────────────────────────────────────── */
int next_student_id(Student arr[], int count) {
    int max = 1000;
    for (int i = 0; i < count; i++)
        if (arr[i].id >= max) max = arr[i].id + 1;
    return max;
}

/* ── Find student by ID ──────────────────────────────────────────────────── */
Student *find_student_by_id(Student arr[], int count, int id) {
    for (int i = 0; i < count; i++)
        if (arr[i].id == id) return &arr[i];
    return NULL;
}

/* ── Add student ─────────────────────────────────────────────────────────── */
void add_student(Student arr[], int *count) {
    if (*count >= MAX_STUDENTS) {
        printf("  Student list is full!\n"); return;
    }
    print_header("ADD NEW STUDENT");
    Student s;
    s.id = next_student_id(arr, *count);
    get_string("  Name   : ", s.name,   NAME_LEN);
    get_string("  Course : ", s.course, NAME_LEN);
    s.year = get_int("  Year (1-4) : ", 1, 4);
    arr[(*count)++] = s;
    save_students(arr, *count);
    printf("\n  Student added successfully! ID: %d\n", s.id);
}

/* ── View all students ───────────────────────────────────────────────────── */
void view_all_students(Student arr[], int count) {
    print_header("ALL STUDENTS");
    if (count == 0) { printf("  No students found.\n"); return; }
    printf("  %-6s %-22s %-18s %s\n", "ID", "Name", "Course", "Year");
    printf("  %s\n", DIV2);
    for (int i = 0; i < count; i++)
        printf("  %-6d %-22s %-18s %d\n",
               arr[i].id, arr[i].name, arr[i].course, arr[i].year);
    printf("  %s\n  Total: %d student(s)\n", DIV2, count);
}

/* ── Update student ──────────────────────────────────────────────────────── */
void update_student(Student arr[], int count) {
    print_header("UPDATE STUDENT");
    int id = get_int("  Enter Student ID to update: ", 1, 99999);
    Student *s = find_student_by_id(arr, count, id);
    if (!s) { printf("  Student ID %d not found.\n", id); return; }
    printf("  Current — Name: %s | Course: %s | Year: %d\n",
           s->name, s->course, s->year);
    get_string("  New Name   (Enter to keep): ", s->name,   NAME_LEN);
    get_string("  New Course (Enter to keep): ", s->course, NAME_LEN);
    s->year = get_int("  New Year (1-4) : ", 1, 4);
    save_students(arr, count);
    printf("  Student updated successfully!\n");
}

/* ── Delete student ──────────────────────────────────────────────────────── */
void delete_student(Student arr[], int *count) {
    print_header("DELETE STUDENT");
    int id = get_int("  Enter Student ID to delete: ", 1, 99999);
    for (int i = 0; i < *count; i++) {
        if (arr[i].id == id) {
            printf("  Delete '%s'? (y/n): ", arr[i].name);
            char c[4]; fgets(c, sizeof(c), stdin); trim(c);
            if (tolower(c[0]) != 'y') { printf("  Cancelled.\n"); return; }
            arr[i] = arr[--(*count)];
            save_students(arr, *count);
            printf("  Student deleted.\n");
            return;
        }
    }
    printf("  Student ID %d not found.\n", id);
}

/* ── Search student ──────────────────────────────────────────────────────── */
void search_student(Student arr[], int count) {
    print_header("SEARCH STUDENT");
    printf("  1. Search by ID\n  2. Search by Name\n");
    int opt = get_int("  Choice: ", 1, 2);
    int found = 0;

    if (opt == 1) {
        int id = get_int("  Enter Student ID: ", 1, 99999);
        for (int i = 0; i < count; i++) {
            if (arr[i].id == id) {
                printf("\n  ID: %d | Name: %s | Course: %s | Year: %d\n",
                       arr[i].id, arr[i].name, arr[i].course, arr[i].year);
                found++;
            }
        }
    } else {
        char kw[NAME_LEN];
        get_string("  Enter Name (partial ok): ", kw, NAME_LEN);
        /* lowercase keyword */
        for (char *p = kw; *p; p++) *p = (char)tolower((unsigned char)*p);
        printf("\n  %-6s %-22s %-18s %s\n", "ID", "Name", "Course", "Year");
        printf("  %s\n", DIV2);
        for (int i = 0; i < count; i++) {
            char lower[NAME_LEN];
            strncpy(lower, arr[i].name, NAME_LEN);
            for (char *p = lower; *p; p++) *p = (char)tolower((unsigned char)*p);
            if (strstr(lower, kw)) {
                printf("  %-6d %-22s %-18s %d\n",
                       arr[i].id, arr[i].name, arr[i].course, arr[i].year);
                found++;
            }
        }
    }
    if (!found) printf("  No matching students found.\n");
}