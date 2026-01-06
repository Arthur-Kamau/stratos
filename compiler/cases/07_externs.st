// 07_externs.st
// Extern functions

extern fn printf(format: string, value: int);

fn main() {
    printf("Hello from extern %d\n", 123);
}
