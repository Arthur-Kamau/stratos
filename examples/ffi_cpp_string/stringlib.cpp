/* stringlib.cpp - C++ library for FFI testing (with C ABI exports) */

#include <string>
#include <algorithm>
#include <cctype>
#include <cstring>

// Use extern "C" to prevent C++ name mangling
extern "C" {

// String length
int str_length(const char* str) {
    if (!str) return 0;
    return std::strlen(str);
}

// Convert to uppercase (returns static buffer - simplified)
const char* str_to_upper(const char* str) {
    static std::string result;
    result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result.c_str();
}

// Convert to lowercase
const char* str_to_lower(const char* str) {
    static std::string result;
    result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result.c_str();
}

// Reverse a string
const char* str_reverse(const char* str) {
    static std::string result;
    result = str;
    std::reverse(result.begin(), result.end());
    return result.c_str();
}

// Count vowels in a string
int str_count_vowels(const char* str) {
    if (!str) return 0;

    int count = 0;
    for (int i = 0; str[i]; i++) {
        char c = std::tolower(str[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
            count++;
        }
    }
    return count;
}

// Check if string is palindrome
int str_is_palindrome(const char* str) {
    if (!str) return 0;

    std::string s(str);
    // Remove spaces and convert to lowercase
    s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);

    std::string reversed = s;
    std::reverse(reversed.begin(), reversed.end());

    return s == reversed ? 1 : 0;
}

// Repeat a string n times
const char* str_repeat(const char* str, int n) {
    static std::string result;
    result.clear();
    for (int i = 0; i < n; i++) {
        result += str;
    }
    return result.c_str();
}

} // extern "C"
