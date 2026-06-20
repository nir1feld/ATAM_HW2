#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define GRN "\e[0;32m"
#define RED "\e[0;31m"
#define RESET "\e[0m"

// The student's assembly function
extern unsigned short sum_of_text(char separator);

// Create the in.txt file expected by aux_HW2.o
void create_test_file(const char* content) {
    FILE *f = fopen("in.txt", "w");
    if (f == NULL) {
        printf(RED "Fatal TA Error: Could not create in.txt.\n" RESET);
        exit(1);
    }
    fprintf(f, "%s", content);
    fclose(f);
}

// Generate a massive garbage string to break hardcoded 50-byte stack buffers
void create_buffer_overflow_file() {
    FILE *f = fopen("in.txt", "w");
    if (f == NULL) exit(1);
    
    // 5000 characters of 'z' (invalid word, returns -1)
    for(int i = 0; i < 5000; i++) {
        fputc('z', f);
    }
    // Followed by a valid separator and a valid word (assuming 'one' = 1)
    fprintf(f, ",one\n");
    fclose(f);
}

// Global variables to store register states
uint64_t rbx_before, rbp_before, r12_before, r13_before, r14_before, r15_before;
uint64_t rbx_after, rbp_after, r12_after, r13_after, r14_after, r15_after;

// A wrapper to mercilessly check System V ABI callee-saved registers
unsigned short abi_checked_sum_of_text(char separator) {
    unsigned short result;
    
    // Seed registers with garbage to ensure the student's code restores them
    __asm__ volatile (
        "mov %%rbx, %[rbx_b]\n\t"
        "mov %%rbp, %[rbp_b]\n\t"
        "mov %%r12, %[r12_b]\n\t"
        "mov %%r13, %[r13_b]\n\t"
        "mov %%r14, %[r14_b]\n\t"
        "mov %%r15, %[r15_b]\n\t"
        // Poison the registers!
        "mov $0xDEADBEEF, %%rbx\n\t"
        "mov $0xCAFEBABE, %%r12\n\t"
        "mov $0x8BADF00D, %%r13\n\t"
        "mov $0x0DEFACED, %%r14\n\t"
        "mov $0xFEEDFACE, %%r15\n\t"
        : [rbx_b] "=m" (rbx_before), [rbp_b] "=m" (rbp_before), 
          [r12_b] "=m" (r12_before), [r13_b] "=m" (r13_before), 
          [r14_b] "=m" (r14_before), [r15_b] "=m" (r15_before)
        : // no inputs
        : "memory"
    );

    result = sum_of_text(separator);

    __asm__ volatile (
        "mov %%rbx, %[rbx_a]\n\t"
        "mov %%rbp, %[rbp_a]\n\t"
        "mov %%r12, %[r12_a]\n\t"
        "mov %%r13, %[r13_a]\n\t"
        "mov %%r14, %[r14_a]\n\t"
        "mov %%r15, %[r15_a]\n\t"
        : [rbx_a] "=m" (rbx_after), [rbp_a] "=m" (rbp_after), 
          [r12_a] "=m" (r12_after), [r13_a] "=m" (r13_after), 
          [r14_a] "=m" (r14_after), [r15_a] "=m" (r15_after)
        : // no inputs
        : "memory"
    );

    return result;
}

void run_test(const char* test_name, const char* content, char separator, unsigned short expected, int is_overflow_test) {
    if (is_overflow_test) {
        create_buffer_overflow_file();
    } else {
        create_test_file(content);
    }
    
    // Call the ABI-checking wrapper instead of the raw function
    unsigned short result = abi_checked_sum_of_text(separator);
    
    int abi_violation = (rbx_after != 0xDEADBEEF || r12_after != 0xCAFEBABE || 
                         r13_after != 0x8BADF00D || r14_after != 0x0DEFACED || 
                         r15_after != 0xFEEDFACE);

    if (abi_violation) {
        printf(RED "[FAIL - ABI VIOLATION]" RESET " %s\n", test_name);
        printf("\tYou did not restore Callee-Saved registers (System V ABI)!\n");
    } else if (result == expected) {
        printf(GRN "[PASS]" RESET " %s\n", test_name);
    } else {
        printf(RED "[FAIL]" RESET " %s\n", test_name);
        if (!is_overflow_test) {
            printf("\tContent:   '%s'\n", content);
        } else {
            printf("\tContent:   [5000 'z's],one\\n\n");
        }
        printf("\tSeparator: '%c'\n", separator);
        printf("\tExpected:  %hu\n", expected);
        printf("\tGot:       %hu\n", result);
    }
}

int main() {
    printf("====================================================\n");
    printf("  ATAM HW2 Part 1 - The 'No Mercy' Evaluation Suite\n");
    printf("====================================================\n\n");

    // 1. The False Sense of Security (Happy Path)
    run_test("Basic Sanity", "one,two,three", ',', 6, 0);

    // 2. The Missing EOF Newline Trap
    // Many students loop reading until '\n'. If EOF hits first, they lose the last word or infinite loop.
    run_test("EOF Without Newline Trap", "one,two,three", ',', 6, 0);

    // 3. The Separator / Newline Combo Confusion
    // Both newline and the separator are delimiters. 
    run_test("Newline and Separator Collision", "one\ntwo,three\nfour", ',', 10, 0);

    // 4. Pure Noise & Garbage Filtering
    // Verifying text_to_number's -1 return value is completely ignored and doesn't subtract from the sum.
    run_test("Pure Noise Filtering", "cat,dog\nbird-plane,five,seeveen", ',', 5, 0);

    // 5. The ASCII Edge-Case Separator
    // The separator can be any non a-z ASCII character. Testing with a tab character.
    run_test("Tab Separator", "one\ttwo\tthree", '\t', 6, 0);

    // 6. The "Stack Buffer Overflow" Deathblow
    // If they hardcoded a `sub $100, %rsp` for their string buffer and read blindly, this will segfault.
    run_test("Massive Buffer Overflow check", "", ',', 1, 1);

    // 7. Empty/Meaningless Sequences
    // What if the file is just one giant invalid word that hits EOF?
    run_test("Giant Invalid Word to EOF", "thisisasuperlongwordthatisntanumberandhasnoseparatorsornewlines", ',', 0, 0);

    // Clean up
    remove("in.txt");

    printf("\n====================================================\n");
    printf("  If you survived this without a segfault, well done.\n");
    printf("====================================================\n");

    return 0;
}