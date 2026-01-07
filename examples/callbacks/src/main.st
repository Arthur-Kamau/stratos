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

    // 4. Map implementation using callbacks
    fn map(arr: Array<int>, transform: Function) Array<int> {
        val result = []; 
        
        for val i in 0..arr.length() {
            val item = arr[i];
            val transformed = transform(item);
            result.push(transformed);
        }
        return result;
    }
    
    println("--- Done ---");
}
