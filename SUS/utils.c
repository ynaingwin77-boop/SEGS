/*
 * utils.c — shared utility functions
 */

#include "SrcHeaders/sus.h"

/* Remove leading and trailing whitespace in-place */
void trim(char *s) {
    char *p = s;
    while (isspace((unsigned char)*p)) p++;
    if (p != s) memmove(s, p, strlen(p) + 1);
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) s[--len] = '\0';
}

/* Discard everything left in stdin up to newline */
void flush_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Prompt for an integer in [min, max] — keeps asking until valid */
int get_int(const char *prompt, int min, int max) {
    char buf[32];
    int  val;
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof(buf), stdin)) exit(0);
        trim(buf);
        /* check digits */
        char *p = buf;
        int ok = 1;
        if (*p == '-') p++;
        if (*p == '\0') ok = 0;
        for (; *p; p++) if (!isdigit((unsigned char)*p)) { ok = 0; break; }
        if (!ok) { printf("  Invalid! Enter a number between %d and %d.\n", min, max); continue; }
        val = atoi(buf);
        if (val < min || val > max) {
            printf("  Out of range! Enter a number between %d and %d.\n", min, max);
            continue;
        }
        return val;
    }
}

/* Prompt for a non-empty string, stores in buf */
void get_string(const char *prompt, char *buf, int size) {
    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, size, stdin)) exit(0);
        trim(buf);
        if (strlen(buf) > 0) return;
        printf("  Input cannot be empty.\n");
    }
}

/* Fill buf with today's date as YYYY-MM-DD */
void today_str(char *buf) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    snprintf(buf, DATE_LEN, "%04d-%02d-%02d",
             tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
}

/* Return number of days from date string d1 to d2 (d2 - d1) */
int days_between(const char *d1, const char *d2) {
    struct tm t1 = {0}, t2 = {0};
    sscanf(d1, "%d-%d-%d", &t1.tm_year, &t1.tm_mon, &t1.tm_mday);
    sscanf(d2, "%d-%d-%d", &t2.tm_year, &t2.tm_mon, &t2.tm_mday);
    t1.tm_year -= 1900; t1.tm_mon -= 1;
    t2.tm_year -= 1900; t2.tm_mon -= 1;
    t1.tm_isdst = -1; t2.tm_isdst = -1;
    double diff = difftime(mktime(&t2), mktime(&t1));
    return (int)(diff / 86400.0);
}

/* Wait for user to press Enter */
void press_enter(void) {
    printf("\n  Press Enter to continue...");
    flush_stdin();
}

/* Clear terminal screen */
void clear_screen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Print a styled section header */
void print_header(const char *title) {
    const char *icon = "📌";
    const char *color = C_CYAN;

    if (strstr(title, "STUDENT"))       { icon = "👨‍🎓"; color = C_BLUE; }
    else if (strstr(title, "BOOK"))     { icon = "📚";   color = C_MAGENTA; }
    else if (strstr(title, "ATTEND"))   { icon = "📋";   color = C_GREEN; }
    else if (strstr(title, "REPORT"))   { icon = "📊";   color = C_YELLOW; }
    else if (strstr(title, "ISSUE"))    { icon = "📖";   color = C_MAGENTA; }
    else if (strstr(title, "RETURN"))   { icon = "↩️";   color = C_GREEN; }
    else if (strstr(title, "ADD"))      { icon = "➕";   color = C_GREEN; }
    else if (strstr(title, "UPDATE"))   { icon = "✏️";   color = C_YELLOW; }
    else if (strstr(title, "DELETE"))   { icon = "🗑️";   color = C_RED; }

    printf("\n%s%s%s\n", color, DIV, C_RESET);
    int pad = (int)(60 - strlen(title)) / 2;
    if (pad < 0) pad = 0;
    printf("%s%s%s %s%s%s\n", color, C_BOLD, icon, title, C_RESET, "");
    printf("%s%s%s\n", color, DIV, C_RESET);
}