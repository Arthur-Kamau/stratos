export interface TutorialStep {
    id: number;
    title: string;
    content: string;
    initialCode: string;
}

export const tutorialSteps: TutorialStep[] = [
    {
        id: 1,
        title: "Introduction",
        content: `
            <h3 class="text-lg font-semibold mb-2">Welcome to Stratos!</h3>
            <p class="mb-4">Stratos is a modern, statically-typed programming language designed for readability and tooling.</p>
            <p class="mb-4">Every Stratos file must start with a <code>package</code> declaration. In this example, we use <code>package main;</code>.</p>
            <p class="mb-4">The entry point of the program is the <code>main</code> function.</p>
        `,
        initialCode: `package main;

fn main() {
    println("Hello from Stratos!");
}`
    },
    {
        id: 2,
        title: "Syntax Fundamentals",
        content: `
            <h3 class="text-lg font-semibold mb-2">Basic Syntax Rules</h3>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><strong>Semicolons:</strong> Every statement must end with a semicolon (<code>;</code>). This is mandatory!</li>
                <li><strong>Comments:</strong> Use <code>//</code> for single-line and <code>/* */</code> for multi-line comments.</li>
                <li><strong>Blocks:</strong> Braces <code>{ }</code> are required for all blocks (functions, loops, if-statements).</li>
            </ul>
            <p class="mb-4">Try removing a semicolon and see what happens!</p>
        `,
        initialCode: `package main;

fn main() {
    // This is a comment
    val x = 10;
    val y = 20;

    println("Sum: " + (x + y));
}`
    },
    {
        id: 3,
        title: "Data Types & Casting",
        content: `
            <h3 class="text-lg font-semibold mb-2">Primitive Types</h3>
            <p class="mb-4">Stratos has standard primitive types:</p>
            <ul class="list-disc list-inside ml-2 mb-4">
                <li><code>int</code>: Integers (e.g., 42)</li>
                <li><code>double</code>: Floating-point (e.g., 3.14)</li>
                <li><code>string</code>: Text (e.g., "Hello")</li>
                <li><code>bool</code>: Boolean (true/false)</li>
            </ul>
            <h3 class="text-lg font-semibold mb-2">Type Casting</h3>
            <ul class="list-disc list-inside ml-2 mb-4">
                <li><strong>Forced Cast (<code>as</code>):</strong> Use when you are sure. Throws error if invalid.</li>
                <li><strong>Safe Cast (<code>as?</code>):</strong> Use when it might fail. Returns <code>void</code> (empty) on failure.</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val pi: double = 3.14159;
    
    // Forced cast (truncates)
    val rounded = pi as int;
    println("Rounded: " + rounded);

    // Safe cast (success)
    val num = "123" as? int;
    println("Parsed: " + num);

    // Safe cast (failure)
    val invalid = "abc" as? int;
    println("Invalid result: '" + invalid + "' (should be empty)");
}`
    },
    {
        id: 4,
        title: "Variables & Scope",
        content: `
            <h3 class="text-lg font-semibold mb-2">Variables</h3>
            <p class="mb-4">Stratos encourages immutability.</p>
            <ul class="list-disc list-inside ml-2 mb-4">
                <li>Use <code>val</code> for immutable variables (cannot change).</li>
                <li>Use <code>var</code> for mutable variables (can change).</li>
            </ul>
            <p class="mb-4">Types are often inferred, so you don't always need to write <code>: int</code>.</p>
        `,
        initialCode: `package main;

fn main() {
    // Immutable (preferred)
    val name = "Stratos"; 
    // name = "Other"; // This would be an error!

    // Mutable
    var count = 0;
    count = count + 1;

    println(name + " count: " + count);
}`
    },
    {
        id: 5,
        title: "Control Flow",
        content: `
            <h3 class="text-lg font-semibold mb-2">Control Flow</h3>
            <p class="mb-4">Standard <code>if/else</code>, <code>while</code>, and <code>for</code> loops are supported.</p>
            <p class="mb-4">The <strong><code>when</code></strong> expression is a powerful way to handle multiple conditions (like a switch statement).</p>
        `,
        initialCode: `package main;

fn main() {
    val x = 5;

    // If expression
    val status = if (x > 0) "Positive" else "Non-positive";
    println("Status: " + status);

    // Loop
    println("Counting:");
    for (i in 1..=3) {
        println(i);
    }

    // When expression
    val message = when (x) {
        1 -> "One"
        5 -> "Five"
        else -> "Other"
    };
    println("Match: " + message);
}`
    },
    {
        id: 6,
        title: "Functions",
        content: `
            <h3 class="text-lg font-semibold mb-2">Functions</h3>
            <p class="mb-4">Defined with the <code>fn</code> keyword.</p>
            <p class="mb-4"><strong>Pipe Operator (<code>|></code>):</strong> Chain function calls for better readability. <code>x |> f()</code> is the same as <code>f(x)</code>.</p>
        `,
        initialCode: `package main;

fn square(x: int) int {
    return x * x;
}

fn increment(x: int) int = x + 1; // Single expression syntax

fn main() {
    val num = 5;
    
    // Standard call
    println("Square: " + square(num));

    // Pipe operator
    val result = num 
        |> increment() 
        |> square();
        
    println("Result (5+1)^2: " + result);
}`
    },
    {
        id: 7,
        title: "Error Handling",
        content: `
            <h3 class="text-lg font-semibold mb-2">Error Handling</h3>
            <p class="mb-4">Use <code>try-catch</code> blocks to handle runtime exceptions.</p>
            <p class="mb-4">You can throw errors using <code>throw Error("message")</code>.</p>
        `,
        initialCode: `package main;

fn divide(a: int, b: int) int {
    if (b == 0) {
        throw Error("Division by zero!");
    }
    return a / b;
}

fn main() {
    try {
        println("10 / 2 = " + divide(10, 2));
        println("10 / 0 = " + divide(10, 0));
    } catch (e) {
        println("Caught error: " + e.message);
    }
}`
    },
    {
        id: 8,
        title: "Modules & Imports",
        content: `
            <h3 class="text-lg font-semibold mb-2">Modules</h3>
            <p class="mb-4">Code is organized into packages.</p>
            <ul class="list-disc list-inside ml-2 mb-4">
                <li><code>package name;</code> declares the current package.</li>
                <li><code>use name;</code> imports another package.</li>
            </ul>
            <p class="mb-4">Common standard libraries include <code>math</code>, <code>strings</code>, and <code>log</code>.</p>
        `,
        initialCode: `package main;

use math;
use strings;

fn main() {
    val root = math.sqrt(16.0);
    val shout = strings.toUpper("hello");

    println("Root: " + root);
    println("Shout: " + shout);
}`
    },
    {
        id: 9,
        title: "Best Practices",
        content: `
            <h3 class="text-lg font-semibold mb-2">Style & Best Practices</h3>
            <ul class="list-disc list-inside ml-2 mb-4">
                <li><strong>Naming:</strong> Use <code>camelCase</code> for variables/functions and <code>PascalCase</code> for types.</li>
                <li><strong>Immutability:</strong> Prefer <code>val</code>. Only use <code>var</code> when necessary.</li>
                <li><strong>Formatting:</strong> Standard indentation is 4 spaces. Spaces around operators.</li>
            </ul>
        `,
        initialCode: `package main;

// Good style class
class User {
    val name: string;
    val age: int;
    
    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }
}

fn main() {
    val currentUser = User("Alice", 30);
    println("User: " + currentUser.name);
}`
    },
    {
        id: 10,
        title: "Examples: FizzBuzz",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: FizzBuzz</h3>
            <p class="mb-4">Combine loops, conditionals (or pattern matching), and arithmetic.</p>
            <p class="mb-4">Print numbers 1 to 20. If divisible by 3, print "Fizz". If by 5, "Buzz". If both, "FizzBuzz".</p>
        `,
        initialCode: `package main;

fn main() {
    for (i in 1..=20) {
        val output = when {
            i % 15 == 0 -> "FizzBuzz"
            i % 3 == 0 -> "Fizz"
            i % 5 == 0 -> "Buzz"
            else -> i.toString() // Convert int to string
        };
        
        println(output);
    }
}`
    }
];
