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
            <p class="mb-4">Click the "Run" button to execute the code and see the output!</p>
        `,
        initialCode: `package main;

fn main() {
    println("Hello from Stratos!");
}`
    },
    {
        id: 2,
        title: "Basic Syntax",
        content: `
            <h3 class="text-lg font-semibold mb-2">Syntax Fundamentals</h3>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><strong>Semicolons:</strong> Every statement must end with a semicolon (<code>;</code>). This is mandatory!</li>
                <li><strong>Comments:</strong> Use <code>//</code> for single-line and <code>/* */</code> for multi-line comments.</li>
                <li><strong>Blocks:</strong> Braces <code>{ }</code> are required for all blocks (functions, loops, if-statements).</li>
            </ul>
            <p class="mb-4">Try removing a semicolon and see what happens!</p>
        `,
        initialCode: `package main;

fn main() {
    // This is a single-line comment
    /* 
       This is a 
       multi-line comment
    */
    
    val x = 10;
    val y = 20;

    println("Sum: " + (x + y));
}`
    },
    {
        id: 3,
        title: "Variables - Immutable (val)",
        content: `
            <h3 class="text-lg font-semibold mb-2">Immutable Variables</h3>
            <p class="mb-4">Stratos encourages immutability. Use <code>val</code> for variables that won't change.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Once assigned, values cannot be changed</li>
                <li>Safer and makes code more predictable</li>
                <li>Types can be inferred or explicitly declared</li>
            </ul>
            <p class="mb-4">Try uncommenting the line that changes <code>name</code> and see the error!</p>
        `,
        initialCode: `package main;

