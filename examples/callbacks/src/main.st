package main;

fn main() {
    println("--- Callback Showcase ---");

    // 1. Simple callback
    fn apply(value: int, op: Function) int {
        return op(value);
    }

    val res1 = apply(10, (x) => x * 2);
    println("10 * 2 = " + res1);

    // 2. Closure capturing environment
    val factor = 5;
    val res2 = apply(3, (x) => x * factor);
    println("3 * 5 = " + res2);

    // 3. Returning a function (Higher-Order Function)
    fn makeAdder(n: int) Function {
        return (x) => x + n;
    }

    val add10 = makeAdder(10);
    println("5 + 10 = " + add10(5));

    // 4. Array transformation using callbacks
    println("--- Array Transformation ---");
    val numbers: Array<int> = [1, 2, 3, 4, 5];

    // Transform each element
    println("Original: 1, 2, 3, 4, 5");
    println("Doubled:");
    for val i in 0..numbers.length() {
        val doubled = (numbers[i]) * 2;
        println("  " + numbers[i] + " -> " + doubled);
    }

    // Using a callback for transformation
    fn processArray(arr: Array<int>, callback: Function) void {
        for val i in 0..arr.length() {
            val item = arr[i];
            callback(item);
        }
    }

    println("Squared using callback:");
    processArray(numbers, (n) => {
        println("  " + n + " -> " + (n * n));
    });

    println("--- Done ---");
}
