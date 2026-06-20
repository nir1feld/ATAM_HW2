#include <stdio.h>
#include <string.h>

// Your assembly function
unsigned short sum_of_text(char separator);

// Helper function to write to in.txt, run the assembly, and check the result
void run_test(int test_num, const char* test_name, const char* content, char separator, unsigned short expected) {
    // 1. Write the test content to in.txt
    FILE *f = fopen("in.txt", "w");
    if (f == NULL) {
        printf("Error: Could not open in.txt for writing.\n");
        return;
    }
    fputs(content, f);
    fclose(f);

    // 2. Call your assembly function
    unsigned short result = sum_of_text(separator);

    // 3. Print the results
    if (result == expected) {
        printf("[PASS] Test %d: %s\n", test_num, test_name);
    } else {
        printf("[FAIL] Test %d: %s\n", test_num, test_name);
        printf("       Expected: %hu, but got: %hu\n\n", expected, result);
    }
}

int main() {
    printf("--- Starting Assembly Tests ---\n\n");

    // Test 1: The original provided test (Linux line endings \n)
    run_test(1, "Original Baseline Test", 
             "two,three,four,one,one,eight\nis,our,number\ni,love,atam", 
             ',', 19);

    // Test 2: Different separator (Space)
    run_test(2, "Space Separator", 
             "one two cat three dog four", 
             ' ', 10);

    // Test 3: Different separator (Dash) & consecutive separators
    run_test(3, "Dash Separator & Consecutive Separators", 
             "five--six-\n-seven\n\neight-", 
             '-', 26);

    // Test 4: Windows style line endings (\r\n)
    run_test(4, "Windows Carriage Returns (\\r\\n)", 
             "one,\r\ntwo,\r\nthree", 
             ',', 6);

    // Test 5: Only invalid words
    run_test(5, "Only Invalid Words", 
             "cat,dog,fish,technion,atam", 
             ',', 0);

    // Test 6: Empty File
    run_test(6, "Empty File", 
             "", 
             ',', 0);

    // Test 7: Single valid word (No separators or newlines at all)
    run_test(7, "Single Valid Word (No EOF newline)", 
             "nine", 
             ',', 9);

    // Test 8: Mix of numbers, text, and weird spacing
    run_test(8, "Messy formatting", 
             "one;two;cat;three\nfour;\r\nfive;;six", 
             ';', 21);

    printf("\n--- Tests Complete ---\n");
    return 0;
}