fn main() {
    // Immutable variables - type inferred
    val name = "Stratos"; 
    val age = 5;
    val isActive = true;
    
    // Immutable with explicit type
    val version: double = 1.0;
    
    // name = "Other"; // This would be an error!
    
    println("Name: " + name);
    println("Age: " + age);
    println("Version: " + version);
    println("Active: " + isActive);
}`
    },
    {
        id: 4,
        title: "Variables - Mutable (var)",
        content: `
            <h3 class="text-lg font-semibold mb-2">Mutable Variables</h3>
            <p class="mb-4">Use <code>var</code> for variables that need to change values.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Can be reassigned after declaration</li>
                <li>Use sparingly - prefer <code>val</code> whenever possible</li>
                <li>Supports compound assignment operators</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Mutable variable
    var count = 0;
    
    // Increment
    count = count + 1;
    println("Count: " + count);
    
    // Compound assignment
    count += 2;
    println("Count after +=2: " + count);
    
    // Another mutable variable
    var message = "Hello";
    message = message + " World!";
    println(message);
}`
    },
    {
        id: 5,
        title: "Primitive Types - Numbers",
        content: `
            <h3 class="text-lg font-semibold mb-2">Numeric Types</h3>
            <p class="mb-4">Stratos supports various numeric types:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>int</code>: Signed integers (e.g., 42, -10)</li>
                <li><code>double</code>: Floating-point numbers (e.g., 3.14, -0.5)</li>
                <li><code>i8, i16, i32, i64</code>: Fixed-size signed integers</li>
                <li><code>u8, u16, u32, u64</code>: Fixed-size unsigned integers</li>
                <li><code>f32, f64</code>: Fixed-size floating-point numbers</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Integer types
    val intVal: int = 42;
    val i32Val: i32 = 1000;
    val u64Val: u64 = 1000000;
    
    // Floating-point
    val doubleVal: double = 3.14159;
    val f32Val: f32 = 2.718f;
    
    println("Integer: " + intVal);
    println("i32: " + i32Val);
    println("u64: " + u64Val);
    println("Double: " + doubleVal);
    println("f32: " + f32Val);
}`
    },
    {
        id: 6,
        title: "Primitive Types - Strings & Chars",
        content: `
            <h3 class="text-lg font-semibold mb-2">Text Types</h3>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>string</code>: Text strings (double quotes)</li>
                <li><code>char</code>: Single characters (single quotes)</li>
                <li>Strings support concatenation with + operator</li>
                <li>Char supports escape sequences</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // String literals
    val greeting: string = "Hello, World!";
    val emptyStr: string = "";
    
    // Character literals
    val initial: char = 'S';
    val newline: char = '\\n';
    val tab: char = '\\t';
    
    // String concatenation
    val fullName = "John" + " " + "Doe";
    
    println(greeting);
    println("Name: " + fullName);
    println("Initial: " + initial);
}`
    },
    {
        id: 7,
        title: "Primitive Types - Booleans",
        content: `
            <h3 class="text-lg font-semibold mb-2">Boolean Type</h3>
            <p class="mb-4">The <code>bool</code> type represents true/false values.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>true</code> and <code>false</code> are boolean literals</li>
                <li>Supports logical operations: <code>&&</code> (and), <code>||</code> (or), <code>!</code> (not)</li>
                <li>Also supports keyword syntax: <code>and</code>, <code>or</code>, <code>not</code></li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val isActive: bool = true;
    val hasPermission: bool = false;
    
    // Logical operations
    val canAccess = isActive && hasPermission;
    val showMessage = isActive || hasPermission;
    val isInactive = !isActive;
    
    println("Can Access: " + canAccess);
    println("Show Message: " + showMessage);
    println("Is Inactive: " + isInactive);
    
    // Keyword syntax
    val keywordLogic = isActive and not hasPermission;
    println("Keyword Logic: " + keywordLogic);
}`
    },
    {
        id: 8,
        title: "Type Casting",
        content: `
            <h3 class="text-lg font-semibold mb-2">Type Conversion</h3>
            <p class="mb-4">Stratos requires explicit type conversion. Two ways to cast:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
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
        id: 9,
        title: "Arithmetic Operations",
        content: `
            <h3 class="text-lg font-semibold mb-2">Basic Arithmetic</h3>
            <p class="mb-4">Supports all standard arithmetic operations.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>+</code> Addition</li>
                <li><code>-</code> Subtraction</li>
                <li><code>*</code> Multiplication</li>
                <li><code>/</code> Division</li>
                <li><code>%</code> Modulo (remainder)</li>
            </ul>
            <p class="mb-4">Note: Division of integers truncates towards zero.</p>
        `,
        initialCode: `package main;

fn main() {
    val a = 10;
    val b = 3;
    
    println("Addition: " + (a + b));
    println("Subtraction: " + (a - b));
    println("Multiplication: " + (a * b));
    println("Division: " + (a / b));
    println("Modulo: " + (a % b));
    
    val x = 10.0;
    val y = 3.0;
    println("Float Division: " + (x / y));
}`
    },
    {
        id: 10,
        title: "Comparison Operations",
        content: `
            <h3 class="text-lg font-semibold mb-2">Comparisons</h3>
            <p class="mb-4">Compare values using relational operators:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>==</code> Equal to</li>
                <li><code>!=</code> Not equal to</li>
                <li><code><</code> Less than</li>
                <li><code>></code> Greater than</li>
                <li><code><=</code> Less than or equal to</li>
                <li><code>>=</code> Greater than or equal to</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val x = 5;
    val y = 10;
    
    println(x == y); // false
    println(x != y); // true
    println(x < y);  // true
    println(x > y);  // false
    println(x <= y); // true
    println(x >= y); // false
    
    // Compare strings
    val str1 = "apple";
    val str2 = "banana";
    println(str1 < str2); // true (lexicographic order)
}`
    },
    {
        id: 11,
        title: "If Statements",
        content: `
            <h3 class="text-lg font-semibold mb-2">Conditional Statements</h3>
            <p class="mb-4">Use <code>if</code> statements for conditional execution.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Simple if statement</li>
                <li>If-else statements</li>
                <li>If-else if chains</li>
                <li>Ternary if expressions</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val score = 85;
    
    if (score >= 90) {
        println("A Grade");
    } else if (score >= 80) {
        println("B Grade");
    } else if (score >= 70) {
        println("C Grade");
    } else {
        println("Need Improvement");
    }
    
    // Ternary if expression
    val status = if (score >= 70) "Passed" else "Failed";
    println("Status: " + status);
}`
    },
    {
        id: 12,
        title: "While Loops",
        content: `
            <h3 class="text-lg font-semibold mb-2">While Loop</h3>
            <p class="mb-4">Loop while a condition is true.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Execute code repeatedly while condition is true</li>
                <li>Can use <code>break</code> to exit early</li>
                <li>Can use <code>continue</code> to skip to next iteration</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    var count = 1;
    
    while (count <= 5) {
        println("Count: " + count);
        count += 1;
    }
    
    println("Loop finished!");
}`
    },
    {
        id: 13,
        title: "For Loops",
        content: `
            <h3 class="text-lg font-semibold mb-2">For Loop</h3>
            <p class="mb-4">Loop over ranges or collections.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Range loops: <code>for (i in 1..10)</code></li>
                <li>Inclusive range: <code>for (i in 1..=10)</code></li>
                <li>Loop variables are immutable by default</li>
                <li>Can use <code>var</code> for mutable loop variables</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    println("Counting 1 to 5:");
    for (i in 1..=5) {
        println("i: " + i);
    }
    
    println("\\nCounting even numbers:");
    for (i in 0..10 step 2) {
        println("i: " + i);
    }
    
    println("\\nReverse counting:");
    for (i in 5..=1) {
        println("i: " + i);
    }
}`
    },
    {
        id: 14,
        title: "When Expressions",
        content: `
            <h3 class="text-lg font-semibold mb-2">Pattern Matching</h3>
            <p class="mb-4">The <code>when</code> expression is a powerful pattern matching construct.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Match against values</li>
                <li>Handle multiple cases</li>
                <li>Default case with <code>else</code></li>
                <li>Pattern matching with conditions</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val x = 5;
    
    val message = when (x) {
        1 -> "One"
        2 -> "Two"
        5 -> "Five"
        else -> "Other"
    };
    
    println("Number: " + x);
    println("Message: " + message);
    
    // When without argument (like if-else chain)
    val status = when {
        x > 0 -> "Positive"
        x < 0 -> "Negative"
        else -> "Zero"
    };
    
    println("Status: " + status);
}`
    },
    {
        id: 15,
        title: "Function Basics",
        content: `
            <h3 class="text-lg font-semibold mb-2">Function Declaration</h3>
            <p class="mb-4">Define functions with <code>fn</code> keyword.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Function name with parameters</li>
                <li>Optional return type</li>
                <li>Function body with <code>return</code> statement</li>
                <li>Single-expression functions (arrow syntax)</li>
            </ul>
        `,
        initialCode: `package main;

// Function with parameters and return type
fn add(a: int, b: int) int {
    return a + b;
}

// Single-expression function (arrow syntax)
fn square(x: int) int = x * x;

fn main() {
    val sum = add(5, 3);
    println("5 + 3 = " + sum);
    
    val squared = square(4);
    println("4 squared = " + squared);
}`
    },
    {
        id: 16,
        title: "Function Parameters",
        content: `
            <h3 class="text-lg font-semibold mb-2">Parameter Types</h3>
            <p class="mb-4">Functions support various parameter types.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Basic parameter types</li>
                <li>Default parameters</li>
                <li>Variadic parameters (<code>...</code>)</li>
                <li>Type annotations are optional (inferred)</li>
            </ul>
        `,
        initialCode: `package main;

// Default parameters
fn greet(name: string, greeting: string = "Hello") {
    println(greeting + ", " + name + "!");
}

// Variadic parameters
fn sum(...numbers: int) int {
    var total = 0;
    for (num in numbers) {
        total += num;
    }
    return total;
}

fn main() {
    greet("Alice");
    greet("Bob", "Hi");
    
    val result = sum(1, 2, 3, 4);
    println("Sum: " + result);
}`
    },
    {
        id: 17,
        title: "The Pipe Operator",
        content: `
            <h3 class="text-lg font-semibold mb-2">Function Chaining</h3>
            <p class="mb-4">The <code>|></code> operator lets you chain function calls.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Improves readability of nested calls</li>
                <li>Left-to-right flow of data</li>
                <li>Alternative to nested function calls</li>
            </ul>
        `,
        initialCode: `package main;

fn increment(x: int) int = x + 1;
fn double(x: int) int = x * 2;
fn square(x: int) int = x * x;

fn main() {
    val num = 5;
    
    // Traditional nested calls
    val result1 = square(double(increment(num)));
    
    // Pipe operator (more readable)
    val result2 = num 
        |> increment() 
        |> double() 
        |> square();
    
    println("Result 1: " + result1);
    println("Result 2: " + result2);
}`
    },
    {
        id: 18,
        title: "Lambda Functions",
        content: `
            <h3 class="text-lg font-semibold mb-2">Anonymous Functions</h3>
            <p class="mb-4">Lambda functions are anonymous functions created with <code>=></code> syntax.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Simple lambdas: <code>(x) => x * 2</code></li>
                <li>Multiple parameters: <code>(a, b) => a + b</code></li>
                <li>Block body: <code>(x) => { ... }</code></li>
                <li>Capture variables from surrounding scope</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Simple lambda
    val double = (x: int) => x * 2;
    println("Double 5: " + double(5));
    
    // Multiple parameters
    val add = (a: int, b: int) => a + b;
    println("5 + 3: " + add(5, 3));
    
    // Block body
    val factorial = (n: int) => {
        var result = 1;
        for (i in 1..=n) {
            result *= i;
        }
        return result;
    };
    
    println("5!: " + factorial(5));
}`
    },
    {
        id: 19,
        title: "Arrays",
        content: `
            <h3 class="text-lg font-semibold mb-2">Array Basics</h3>
            <p class="mb-4">Arrays store multiple values of the same type.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Array literals: <code>[1, 2, 3]</code></li>
                <li>Type annotations: <code>Array<int></code></li>
                <li>Index access: <code>arr[0]</code></li>
                <li>Length property: <code>arr.length()</code></li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Array literal with type annotation
    val numbers: Array<int> = [1, 2, 3, 4, 5];
    
    // Type inferred array
    val names = ["Alice", "Bob", "Charlie"];
    
    println("First number: " + numbers[0]);
    println("Second name: " + names[1]);
    println("Number of elements: " + numbers.length());
    
    // Loop through array
    println("Numbers:");
    for (num in numbers) {
        println(num);
    }
}`
    },
    {
        id: 20,
        title: "Maps",
        content: `
            <h3 class="text-lg font-semibold mb-2">Key-Value Pairs</h3>
            <p class="mb-4">Maps store key-value pairs.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Map creation: <code>map["key": "value"]</code></li>
                <li>Type: <code>map<string, int></code></li>
                <li>Get values: <code>map.get("key")</code></li>
                <li>Set values: <code>map.set("key", value)</code></li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Create a map
    val scores = map["Alice": 90, "Bob": 85, "Charlie": 88];
    
    // Get values
    println("Alice's score: " + scores.get("Alice"));
    
    // Check if key exists
    println("Bob exists: " + scores.has("Bob"));
    println("Dave exists: " + scores.has("Dave"));
    
    // Iterate over map
    println("All scores:");
    for (entry in scores.entries()) {
        println(entry.key + ": " + entry.value);
    }
}`
    },
    {
        id: 21,
        title: "Error Handling - Try Catch",
        content: `
            <h3 class="text-lg font-semibold mb-2">Handling Exceptions</h3>
            <p class="mb-4">Use <code>try-catch</code> blocks to handle errors.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Wrap risky code in <code>try { ... }</code></li>
                <li>Catch errors with <code>catch (e) { ... }</code></li>
                <li>Throw errors with <code>throw Error("message")</code></li>
            </ul>
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
        id: 22,
        title: "Modules & Imports",
        content: `
            <h3 class="text-lg font-semibold mb-2">Package System</h3>
            <p class="mb-4">Code is organized into packages.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>package name;</code> declares the current package</li>
                <li><code>use name;</code> imports another package</li>
                <li>Standard library modules: <code>math</code>, <code>strings</code>, <code>log</code>, etc.</li>
                <li>Use module.function syntax: <code>math.sqrt(16)</code></li>
            </ul>
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
        id: 23,
        title: "Math Module",
        content: `
            <h3 class="text-lg font-semibold mb-2">Mathematical Functions</h3>
            <p class="mb-4">The <code>math</code> module provides common math functions.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>math.sqrt(x)</code> - Square root</li>
                <li><code>math.sin(x)</code>, <code>math.cos(x)</code>, <code>math.tan(x)</code> - Trigonometry</li>
                <li><code>math.exp(x)</code>, <code>math.log(x)</code> - Exponential and logarithmic</li>
                <li><code>math.max(x, y)</code>, <code>math.min(x, y)</code> - Comparison</li>
                <li><code>math.random()</code> - Random number</li>
            </ul>
        `,
        initialCode: `package main;

use math;

fn main() {
    println("sqrt(16): " + math.sqrt(16.0));
    println("sin(0): " + math.sin(0.0));
    println("cos(0): " + math.cos(0.0));
    println("tan(0): " + math.tan(0.0));
    println("exp(1): " + math.exp(1.0));
    println("log(10): " + math.log(10.0));
    println("max(5, 3): " + math.max(5, 3));
    println("min(5, 3): " + math.min(5, 3));
    println("random: " + math.random());
}`
    },
    {
        id: 24,
        title: "Strings Module",
        content: `
            <h3 class="text-lg font-semibold mb-2">String Operations</h3>
            <p class="mb-4">The <code>strings</code> module provides string manipulation functions.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>strings.toUpper(str)</code>, <code>strings.toLower(str)</code> - Case conversion</li>
                <li><code>strings.length(str)</code> - Get length</li>
                <li><code>strings.trim(str)</code> - Trim whitespace</li>
                <li><code>strings.contains(str, substr)</code> - Check if contains substring</li>
                <li><code>strings.split(str, delimiter)</code> - Split string</li>
            </ul>
        `,
        initialCode: `package main;

use strings;

fn main() {
    val text = "  Hello, Stratos!  ";
    
    println("Original: '" + text + "'");
    println("To Upper: '" + strings.toUpper(text) + "'");
    println("To Lower: '" + strings.toLower(text) + "'");
    println("Trimmed: '" + strings.trim(text) + "'");
    println("Length: " + strings.length(text));
    println("Contains 'Stratos': " + strings.contains(text, "Stratos"));
    
    val parts = strings.split("apple,banana,orange", ",");
    println("Split: [" + parts[0] + ", " + parts[1] + ", " + parts[2] + "]");
}`
    },
    {
        id: 25,
        title: "Classes - Basic",
        content: `
            <h3 class="text-lg font-semibold mb-2">Class Basics</h3>
            <p class="mb-4">Classes define blueprints for objects.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Fields hold data</li>
                <li>Methods define behavior</li>
                <li>Constructors initialize new instances</li>
                <li>Use <code>this</code> to reference current instance</li>
            </ul>
        `,
        initialCode: `package main;

class Person {
    val name: string;
    val age: int;
    
    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }
    
    fn greet() {
        println("Hello, my name is " + this.name);
    }
    
    fn isAdult() bool {
        return this.age >= 18;
    }
}

fn main() {
    val person = Person("Alice", 30);
    person.greet();
    println("Is adult: " + person.isAdult());
}`
    },
    {
        id: 26,
        title: "Classes - Methods",
        content: `
            <h3 class="text-lg font-semibold mb-2">Class Methods</h3>
            <p class="mb-4">Methods are functions associated with classes.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Instance methods (use <code>this</code>)</li>
                <li>Public methods with <code>pub</code> keyword</li>
                <li>Methods can have parameters and return values</li>
                <li>Methods can access private fields</li>
            </ul>
        `,
        initialCode: `package main;

class Rectangle {
    var width: double;
    var height: double;
    
    constructor(width: double, height: double) {
        this.width = width;
        this.height = height;
    }
    
    // Calculate area
    pub fn area() double {
        return this.width * this.height;
    }
    
    // Calculate perimeter
    pub fn perimeter() double {
        return 2 * (this.width + this.height);
    }
    
    // Check if it's a square
    pub fn isSquare() bool {
        return this.width == this.height;
    }
    
    // Set new dimensions
    pub fn setDimensions(newWidth: double, newHeight: double) {
        this.width = newWidth;
        this.height = newHeight;
    }
}

fn main() {
    val rect = Rectangle(10.0, 5.0);
    println("Area: " + rect.area());
    println("Perimeter: " + rect.perimeter());
    println("Is square: " + rect.isSquare());
    
    rect.setDimensions(7.0, 7.0);
    println("Is square now: " + rect.isSquare());
}`
    },
    {
        id: 27,
        title: "Challenge: FizzBuzz",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: FizzBuzz</h3>
            <p class="mb-4">Combine loops, conditionals, and arithmetic.</p>
            <p class="mb-4">Print numbers 1 to 20. If divisible by 3, print "Fizz". If by 5, "Buzz". If both, "FizzBuzz".</p>
            <p class="mb-4">This is a classic coding interview problem.</p>
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
    },
    {
        id: 28,
        title: "Challenge: Prime Numbers",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: Prime Numbers</h3>
            <p class="mb-4">Write a function to check if a number is prime.</p>
            <p class="mb-4">A prime number is a number greater than 1 that has no divisors other than 1 and itself.</p>
        `,
        initialCode: `package main;

fn isPrime(n: int) bool {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    for (i in 3..=n/2 step 2) {
        if (n % i == 0) {
            return false;
        }
    }
    
    return true;
}

fn main() {
    println("Primes up to 20:");
    for (i in 1..=20) {
        if (isPrime(i)) {
            println(i);
        }
    }
}`
    },
    {
        id: 29,
        title: "Best Practices",
        content: `
            <h3 class="text-lg font-semibold mb-2">Style Guidelines</h3>
            <p class="mb-4">Follow these best practices for clean Stratos code.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><strong>Naming:</strong> Use <code>camelCase</code> for variables/functions, <code>PascalCase</code> for types</li>
                <li><strong>Immutability:</strong> Prefer <code>val</code>. Only use <code>var</code> when necessary</li>
                <li><strong>Formatting:</strong> 4 spaces indentation, spaces around operators</li>
                <li><strong>Comments:</strong> Explain why, not what. Use <code>/** */</code> for documentation</li>
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
        id: 30,
        title: "Congratulations!",
        content: `
            <h3 class="text-lg font-semibold mb-2">You're Ready!</h3>
            <p class="mb-4">You've completed the Stratos tutorial!</p>
            <p class="mb-4">Now you know the fundamentals of the Stratos programming language. You can:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Write basic Stratos programs</li>
                <li>Use variables, functions, and control flow</li>
                <li>Work with arrays and maps</li>
                <li>Handle errors</li>
                <li>Use the standard library</li>
                <li>Create classes and objects</li>
            </ul>
            <p class="mb-4">Continue learning by exploring more examples and building your own programs!</p>
        `,
        initialCode: `package main;

fn main() {
    println("Congratulations on completing the Stratos tutorial!");
    println("You're ready to build amazing things!");
}`
    }
];
