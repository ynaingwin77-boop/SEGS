#include "SrcHeaders/Clock.h"

static volatile sig_atomic_t timerRunning = 1;

static void handleTimerExit(int sig) {
    timerRunning = 0;
}

/* ── runTimer ─────────────────────────────────────────────────────────────
   Countdown timer. Smooth update without screen flashing.                  */
void runTimer(int totalSeconds) {
    int remaining = totalSeconds;

    timerRunning = 1;
    signal(SIGINT, handleTimerExit);

    clearScreen();
    while (remaining >= 0 && timerRunning) {
        int h = remaining / 3600;
        int m = (remaining % 3600) / 60;
        int s = remaining % 60;

        printf("\033[H");
        printf("\n\n");
        printf(COLOR_CYAN "  ==============================\n");
        printf("         COUNTDOWN TIMER        \n");
        printf("  ==============================\n\n");
        printf("         %02d : %02d : %02d\n", h, m, s);
        printf("\n  ==============================\n");
        printf("  [Ctrl+C to cancel timer]     \n");
        printf(COLOR_RESET);

        if (remaining == 0) {
            printf(COLOR_YELLOW "\n  *** TIME IS UP! ***\n" COLOR_RESET);
            for (int i = 0; i < 5 && timerRunning; i++) {
                printf("\a");
                fflush(stdout);
                sleepOneSecond();
            }
            break;
        }

        remaining--;
        sleepOneSecond();
    }

    if (!timerRunning) {
        clearScreen();
        printf(COLOR_YELLOW "\n  Timer cancelled. Returning to clock...\n" COLOR_RESET);
        sleepOneSecond();
    }

    signal(SIGINT, SIG_DFL);
    timerRunning = 1;
}

/* ── runAlarm ─────────────────────────────────────────────────────────────
   Alarm clock. Smooth update matching the timer.                          */
void runAlarm(int targetHour, int targetMin, bool isPM, bool use12HourFormat) {

    timerRunning = 1;
    signal(SIGINT, handleTimerExit);

    int targetHour24 = targetHour;
    if (use12HourFormat) {
        if (isPM && targetHour < 12) {
            targetHour24 = targetHour + 12;
        } else if (!isPM && targetHour == 12) {
            targetHour24 = 0;
        } else if (isPM && targetHour == 12) {
            targetHour24 = 12;
        }
    }

    clearScreen();
    printf("\n\n");
    printf(COLOR_GREEN "  ==============================\n");
    printf("          ALARM CLOCK           \n");
    printf("  ==============================\n\n");
    
    if (use12HourFormat) {
        printf("   Alarm set for  %02d:%02d %s\n", targetHour, targetMin, isPM ? "PM" : "AM");
    } else {
        printf("   Alarm set for  %02d:%02d (24-hr)\n", targetHour24, targetMin);
    }
    
    printf("\n  ==============================\n" COLOR_RESET);
    sleepOneSecond();

    clearScreen();
    while (timerRunning) {
        time_t now = time(NULL);
        struct tm *t = localtime(&now);

        int currentDisplayHour = t->tm_hour;
        char *currentAMPM = "";
        
        if (use12HourFormat) {
            currentAMPM = (t->tm_hour >= 12) ? "PM" : "AM";
            currentDisplayHour = (t->tm_hour % 12 == 0) ? 12 : t->tm_hour % 12;
        }

        printf("\033[H");
        printf("\n\n");
        printf(COLOR_CYAN "  ==============================\n");
        printf("          ALARM CLOCK           \n");
        printf("  ==============================\n\n");
        
        if (use12HourFormat) {
            printf("   Current :  %02d:%02d:%02d %s\n",
                   currentDisplayHour, t->tm_min, t->tm_sec, currentAMPM);
            printf("   Alarm   :  %02d:%02d:00 %s\n",
                   targetHour, targetMin, isPM ? "PM" : "AM");
        } else {
            printf("   Current :  %02d:%02d:%02d\n",
                   t->tm_hour, t->tm_min, t->tm_sec);
            printf("   Alarm   :  %02d:%02d:00\n",
                   targetHour24, targetMin);
        }

        printf("\n  ==============================\n");
        printf("  [Ctrl+C to cancel alarm]     \n");
        printf(COLOR_RESET);

        if (t->tm_hour == targetHour24 && t->tm_min == targetMin) {
            printf(COLOR_YELLOW "\n  *** ALARM RINGING! ***\n" COLOR_RESET);
            for (int i = 0; i < 5 && timerRunning; i++) {
                printf("\a");
                fflush(stdout);
                sleepOneSecond();
            }
            break;
        }

        sleepOneSecond();
    }

    if (!timerRunning) {
        clearScreen();
        printf(COLOR_YELLOW "\n  Alarm cancelled. Returning to clock...\n" COLOR_RESET);
        sleepOneSecond();
    }

    signal(SIGINT, SIG_DFL);
    timerRunning = 1;
}