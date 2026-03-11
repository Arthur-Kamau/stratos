/**
 * Stratos WASM Stdlib Shims
 *
 * WASM-compatible implementations of essential stdlib functions.
 * These are compiled into the WASM module and provide basic functionality
 * when running in a browser/WASI environment.
 */

#include <cstring>
#include <cstdint>
#include <cstdlib>

extern "C" {

// ============================================================
// String operations
// ============================================================

int32_t __stratos_str_length(const char* str) {
    if (!str) return 0;
    return static_cast<int32_t>(strlen(str));
}

// Concatenate two strings, caller must free result
char* __stratos_str_concat(const char* a, const char* b) {
    if (!a && !b) return nullptr;
    if (!a) a = "";
    if (!b) b = "";
    size_t lenA = strlen(a);
    size_t lenB = strlen(b);
    char* result = (char*)malloc(lenA + lenB + 1);
    if (!result) return nullptr;
    memcpy(result, a, lenA);
    memcpy(result + lenA, b, lenB);
    result[lenA + lenB] = '\0';
    return result;
}

// Substring, caller must free result
char* __stratos_str_substring(const char* str, int32_t start, int32_t end) {
    if (!str) return nullptr;
    int32_t len = static_cast<int32_t>(strlen(str));
    if (start < 0) start = 0;
    if (end > len) end = len;
    if (start >= end) {
        char* empty = (char*)malloc(1);
        if (empty) empty[0] = '\0';
        return empty;
    }
    int32_t subLen = end - start;
    char* result = (char*)malloc(subLen + 1);
    if (!result) return nullptr;
    memcpy(result, str + start, subLen);
    result[subLen] = '\0';
    return result;
}

int32_t __stratos_str_index_of(const char* str, const char* substr) {
    if (!str || !substr) return -1;
    const char* found = strstr(str, substr);
    if (!found) return -1;
    return static_cast<int32_t>(found - str);
}

int32_t __stratos_str_equals(const char* a, const char* b) {
    if (a == b) return 1;
    if (!a || !b) return 0;
    return strcmp(a, b) == 0 ? 1 : 0;
}

// ============================================================
// Array operations (simple i32 arrays)
// ============================================================

// Create an array with initial capacity
int32_t* __stratos_array_create(int32_t capacity) {
    // Layout: [length, capacity, ...elements]
    int32_t* arr = (int32_t*)malloc((capacity + 2) * sizeof(int32_t));
    if (!arr) return nullptr;
    arr[0] = 0;          // length
    arr[1] = capacity;   // capacity
    return arr;
}

int32_t __stratos_array_length(int32_t* arr) {
    if (!arr) return 0;
    return arr[0];
}

void __stratos_array_push(int32_t* arr, int32_t value) {
    if (!arr) return;
    int32_t len = arr[0];
    int32_t cap = arr[1];
    if (len >= cap) return; // No realloc in simple shim
    arr[2 + len] = value;
    arr[0] = len + 1;
}

int32_t __stratos_array_pop(int32_t* arr) {
    if (!arr || arr[0] <= 0) return 0;
    arr[0]--;
    return arr[2 + arr[0]];
}

int32_t __stratos_array_get(int32_t* arr, int32_t index) {
    if (!arr || index < 0 || index >= arr[0]) return 0;
    return arr[2 + index];
}

void __stratos_array_set(int32_t* arr, int32_t index, int32_t value) {
    if (!arr || index < 0 || index >= arr[0]) return;
    arr[2 + index] = value;
}

// ============================================================
// Math operations (supplement to JS Math imports)
// ============================================================

int32_t __stratos_int_max(int32_t a, int32_t b) { return a > b ? a : b; }
int32_t __stratos_int_min(int32_t a, int32_t b) { return a < b ? a : b; }
int32_t __stratos_int_abs(int32_t a) { return a < 0 ? -a : a; }
int32_t __stratos_int_clamp(int32_t val, int32_t lo, int32_t hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

// ============================================================
// Memory helpers
// ============================================================

void __stratos_free(void* ptr) {
    free(ptr);
}

void* __stratos_memcpy(void* dest, const void* src, int32_t n) {
    return memcpy(dest, src, n);
}

void* __stratos_memset(void* ptr, int32_t value, int32_t n) {
    return memset(ptr, value, n);
}

} // extern "C"
