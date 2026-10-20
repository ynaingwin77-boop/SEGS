#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

/* Returns current timestamp as a string e.g. [14:32:07] */
const char* getTimestamp(void) {
    static char buf[12];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    snprintf(buf, sizeof(buf), "[%02d:%02d:%02d]",
             t->tm_hour, t->tm_min, t->tm_sec);
    return buf;
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);

    int prime = 97;
    int lim = (int)sqrt((double)prime);
    bool isPrime = true;

    printf("%s Input number: %d\n", getTimestamp(), prime);

    for (int i = 2; i <= lim; i++) {   /* <= to include sqrt itself */
        if (prime % i == 0) {
            isPrime = false;
            break;
        }
    }

    if (isPrime) {
        printf("%s %d is a prime number\n", getTimestamp(), prime);
    } else {
        printf("%s %d is NOT a prime number\n", getTimestamp(), prime);
    }

    return 0;
}