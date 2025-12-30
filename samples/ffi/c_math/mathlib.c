/* mathlib.c - Simple C library for FFI testing */

#include <stdio.h>

/* Add two integers */
int add(int a, int b) {
    return a + b;
}

/* Subtract two integers */
int subtract(int a, int b) {
    return a - b;
}

/* Multiply two integers */
int multiply(int a, int b) {
    return a * b;
}

/* Divide two integers */
int divide(int a, int b) {
    if (b == 0) {
        return 0;  // Avoid division by zero
    }
    return a / b;
}

/* Add two doubles */
double add_double(double a, double b) {
    return a + b;
}

/* Print a greeting message */
void greet(const char* name) {
    printf("Hello, %s from C!\n", name);
}

/* Print a message with a number */
void print_number(const char* label, int number) {
    printf("%s: %d\n", label, number);
}

/* Get the library version */
const char* get_version() {
    return "mathlib 1.0.0";
}

/* Factorial function */
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

/* Power function (integer exponent) */
int power(int base, int exp) {
    int result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}
