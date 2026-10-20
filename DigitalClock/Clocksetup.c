#include "SrcHeaders/Clock.h"

/* ── ASCII art digits 0-9, plus colon at index 10 ────────────────────── */
const char* ascii_digits[11][5] = {
    {" ### ", "#   #", "#   #", "#   #", " ### "}, /* 0 */
    {"  #  ", " ##  ", "  #  ", "  #  ", " ### "}, /* 1 */
    {" ### ", "    #", " ### ", "#    ", " ### "}, /* 2 */
    {" ### ", "    #", " ### ", "    #", " ### "}, /* 3 */
    {"#   #", "#   #", " ### ", "    #", "    #"}, /* 4 */
    {" ### ", "#    ", " ### ", "    #", " ### "}, /* 5 */
    {" ### ", "#    ", " ### ", "#   #", " ### "}, /* 6 */
    {" ### ", "    #", "   # ", "  #  ", "  #  "}, /* 7 */
    {" ### ", "#   #", " ### ", "#   #", " ### "}, /* 8 */
    {" ### ", "#   #", " ### ", "    #", " ### "}, /* 9 */
    {"     ", "  #  ", "     ", "  #  ", "     "}  /* 10 = colon */
};

/* ── initConsole ─────────────────────────────────────────────────────── */
void initConsole(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif
}

/* ── clearScreen ─────────────────────────────────────────────────────── */
void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ── sleepOneSecond ──────────────────────────────────────────────────── */
void sleepOneSecond(void) {
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif
}

/* ── sleepMilliseconds ───────────────────────────────────────────────── */
void sleepMilliseconds(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

/* ── displayLargeClock ───────────────────────────────────────────────── */
void displayLargeClock(bool use12HourFormat) {
    time_t rawtime;
    struct tm *currentTime;
    char dateBuffer[80];

    time(&rawtime);
    currentTime = localtime(&rawtime);

    int hour = currentTime->tm_hour;
    int min  = currentTime->tm_min;
    int sec  = currentTime->tm_sec;
    char am_pm[3] = "  ";

    if (use12HourFormat) {
        strcpy(am_pm, (hour >= 12) ? "PM" : "AM");
        if (hour == 0)  hour = 12;
        if (hour > 12)  hour -= 12;
    }

    int h1    = hour / 10;
    int h2    = hour % 10;
    int m1    = min  / 10;
    int m2    = min  % 10;
    int s1    = sec  / 10;
    int s2    = sec  % 10;
    int colon = 10;

    strftime(dateBuffer, sizeof(dateBuffer), "%A, %B %d, %Y", currentTime);

    printf("\n\n");
    printf(COLOR_GREEN "  ======================================================\n");
    printf("  |               DIGITAL CLOCK SIMULATOR              |\n");
    printf("  ======================================================\n\n" COLOR_RESET);

    for (int line = 0; line < 5; line++) {
        printf(COLOR_CYAN "      %s  %s  %s  %s  %s  %s  %s  %s\n" COLOR_RESET,
               ascii_digits[h1][line], ascii_digits[h2][line],
               ascii_digits[colon][line],
               ascii_digits[m1][line], ascii_digits[m2][line],
               ascii_digits[colon][line],
               ascii_digits[s1][line], ascii_digits[s2][line]);
    }

    printf(COLOR_YELLOW "\n      %-20s %25s\n" COLOR_RESET,
           use12HourFormat ? am_pm : "24HR", dateBuffer);

    printf(COLOR_GREEN "\n  ======================================================\n" COLOR_RESET);
    printf("  t = toggle 12/24hr  |  a = set alarm  |  s = set timer  |  q = quit\n");
}

/* ── promptTimer ─────────────────────────────────────────────────────────
   Called when user presses 's' — interactive timer prompt                */
void promptTimer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear input buffer

    while (1) {
        clearScreen();
        printf("\n\n");
        printf(COLOR_CYAN "  ==============================\n");
        printf("          SET TIMER             \n");
        printf("  ==============================\n\n" COLOR_RESET);
        printf("  Enter seconds (e.g. 90) or 'q' to cancel: ");
        fflush(stdout);

        char inputBuffer[50];
        if (scanf("%49s", inputBuffer) != 1) {
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // Allow user to cancel back to main clock
        if (inputBuffer[0] == 'q' || inputBuffer[0] == 'Q') {
            return;
        }

        int seconds = 0;
        if (sscanf(inputBuffer, "%d", &seconds) == 1 && seconds > 0) {
            runTimer(seconds);
            break;
        } else {
            printf(COLOR_YELLOW "\n  Invalid input! Please enter a positive number.\n" COLOR_RESET);
            printf("  Press Enter to try again...");
            fflush(stdout);
            while ((c = getchar()) != '\n' && c != EOF);
            getchar(); // Wait for user to press Enter before retrying
        }
    }
}

/* ── promptAlarm ─────────────────────────────────────────────────────────
   Called when user presses 'a' — interactive alarm prompt                */
/* ── promptAlarm ─────────────────────────────────────────────────────────
   Interactive alarm prompt supporting "8:30 PM" or "14:30" in one go       */
void promptAlarm(bool use12HourFormat) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Clear input buffer

    while (1) {
        clearScreen();
        printf("\n\n");
        printf(COLOR_CYAN "  ==============================\n");
        printf("          SET ALARM             \n");
        printf("  ==============================\n\n" COLOR_RESET);
        printf("  Enter alarm time (e.g. 8:30 PM/AM or 14:30) or 'q' to cancel: ");
        fflush(stdout);

        char inputBuffer[100];
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) {
            continue;
        }
        inputBuffer[strcspn(inputBuffer, "\r\n")] = 0; // Remove newline

        // Allow user to cancel back to main clock
        if (inputBuffer[0] == 'q' || inputBuffer[0] == 'Q') {
            return;
        }

        int h = 0, m = 0;
        char ampm[10] = "";
        int scanned = sscanf(inputBuffer, "%d:%d %9s", &h, &m, ampm);

        if (scanned < 2) {
            printf(COLOR_YELLOW "\n  Invalid format! Please use HH:MM or HH:MM AM/PM.\n" COLOR_RESET);
            printf("  Press Enter to try again...");
            fflush(stdout);
            getchar();
            continue;
        }

        bool is12Hour = use12HourFormat;
        bool isPM = false;

        // If user explicitly typed AM/PM (e.g. 8:30 PM)
        if (scanned == 3) {
            is12Hour = true;
            if (ampm[0] == 'P' || ampm[0] == 'p') {
                isPM = true;
            } else {
                isPM = false;
            }
        } else {
            // If no AM/PM provided, check hour range to decide format
            if (h > 12) {
                is12Hour = false; // 24-hour format
            }
        }

        // Validation
        if (is12Hour) {
            if (h < 1 || h > 12 || m < 0 || m > 59) {
                printf(COLOR_YELLOW "\n  Invalid time! Hour must be 1-12, Minute 0-59.\n" COLOR_RESET);
                printf("  Press Enter to try again...");
                fflush(stdout);
                getchar();
                continue;
            }
        } else {
            if (h < 0 || h > 23 || m < 0 || m > 59) {
                printf(COLOR_YELLOW "\n  Invalid time! Hour must be 0-23, Minute 0-59.\n" COLOR_RESET);
                printf("  Press Enter to try again...");
                fflush(stdout);
                getchar();
                continue;
            }
        }

        // Success! Run the alarm with correct parameters
        runAlarm(h, m, isPM, is12Hour);
        break;
    }
}