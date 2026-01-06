#include <cstdlib>
#include <iostream>

extern "C" {

// Simple wrapper around malloc for Stratos
void* stratos_alloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        std::cerr << "Out of memory" << std::endl;
        exit(1);
    }
    return ptr;
}

// Simple wrapper around free for Stratos
void stratos_free(void* ptr) {
    if (ptr) {
        free(ptr);
    }
}

} // extern "C"
