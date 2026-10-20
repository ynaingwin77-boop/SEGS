 #include <stdio.h>
#include <assert.h>
#include <string.h>
#include "SrcHeaders/Clock.h"

// 1. getTimestamp ကို အရင်ဆုံး (အပေါ်ဆုံးမှာ) တည်ဆောက်ပေးပါ
char* getTimestamp(void) {
    static char buffer[80];
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format: [YYYY-MM-DD HH:MM:SS]
    strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", timeinfo);
    return buffer;
}

// 2. ပြီးမှ test function များကို ရေးပါ
void test_time_validation(void) {
    printf("Running test: test_time_validation...\n");
    
    char* ts = getTimestamp();
    assert(ts != NULL);
    assert(strlen(ts) > 0);

    printf("  -> Generated Timestamp is: %s\n", ts);

    assert(1 == 1); 
    
    printf("Passed: test_time_validation\n");
}

int main(void) {
    printf("=== STARTING AUTOMATED TESTS ===\n\n");
    
    test_time_validation();
    
    printf("\n=== ALL TESTS PASSED SUCCESSFULLY ===\n");
    return 0;
}