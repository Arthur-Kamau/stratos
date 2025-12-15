// 03_functions.st
// Function declarations and Pipes

fn add(a: int, b: int) int {
    return a + b;
}

fn greet(name: string) {
    print("Hello " + name);
}

// Named parameters
// greet(name="Alice");

// Pipe Operator
// Result of first is passed as first arg to next
fn square(x: int) int { return x * x; }
fn makeDouble(x: int) int { return x * 2; }

val result = 5 |> square() |> makeDouble(); 
// Equivalent to: makeDouble(square(5)) -> 50
