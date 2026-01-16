package io;

// Native C bindings
// 'any' maps to i8* (void*) in our IR, which works for printf varargs (mostly)
// or we can define specific overloads if we supported them.
fn printf(format: string, value: any);
fn putchar(char: int);

// Writes the string to the standard output.
fn print(message: string) {
    printf("%s", message);
}

// Writes the string to the standard output, followed by a newline.
fn println(message: string) {
    printf("%s\n", message);
}

// Simplified readLine (requires more complex buffer handling in C usually)
// For now, placeholder or wrapping simple gets?
//extern fn gets(buffer: string) any;