#include "SrcHeaders/Clock.h"

volatile sig_atomic_t keepRunning = 1;
volatile sig_atomic_t use12HourFormat = 0; 

void handleExit(int sig) { keepRunning = 0; }

#ifdef _WIN32
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    
    int kbhit_unix(void) {
        struct termios oldt, newt;
        int ch;
        int oldf;
        
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
        
        ch = getchar();
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, oldf);
        
        if (ch != EOF) {
            ungetc(ch, stdin);
            return 1;
        }
        return 0;
    }
    
    int getch_unix(void) {
        return getchar();
    }
#endif

#ifndef _WIN32
    void handleToggle(int sig) { use12HourFormat = !use12HourFormat; }
#endif

int main(void) {
    initConsole();
    signal(SIGINT, handleExit); 

#ifndef _WIN32
    signal(SIGTSTP, handleToggle); 
#endif

    while (keepRunning) {
        clearScreen();
        displayLargeClock(use12HourFormat);
        
#ifdef _WIN32
        if (_kbhit()) {
            int key = _getch();
            if (key == 'f' || key == 'F' || key == 't' || key == 'T') {
                use12HourFormat = !use12HourFormat;
            } else if (key == 'a' || key == 'A') {
                promptAlarm(use12HourFormat);
            } else if (key == 's' || key == 'S') {
                promptTimer();
            } else if (key == 'q' || key == 'Q') {
                keepRunning = 0;
            }
        }
#else
        if (kbhit_unix()) {
            int key = getch_unix();
            if (key == 'f' || key == 'F' || key == 't' || key == 'T') {
                use12HourFormat = !use12HourFormat;
            } else if (key == 'a' || key == 'A') {
                promptAlarm(use12HourFormat);
            } else if (key == 's' || key == 'S') {
                promptTimer();
            } else if (key == 'q' || key == 'Q') {
                keepRunning = 0;
            }
        }
#endif
        sleepMilliseconds(1000); 
    }
    
    clearScreen();
    printf("Clock safely terminated.Goodbye!\n" COLOR_RESET);
    return 0;
}