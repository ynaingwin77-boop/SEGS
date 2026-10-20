/*
 * attendance.c — Student Attendance CRUD
 */

#include "SrcHeaders/sus.h"

/* ── Attendance percentage ───────────────────────────────────────────────── */
double attendance_pct(int total, int missed) {
    if (total <= 0) return 0.0;
    return ((double)(total - missed) / total) * 100.0;
}

/* ── Load attendance from CSV ────────────────────────────────────────────── */
int load_attendance(Attendance arr[], int *count) {
    *count = 0;
    FILE *fp = fopen(FILE_ATTENDANCE, "r");
    if (!fp) return 0;
    char line[LINE_LEN];
    fgets(line, sizeof(line), fp);   /* skip header */
    while (fgets(line, sizeof(line), fp) && *count < MAX_ATTEND) {
        Attendance a;
        if (sscanf(line, "%d,%d,%d,%63[^,],%d,%d",
                   &a.record_id, &a.student_id, &a.subject_id,
                   a.subject_name, &a.total_periods, &a.missed_periods) == 6) {
            trim(a.subject_name);
            arr[(*count)++] = a;
        }
    }
    fclose(fp);
    return 1;
}

/* ── Save attendance to CSV ──────────────────────────────────────────────── */
int save_attendance(Attendance arr[], int count) {
    FILE *fp = fopen(FILE_ATTENDANCE, "w");
    if (!fp) { printf("  ERROR: Cannot write %s\n", FILE_ATTENDANCE); return 0; }
    fprintf(fp, "record_id,student_id,subject_id,subject_name,total_periods,missed_periods\n");
    for (int i = 0; i < count; i++)
        fprintf(fp, "%d,%d,%d,%s,%d,%d\n",
                arr[i].record_id, arr[i].student_id, arr[i].subject_id,
                arr[i].subject_name, arr[i].total_periods, arr[i].missed_periods);
    fclose(fp);
    return 1;
}

/* ── Add attendance record ───────────────────────────────────────────────── */
void add_attendance(Attendance arr[], int *count,
                    Student sarr[], int scount) {
    print_header("ADD ATTENDANCE RECORD");

    int sid = get_int("  Enter Student ID: ", 1, 99999);
    if (!find_student_by_id(sarr, scount, sid)) {
        printf("  Student ID %d not found.\n", sid); return;
    }

    Attendance a;
    /* find next record_id */
    a.record_id = 4000;
    for (int i = 0; i < *count; i++)
        if (arr[i].record_id >= a.record_id) a.record_id = arr[i].record_id + 1;

    a.student_id = sid;
    a.subject_id = get_int("  Subject ID (any number): ", 1, 9999);
    get_string("  Subject Name : ", a.subject_name, NAME_LEN);
    a.total_periods = get_int("  Total Periods : ", 1, 999);

    while (1) {
        a.missed_periods = get_int("  Missed Periods: ", 0, 9999);
        if (a.missed_periods > a.total_periods) {
            printf("  Missed cannot exceed total (%d).\n", a.total_periods);
        } else break;
    }

    arr[(*count)++] = a;
    save_attendance(arr, *count);

    double pct = attendance_pct(a.total_periods, a.missed_periods);
    printf("\n  " C_GREEN "✅ Record saved! Attendance: %.2f%% — %s\n",
           pct, pct >= PASS_THRESHOLD ? "PASS" : "FAIL");
}

/* ── View attendance ─────────────────────────────────────────────────────── */
void view_attendance(Attendance arr[], int count,
                     Student sarr[], int scount) {
    print_header("ATTENDANCE RECORDS");
    if (count == 0) { printf("  No attendance records found.\n"); return; }

    printf("  %-6s %-6s %-22s %-18s %6s %6s %8s %6s\n",
           "RecID", "StID", "Student", "Subject",
           "Total", "Missed", "Attend%", "Status");
    printf("  %s\n", DIV2);

    for (int i = 0; i < count; i++) {
        Student *s = find_student_by_id(sarr, scount, arr[i].student_id);
        double pct = attendance_pct(arr[i].total_periods, arr[i].missed_periods);
        printf("  %-6d %-6d %-22s %-18s %6d %6d %7.1f%% %s\n",
               arr[i].record_id,
               arr[i].student_id,
               s ? s->name : "Unknown",
               arr[i].subject_name,
               arr[i].total_periods,
               arr[i].missed_periods,
               pct,
               pct >= PASS_THRESHOLD ? "PASS" : "FAIL");
    }
    printf("  %s\n  Total records: %d\n", DIV2, count);
}

/* ── Update attendance ───────────────────────────────────────────────────── */
void update_attendance(Attendance arr[], int count) {
    print_header("UPDATE ATTENDANCE");
    int rid = get_int("  Enter Record ID to update: ", 1, 99999);
    for (int i = 0; i < count; i++) {
        if (arr[i].record_id == rid) {
            printf("  Current — Subject: %s | Total: %d | Missed: %d\n",
                   arr[i].subject_name, arr[i].total_periods, arr[i].missed_periods);
            get_string("  New Subject Name : ", arr[i].subject_name, NAME_LEN);
            arr[i].total_periods  = get_int("  New Total Periods : ", 1, 999);
            while (1) {
                arr[i].missed_periods = get_int("  New Missed Periods: ", 0, 9999);
                if (arr[i].missed_periods <= arr[i].total_periods) break;
                printf("  Missed cannot exceed total (%d).\n", arr[i].total_periods);
            }
            save_attendance(arr, count);
            printf("  " C_YELLOW "✏️  Record updated!\n");
            return;
        }
    }
    printf("  Record ID %d not found.\n", rid);
}

/* ── Delete attendance ───────────────────────────────────────────────────── */
void delete_attendance(Attendance arr[], int *count) {
    print_header("DELETE ATTENDANCE RECORD");
    int rid = get_int("  Enter Record ID to delete: ", 1, 99999);
    for (int i = 0; i < *count; i++) {
        if (arr[i].record_id == rid) {
            printf("  Delete record for subject '%s'? (y/n): ",
                   arr[i].subject_name);
            char c[4]; fgets(c, sizeof(c), stdin); trim(c);
            if (tolower(c[0]) != 'y') { printf("  Cancelled.\n"); return; }
            arr[i] = arr[--(*count)];
            save_attendance(arr, *count);
            printf("  " C_RED "🗑️  Record deleted.\n");
            return;
        }
    }
    printf("  Record ID %d not found.\n", rid);
}