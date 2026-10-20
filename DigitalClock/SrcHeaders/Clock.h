#ifndef CLOCK_H
#define CLOCK_H

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

/* Cross-platform library imports */
#ifdef _WIN32
    #include <windows.h>
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
#else
    #include <unistd.h>
#endif

// ANSI Color Codes
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

// Function prototypes
void initConsole(void);
void clearScreen(void);
void sleepOneSecond(void);
void sleepMilliseconds(int milliseconds); // Added missing prototype
void displayLargeClock(bool use12HourFormat);
void promptTimer(void);
void promptAlarm(bool use12HourFormat);
void runTimer(int seconds);
void runAlarm(int targetHour, int targetMin, bool isPM, bool use12HourFormat);

#endif // CLOCK_H