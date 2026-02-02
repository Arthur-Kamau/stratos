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
            <p class="mb-4">Stratos is a modern, statically-typed programming language designed for readability, safety, and powerful concurrency.</p>
            <p class="mb-4">Every Stratos file must start with a <code>package</code> declaration. In this example, we use <code>package main;</code>.</p>
            <p class="mb-4">The entry point of the program is the <code>main</code> function.</p>
            <p class="mb-4">Click the "Run" button to execute the code and see the output!</p>
        `,
        initialCode: `package main;

fn main() {
    print("Hello from Stratos!");
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

    print("Sum: " + (x + y));
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

    print("Name: " + name);
    print("Age: " + age);
    print("Version: " + version);
    print("Active: " + isActive);
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
    print("Count: " + count);

    // Compound assignment
    count += 2;
    print("Count after +=2: " + count);

    // Another mutable variable
    var message = "Hello";
    message = message + " World!";
    print(message);
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
    val f64Val: f64 = 2.718;

    print("Integer: " + intVal);
    print("i32: " + i32Val);
    print("u64: " + u64Val);
    print("Double: " + doubleVal);
    print("f64: " + f64Val);
}`
    },
    {
        id: 6,
        title: "Strings & String Interpolation",
        content: `
            <h3 class="text-lg font-semibold mb-2">Text Types & Interpolation</h3>
            <p class="mb-4">Stratos has powerful string handling with interpolation:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>string</code>: Text strings (double quotes)</li>
                <li><code>char</code>: Single characters (single quotes)</li>
                <li><strong>String Interpolation:</strong> Use <code>$variable</code> or <code>\${expression}</code> to embed values</li>
                <li>Concatenation with <code>+</code> operator also works</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val name = "Alice";
    val age = 30;
    val x = 10;
    val y = 20;

    // String interpolation with $
    print("Hello, $name!");
    print("You are $age years old");

    // Expression interpolation with \${}
    print("Sum: \${x + y}");
    print("Next year you'll be \${age + 1}");

    // Combining both
    print("$name's calculation: \${x * y}");

    // Character type
    val initial: char = 'A';
    print("Initial: " + initial);
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
                <li>Also supports keyword syntax: <code>and</code>, <code>or</code>, <code>not</code> (recommended)</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val isActive: bool = true;
    val hasPermission: bool = false;

    // Keyword syntax (recommended)
    val canAccess = isActive and hasPermission;
    val showMessage = isActive or hasPermission;
    val isInactive = not isActive;

    print("Can Access: $canAccess");
    print("Show Message: $showMessage");
    print("Is Inactive: $isInactive");

    // Symbol syntax also works
    val result = isActive && !hasPermission;
    print("Symbol syntax result: $result");
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
                <li><strong>Safe Cast (<code>as?</code>):</strong> Use when it might fail. Returns <code>void</code> on failure.</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val pi: double = 3.14159;

    // Forced cast (truncates)
    val rounded = pi as int;
    print("Rounded: $rounded");

    // Safe cast (success)
    val num = "123" as? int;
    print("Parsed: $num");

    // Safe cast (failure returns void)
    val invalid = "abc" as? int;
    print("Invalid result: '$invalid'");

    // Int to double promotion
    val intVal = 42;
    val doubleVal = intVal as double;
    print("As double: $doubleVal");
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

    print("Addition: \${a + b}");
    print("Subtraction: \${a - b}");
    print("Multiplication: \${a * b}");
    print("Division: \${a / b}");
    print("Modulo: \${a % b}");

    val x = 10.0;
    val y = 3.0;
    print("Float Division: \${x / y}");
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

    print("$x == $y: \${x == y}");
    print("$x != $y: \${x != y}");
    print("$x < $y: \${x < y}");
    print("$x > $y: \${x > y}");
    print("$x <= $y: \${x <= y}");
    print("$x >= $y: \${x >= y}");

    // Compare strings
    val str1 = "apple";
    val str2 = "banana";
    print("'$str1' < '$str2': \${str1 < str2}");
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
                <li>Ternary if expressions (returns a value)</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    val score = 85;

    if (score >= 90) {
        print("A Grade");
    } else if (score >= 80) {
        print("B Grade");
    } else if (score >= 70) {
        print("C Grade");
    } else {
        print("Need Improvement");
    }

    // If as expression (returns value)
    val status = if (score >= 70) "Passed" else "Failed";
    print("Status: $status");

    // Using logical operators
    val hasBonus = score >= 80 and score < 90;
    print("Has bonus: $hasBonus");
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
        print("Count: $count");
        count += 1;
    }

    print("Loop finished!");

    // With break
    var i = 0;
    while (true) {
        i += 1;
        if (i > 3) {
            break;
        }
        print("i = $i");
    }
}`
    },
    {
        id: 13,
        title: "For Loops",
        content: `
            <h3 class="text-lg font-semibold mb-2">For Loop</h3>
            <p class="mb-4">Loop over ranges or collections.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Range loops: <code>for val i in 1..5</code> (inclusive)</li>
                <li>Use <code>val</code> to declare the loop variable</li>
                <li>Loop variables are immutable by default</li>
                <li>Works with arrays and other collections</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    print("Counting 1 to 5:");
    for val i in 1..5 {
        print("i: $i");
    }

    print("\\nCounting by twos:");
    var j = 0;
    while (j <= 10) {
        print("j: $j");
        j += 2;
    }

    print("\\nReverse counting:");
    for val k in 5..1 {
        print("k: $k");
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
                <li>Guards with <code>if</code> for complex conditions</li>
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

    print("Number: $x");
    print("Message: $message");

    // When with guards (conditions)
    val age = 25;
    val category = when (age) {
        _ if age < 13 -> "Child"
        _ if age < 20 -> "Teenager"
        _ if age < 65 -> "Adult"
        else -> "Senior"
    };

    print("Age $age is: $category");
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
                <li>Return type after parameters</li>
                <li>Function body with <code>return</code> statement</li>
                <li>Single-expression functions with <code>=</code> syntax</li>
            </ul>
        `,
        initialCode: `package main;

// Function with parameters and return type
fn add(a: int, b: int) int {
    return a + b;
}

// Single-expression function
fn square(x: int) int = x * x;

// Function without return value
fn greet(name: string) {
    print("Hello, $name!");
}

fn main() {
    val sum = add(5, 3);
    print("5 + 3 = $sum");

    val squared = square(4);
    print("4 squared = $squared");

    greet("Stratos");
}`
    },
    {
        id: 16,
        title: "Function Parameters",
        content: `
            <h3 class="text-lg font-semibold mb-2">Parameter Types</h3>
            <p class="mb-4">Functions support various parameter types.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Basic parameter types with type annotations</li>
                <li>Default parameters with <code>=</code></li>
                <li>Variadic parameters with <code>...</code></li>
            </ul>
        `,
        initialCode: `package main;

// Default parameters
fn greet(name: string, greeting: string = "Hello") {
    print("$greeting, $name!");
}

// Variadic parameters
fn sum(...numbers: int) int {
    var total = 0;
    for val num in numbers {
        total += num;
    }
    return total;
}

fn main() {
    greet("Alice");
    greet("Bob", "Hi");

    val result = sum(1, 2, 3, 4, 5);
    print("Sum: $result");
}`
    },
    {
        id: 17,
        title: "The Pipe Operator",
        content: `
            <h3 class="text-lg font-semibold mb-2">Function Chaining</h3>
            <p class="mb-4">The <code>|></code> operator lets you chain function calls left-to-right.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Improves readability of nested calls</li>
                <li>Data flows from left to right</li>
                <li>Alternative to deeply nested function calls</li>
            </ul>
        `,
        initialCode: `package main;

fn increment(x: int) int = x + 1;
fn double(x: int) int = x * 2;
fn square(x: int) int = x * x;

fn main() {
    val num = 5;

    // Traditional nested calls (hard to read)
    val result1 = square(double(increment(num)));

    // Pipe operator (much clearer!)
    val result2 = num
        |> increment()
        |> double()
        |> square();

    print("Input: $num");
    print("After increment: \${num + 1}");
    print("After double: \${(num + 1) * 2}");
    print("After square: $result2");
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
                <li>Block body: <code>(x) => { ... return value; }</code></li>
                <li>Closures capture variables from surrounding scope</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Simple lambda
    val double = (x: int) => x * 2;
    print("Double 5: \${double(5)}");

    // Multiple parameters
    val add = (a: int, b: int) => a + b;
    print("5 + 3: \${add(5, 3)}");

    // Closure (captures outer variable)
    val factor = 10;
    val multiply = (x: int) => x * factor;
    print("7 * $factor = \${multiply(7)}");

    // Block body with return
    val factorial = (n: int) => {
        var result = 1;
        for val i in 1..n {
            result *= i;
        }
        return result;
    };

    print("5! = \${factorial(5)}");
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
                <li>Type annotations: <code>Array&lt;int&gt;</code></li>
                <li>Index access: <code>arr[0]</code> (zero-based)</li>
                <li>Length: <code>arr.length()</code></li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Array literal with type annotation
    val numbers: Array<int> = [1, 2, 3, 4, 5];

    // Type inferred array
    val names = ["Alice", "Bob", "Charlie"];

    print("First number: \${numbers[0]}");
    print("Second name: \${names[1]}");
    print("Array length: \${numbers.length()}");

    // Loop through array
    print("\\nAll numbers:");
    for val num in numbers {
        print("  $num");
    }

    // Loop with index
    print("\\nNames with index:");
    for val i in 0..names.length() {
        print("  $i: \${names[i]}");
    }
}`
    },
    {
        id: 20,
        title: "Destructuring",
        content: `
            <h3 class="text-lg font-semibold mb-2">Destructuring Assignment</h3>
            <p class="mb-4">Extract multiple values from arrays or ranges at once.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Array destructuring: <code>val (a, b, c) = [1, 2, 3]</code></li>
                <li>Range destructuring: <code>val (x, y, z) = 1..4</code></li>
                <li>Mutable destructuring with <code>var</code></li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    // Array destructuring
    val numbers = [10, 20, 30];
    val (a, b, c) = numbers;

    print("a = $a");
    print("b = $b");
    print("c = $c");

    // Range destructuring
    val (first, second, third) = 1..4;
    print("\\nFrom range: $first, $second, $third");

    // Partial destructuring (extra elements ignored)
    val coords = [100, 200, 300, 400];
    val (x, y) = coords;
    print("\\nFirst two: x=$x, y=$y");

    // Mutable destructuring
    var (m, n) = [5, 10];
    m = m * 2;
    print("\\nMutable m = $m");
}`
    },
    {
        id: 21,
        title: "Maps",
        content: `
            <h3 class="text-lg font-semibold mb-2">Key-Value Pairs</h3>
            <p class="mb-4">Maps store key-value pairs. Use the <code>maps</code> module for immutable maps.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Create with <code>maps.create()</code></li>
                <li>Add entries with <code>.put(key, value)</code></li>
                <li>Get values with <code>.get(key)</code></li>
                <li>Check existence with <code>.has(key)</code></li>
            </ul>
        `,
        initialCode: `package main;

use maps;

fn main() {
    // Create an immutable map (returns new map on each operation)
    val scores = maps.create()
        .put("Alice", "90")
        .put("Bob", "85")
        .put("Charlie", "88");

    // Get values
    print("Alice's score: \${scores.get("Alice")}");

    // Check if key exists
    print("Bob exists: \${scores.has("Bob")}");
    print("Dave exists: \${scores.has("Dave")}");

    // Map size
    print("Number of entries: \${scores.size()}");

    // Iterate over entries
    print("\\nAll scores:");
    for val entry in scores.entries() {
        print("  \${entry.first}: \${entry.second}");
    }
}`
    },
    {
        id: 22,
        title: "Optional Types",
        content: `
            <h3 class="text-lg font-semibold mb-2">Handling Missing Values</h3>
            <p class="mb-4"><code>Optional&lt;T&gt;</code> represents a value that may or may not be present.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>Optional.some(value)</code> - Create with a value</li>
                <li><code>Optional.none()</code> - Create empty</li>
                <li><code>.isSome()</code>, <code>.isNone()</code> - Check presence</li>
                <li><code>.unwrap()</code>, <code>.unwrapOr(default)</code> - Get value</li>
            </ul>
        `,
        initialCode: `package main;

fn findUser(id: int) Optional<string> {
    if (id == 1) {
        return Optional.some("Alice");
    } else if (id == 2) {
        return Optional.some("Bob");
    }
    return Optional.none();
}

fn main() {
    val user1 = findUser(1);
    val user2 = findUser(99);

    // Check and unwrap
    if (user1.isSome()) {
        print("Found: \${user1.unwrap()}");
    }

    if (user2.isNone()) {
        print("User 99 not found");
    }

    // Use default value
    val name = findUser(99).unwrapOr("Guest");
    print("Name: $name");

    // Pattern matching with when
    when (findUser(2)) {
        Some(n) -> print("User 2 is: $n")
        None -> print("Not found")
    }
}`
    },
    {
        id: 23,
        title: "Result Types",
        content: `
            <h3 class="text-lg font-semibold mb-2">Error Handling with Result</h3>
            <p class="mb-4"><code>Result&lt;T, E&gt;</code> represents success or failure.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>Result.ok(value)</code> - Create success</li>
                <li><code>Result.err(message)</code> - Create error</li>
                <li>Pattern match with <code>Ok(value)</code> and <code>Err(error)</code></li>
                <li>More explicit than exceptions</li>
            </ul>
        `,
        initialCode: `package main;

fn divide(a: int, b: int) Result<int, string> {
    if (b == 0) {
        return Result.err("Division by zero");
    }
    return Result.ok(a / b);
}

fn main() {
    val result1 = divide(10, 2);
    val result2 = divide(10, 0);

    // Pattern matching
    when (result1) {
        Ok(value) -> print("10 / 2 = $value")
        Err(error) -> print("Error: $error")
    }

    when (result2) {
        Ok(value) -> print("Result: $value")
        Err(error) -> print("Error: $error")
    }

    // Chain operations
    val calc = divide(20, 4);
    when (calc) {
        Ok(v) -> print("20 / 4 = $v")
        Err(e) -> print("Failed: $e")
    }
}`
    },
    {
        id: 24,
        title: "Try-Catch",
        content: `
            <h3 class="text-lg font-semibold mb-2">Exception Handling</h3>
            <p class="mb-4">Use <code>try-catch</code> blocks for exception-style error handling.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Wrap risky code in <code>try { ... }</code></li>
                <li>Catch errors with <code>catch (e) { ... }</code></li>
                <li>Throw errors with <code>throw Error("message")</code></li>
            </ul>
        `,
        initialCode: `package main;

fn riskyOperation(value: int) int {
    if (value < 0) {
        throw Error("Value cannot be negative!");
    }
    if (value == 0) {
        throw Error("Value cannot be zero!");
    }
    return 100 / value;
}

fn main() {
    try {
        print("Result: \${riskyOperation(10)}");
        print("Result: \${riskyOperation(-5)}");
    } catch (e) {
        print("Caught error: \${e.message}");
    }

    print("Program continues...");
}`
    },
    {
        id: 25,
        title: "Modules & Imports",
        content: `
            <h3 class="text-lg font-semibold mb-2">Package System</h3>
            <p class="mb-4">Code is organized into packages and modules.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>package name;</code> declares the current package</li>
                <li><code>use name;</code> imports a module</li>
                <li>Standard library: <code>math</code>, <code>strings</code>, <code>log</code>, <code>io</code>, etc.</li>
                <li>Access with <code>module.function()</code> syntax</li>
            </ul>
        `,
        initialCode: `package main;

use math;
use strings;
use log;

fn main() {
    // Math module
    val root = math.sqrt(16.0);
    print("sqrt(16) = $root");

    // Strings module
    val upper = strings.toUpper("hello");
    print("Uppercase: $upper");

    // Log module for structured logging
    log.info("Application started");
    log.warn("This is a warning");

    print("\\nModules are powerful!");
}`
    },
    {
        id: 26,
        title: "Math Module",
        content: `
            <h3 class="text-lg font-semibold mb-2">Mathematical Functions</h3>
            <p class="mb-4">The <code>math</code> module provides common math functions.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>sqrt</code>, <code>pow</code>, <code>abs</code> - Basic math</li>
                <li><code>sin</code>, <code>cos</code>, <code>tan</code> - Trigonometry</li>
                <li><code>exp</code>, <code>log</code>, <code>log10</code> - Exponential/logarithmic</li>
                <li><code>max</code>, <code>min</code>, <code>floor</code>, <code>ceil</code></li>
            </ul>
        `,
        initialCode: `package main;

use math;

fn main() {
    print("=== Basic Math ===");
    print("sqrt(16): \${math.sqrt(16.0)}");
    print("pow(2, 8): \${math.pow(2.0, 8.0)}");
    print("abs(-42): \${math.abs(-42)}");

    print("\\n=== Trigonometry ===");
    print("sin(0): \${math.sin(0.0)}");
    print("cos(0): \${math.cos(0.0)}");

    print("\\n=== Comparison ===");
    print("max(5, 3): \${math.max(5, 3)}");
    print("min(5, 3): \${math.min(5, 3)}");

    print("\\n=== Rounding ===");
    print("floor(3.7): \${math.floor(3.7)}");
    print("ceil(3.2): \${math.ceil(3.2)}");
    print("round(3.5): \${math.round(3.5)}");
}`
    },
    {
        id: 27,
        title: "Strings Module",
        content: `
            <h3 class="text-lg font-semibold mb-2">String Operations</h3>
            <p class="mb-4">The <code>strings</code> module provides string manipulation functions.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>toUpper</code>, <code>toLower</code> - Case conversion</li>
                <li><code>trim</code>, <code>length</code> - Whitespace and size</li>
                <li><code>contains</code>, <code>indexOf</code> - Searching</li>
                <li><code>split</code>, <code>replace</code> - Manipulation</li>
            </ul>
        `,
        initialCode: `package main;

use strings;

fn main() {
    val text = "  Hello, Stratos!  ";

    print("Original: '$text'");
    print("Trimmed: '\${strings.trim(text)}'");
    print("Upper: '\${strings.toUpper(text)}'");
    print("Lower: '\${strings.toLower(text)}'");
    print("Length: \${strings.length(text)}");

    print("\\n=== Searching ===");
    print("Contains 'Stratos': \${strings.contains(text, "Stratos")}");
    print("Contains 'Python': \${strings.contains(text, "Python")}");

    print("\\n=== Split ===");
    val csv = "apple,banana,orange";
    val parts = strings.split(csv, ",");
    for val part in parts {
        print("  - $part");
    }
}`
    },
    {
        id: 28,
        title: "Classes - Basic",
        content: `
            <h3 class="text-lg font-semibold mb-2">Class Basics</h3>
            <p class="mb-4">Classes define blueprints for objects.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Fields hold data (<code>val</code> or <code>var</code>)</li>
                <li>Constructor initializes new instances</li>
                <li>Methods define behavior</li>
                <li>Use <code>this</code> to reference current instance</li>
            </ul>
        `,
        initialCode: `package main;

class Person {
    val name: string;
    var age: int;

    constructor(name: string, age: int) {
        this.name = name;
        this.age = age;
    }

    fn greet() {
        print("Hello, I'm \${this.name}!");
    }

    fn isAdult() bool {
        return this.age >= 18;
    }

    fn haveBirthday() {
        this.age += 1;
        print("\${this.name} is now \${this.age}!");
    }
}

fn main() {
    val person = Person("Alice", 25);
    person.greet();
    print("Is adult: \${person.isAdult()}");
    person.haveBirthday();
}`
    },
    {
        id: 29,
        title: "Interfaces",
        content: `
            <h3 class="text-lg font-semibold mb-2">Interfaces</h3>
            <p class="mb-4">Interfaces define contracts that classes must implement.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Define method signatures without implementation</li>
                <li>Classes implement with <code>: InterfaceName</code></li>
                <li>Enables polymorphism</li>
                <li>Multiple interfaces can be implemented</li>
            </ul>
        `,
        initialCode: `package main;

interface Shape {
    fn area() double;
    fn describe() string;
}

class Circle : Shape {
    val radius: double;

    constructor(radius: double) {
        this.radius = radius;
    }

    fn area() double {
        return 3.14159 * this.radius * this.radius;
    }

    fn describe() string {
        return "Circle with radius \${this.radius}";
    }
}

class Rectangle : Shape {
    val width: double;
    val height: double;

    constructor(width: double, height: double) {
        this.width = width;
        this.height = height;
    }

    fn area() double {
        return this.width * this.height;
    }

    fn describe() string {
        return "Rectangle \${this.width}x\${this.height}";
    }
}

fn main() {
    val circle = Circle(5.0);
    val rect = Rectangle(4.0, 6.0);

    print("\${circle.describe()}: area = \${circle.area()}");
    print("\${rect.describe()}: area = \${rect.area()}");
}`
    },
    {
        id: 30,
        title: "Inheritance",
        content: `
            <h3 class="text-lg font-semibold mb-2">Class Inheritance</h3>
            <p class="mb-4">Classes can inherit from other classes.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Extend with <code>class Child : Parent</code></li>
                <li>Call parent constructor with <code>super(...)</code></li>
                <li>Override methods to customize behavior</li>
                <li>Reuse parent methods and fields</li>
            </ul>
        `,
        initialCode: `package main;

class Animal {
    val name: string;

    constructor(name: string) {
        this.name = name;
    }

    fn speak() {
        print("\${this.name} makes a sound");
    }
}

class Dog : Animal {
    val breed: string;

    constructor(name: string, breed: string) {
        super(name);
        this.breed = breed;
    }

    // Override parent method
    fn speak() {
        print("\${this.name} barks: Woof!");
    }

    fn fetch() {
        print("\${this.name} fetches the ball!");
    }
}

class Cat : Animal {
    constructor(name: string) {
        super(name);
    }

    fn speak() {
        print("\${this.name} meows: Meow!");
    }
}

fn main() {
    val dog = Dog("Max", "Golden Retriever");
    val cat = Cat("Luna");

    dog.speak();
    dog.fetch();
    cat.speak();
}`
    },
    {
        id: 31,
        title: "Structs",
        content: `
            <h3 class="text-lg font-semibold mb-2">Structs</h3>
            <p class="mb-4">Structs are lightweight data containers without methods.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Simple data grouping</li>
                <li>Initialized with <code>Name { field: value }</code></li>
                <li>Access fields with dot notation</li>
                <li>Use for simple data transfer objects</li>
            </ul>
        `,
        initialCode: `package main;

struct Point {
    x: int;
    y: int;
}

struct Color {
    r: int;
    g: int;
    b: int;
}

struct Pixel {
    position: Point;
    color: Color;
}

fn main() {
    // Create struct instances
    val origin = Point { x: 0, y: 0 };
    val red = Color { r: 255, g: 0, b: 0 };

    print("Origin: (\${origin.x}, \${origin.y})");
    print("Red: rgb(\${red.r}, \${red.g}, \${red.b})");

    // Nested structs
    val pixel = Pixel {
        position: Point { x: 100, y: 200 },
        color: Color { r: 0, g: 255, b: 0 }
    };

    print("Pixel at (\${pixel.position.x}, \${pixel.position.y})");
}`
    },
    {
        id: 32,
        title: "Enums",
        content: `
            <h3 class="text-lg font-semibold mb-2">Enumerations</h3>
            <p class="mb-4">Enums define a type with a fixed set of values.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Define named constants</li>
                <li>Can hold associated data</li>
                <li>Pattern match with <code>when</code></li>
                <li>Type-safe alternatives to magic strings/numbers</li>
            </ul>
        `,
        initialCode: `package main;

enum Direction {
    NORTH,
    SOUTH,
    EAST,
    WEST
}

enum Status {
    Pending,
    Active,
    Completed,
    Failed
}

fn describeDirection(dir: Direction) string {
    return when (dir) {
        NORTH -> "Going up"
        SOUTH -> "Going down"
        EAST -> "Going right"
        WEST -> "Going left"
    };
}

fn main() {
    val heading = Direction.NORTH;
    print("Direction: \${describeDirection(heading)}");

    val status = Status.Active;
    val message = when (status) {
        Pending -> "Waiting..."
        Active -> "In progress"
        Completed -> "Done!"
        Failed -> "Error occurred"
    };

    print("Status: $message");
}`
    },
    {
        id: 33,
        title: "Defer Statements",
        content: `
            <h3 class="text-lg font-semibold mb-2">Defer for Cleanup</h3>
            <p class="mb-4">The <code>defer</code> statement schedules code to run when the function returns.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Guaranteed cleanup (like Go's defer)</li>
                <li>Runs even if function returns early or throws</li>
                <li>Multiple defers execute in LIFO order (last-in, first-out)</li>
                <li>Perfect for resource cleanup</li>
            </ul>
        `,
        initialCode: `package main;

fn processData() {
    print("1. Starting process");

    defer print("5. Cleanup complete (defer 1)");
    defer print("4. Releasing resources (defer 2)");

    print("2. Doing work...");
    print("3. Work complete");

    // Defers run in reverse order when function ends
}

fn earlyReturn(value: int) {
    defer print("  Defer always runs!");

    if (value < 0) {
        print("  Early return for negative");
        return;
    }

    print("  Normal path");
}

fn main() {
    processData();

    print("\\nTesting early return:");
    earlyReturn(-1);
    earlyReturn(5);
}`
    },
    {
        id: 34,
        title: "Concurrency - Goroutines",
        content: `
            <h3 class="text-lg font-semibold mb-2">Lightweight Threads</h3>
            <p class="mb-4">Goroutines are lightweight threads for concurrent execution.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Launch with <code>concurrent.go(|| { ... })</code></li>
                <li>Extremely lightweight (small stack)</li>
                <li>Multiplexed onto OS threads</li>
                <li>Use <code>concurrent.sleep()</code> for delays</li>
            </ul>
        `,
        initialCode: `package main;

use concurrent;

fn worker(id: int) {
    print("Worker $id starting");
    concurrent.sleep(100);
    print("Worker $id done");
}

fn main() {
    print("Launching workers...");

    // Launch goroutines
    concurrent.go(|| worker(1));
    concurrent.go(|| worker(2));
    concurrent.go(|| worker(3));

    print("Workers launched!");

    // Wait for workers to complete
    concurrent.sleep(500);

    print("All workers finished");
}`
    },
    {
        id: 35,
        title: "Concurrency - Channels",
        content: `
            <h3 class="text-lg font-semibold mb-2">Channel Communication</h3>
            <p class="mb-4">Channels are typed conduits for safe goroutine communication.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Create with <code>concurrent.newChannel&lt;T&gt;(size)</code></li>
                <li>Send with <code>ch.send(value)</code></li>
                <li>Receive with <code>ch.receive()</code></li>
                <li>Close with <code>ch.close()</code></li>
            </ul>
        `,
        initialCode: `package main;

use concurrent;

fn producer(ch: Channel<int>) {
    for val i in 1..5 {
        print("Sending: $i");
        ch.send(i);
        concurrent.sleep(50);
    }
    ch.close();
}

fn consumer(ch: Channel<int>) {
    while (true) {
        val value = ch.receive();
        if (value == null) {
            break;
        }
        print("Received: $value");
    }
    print("Consumer done");
}

fn main() {
    val ch = concurrent.newChannel<int>(3);

    concurrent.go(|| producer(ch));
    concurrent.go(|| consumer(ch));

    concurrent.sleep(500);
    print("\\nDone!");
}`
    },
    {
        id: 36,
        title: "Concurrency - WaitGroup",
        content: `
            <h3 class="text-lg font-semibold mb-2">Synchronization</h3>
            <p class="mb-4">WaitGroup waits for a collection of goroutines to finish.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>wg.add(n)</code> - Add to wait count</li>
                <li><code>wg.done()</code> - Decrement count (call when done)</li>
                <li><code>wg.wait()</code> - Block until count is zero</li>
                <li>Combine with <code>defer</code> for safety</li>
            </ul>
        `,
        initialCode: `package main;

use concurrent;

fn task(id: int, wg: WaitGroup) {
    defer wg.done();

    print("Task $id starting");
    concurrent.sleep(100);
    print("Task $id complete");
}

fn main() {
    val wg = concurrent.newWaitGroup();

    print("Starting tasks...");

    for val i in 1..4 {
        wg.add(1);
        concurrent.go(|| task(i, wg));
    }

    print("Waiting for tasks...");
    wg.wait();

    print("All tasks complete!");
}`
    },
    {
        id: 37,
        title: "Async/Await",
        content: `
            <h3 class="text-lg font-semibold mb-2">Asynchronous Programming</h3>
            <p class="mb-4">Write async code that looks synchronous with <code>async</code> and <code>await</code>.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><code>async fn</code> declares an async function</li>
                <li><code>await</code> waits for a Future to complete</li>
                <li>Returns <code>Future&lt;T&gt;</code></li>
                <li>Non-blocking I/O operations</li>
            </ul>
        `,
        initialCode: `package main;

use async;

async fn fetchData(id: int) Future<string> {
    print("Fetching data for id $id...");
    await async.delay(100);
    return "Data-$id";
}

async fn processAll() {
    print("Starting async operations...");

    val data1 = await fetchData(1);
    print("Got: $data1");

    val data2 = await fetchData(2);
    print("Got: $data2");

    print("All data fetched!");
}

fn main() {
    print("=== Async/Await Demo ===");
    processAll();

    // Keep main alive for async operations
    async.delay(500);
}`
    },
    {
        id: 38,
        title: "Challenge: FizzBuzz",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: FizzBuzz</h3>
            <p class="mb-4">Combine loops, conditionals, and pattern matching.</p>
            <p class="mb-4">Print numbers 1 to 20:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>If divisible by 3, print "Fizz"</li>
                <li>If divisible by 5, print "Buzz"</li>
                <li>If divisible by both, print "FizzBuzz"</li>
                <li>Otherwise, print the number</li>
            </ul>
        `,
        initialCode: `package main;

fn main() {
    print("=== FizzBuzz ===");

    for val i in 1..20 {
        val output = when {
            i % 15 == 0 -> "FizzBuzz"
            i % 3 == 0 -> "Fizz"
            i % 5 == 0 -> "Buzz"
            else -> "$i"
        };

        print(output);
    }
}`
    },
    {
        id: 39,
        title: "Challenge: Prime Numbers",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: Prime Numbers</h3>
            <p class="mb-4">Write a function to check if a number is prime.</p>
            <p class="mb-4">A prime number is greater than 1 and has no divisors other than 1 and itself.</p>
        `,
        initialCode: `package main;

fn isPrime(n: int) bool {
    if (n <= 1) {
        return false;
    }
    if (n == 2) {
        return true;
    }
    if (n % 2 == 0) {
        return false;
    }

    var i = 3;
    while (i * i <= n) {
        if (n % i == 0) {
            return false;
        }
        i += 2;
    }

    return true;
}

fn main() {
    print("Prime numbers from 1 to 30:");

    for val i in 1..30 {
        if (isPrime(i)) {
            print("$i is prime");
        }
    }
}`
    },
    {
        id: 40,
        title: "Challenge: Worker Pool",
        content: `
            <h3 class="text-lg font-semibold mb-2">Challenge: Worker Pool Pattern</h3>
            <p class="mb-4">Implement a concurrent worker pool that processes jobs.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Create a job channel</li>
                <li>Launch multiple worker goroutines</li>
                <li>Send jobs and collect results</li>
            </ul>
        `,
        initialCode: `package main;

use concurrent;

fn worker(id: int, jobs: Channel<int>, results: Channel<int>) {
    while (true) {
        val job = jobs.receive();
        if (job == null) {
            break;
        }

        print("Worker $id processing job $job");
        concurrent.sleep(50);

        results.send(job * 2);
    }
}

fn main() {
    val numJobs = 5;
    val numWorkers = 3;

    val jobs = concurrent.newChannel<int>(numJobs);
    val results = concurrent.newChannel<int>(numJobs);

    // Start workers
    for val w in 1..numWorkers {
        concurrent.go(|| worker(w, jobs, results));
    }

    // Send jobs
    for val j in 1..numJobs {
        jobs.send(j);
    }
    jobs.close();

    // Collect results
    print("\\nResults:");
    for val _ in 1..numJobs {
        val result = results.receive();
        print("  Got: $result");
    }
}`
    },
    {
        id: 41,
        title: "Best Practices",
        content: `
            <h3 class="text-lg font-semibold mb-2">Style Guidelines</h3>
            <p class="mb-4">Follow these best practices for clean Stratos code.</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li><strong>Naming:</strong> <code>camelCase</code> for variables/functions, <code>PascalCase</code> for types</li>
                <li><strong>Immutability:</strong> Prefer <code>val</code>. Only use <code>var</code> when necessary</li>
                <li><strong>String Interpolation:</strong> Use <code>$var</code> instead of concatenation</li>
                <li><strong>Error Handling:</strong> Prefer Result/Optional over exceptions</li>
                <li><strong>Defer:</strong> Use for cleanup operations</li>
                <li><strong>Concurrency:</strong> Prefer channels over shared memory</li>
            </ul>
        `,
        initialCode: `package main;

use log;

// Good: PascalCase for types
struct UserProfile {
    name: string;
    email: string;
}

// Good: Descriptive function name
fn createUserProfile(name: string, email: string) Result<UserProfile, string> {
    // Good: Validate input, return Result
    if (name == "") {
        return Result.err("Name is required");
    }

    return Result.ok(UserProfile { name: name, email: email });
}

fn main() {
    // Good: Use val for immutability
    val result = createUserProfile("Alice", "alice@example.com");

    // Good: Pattern match on Result
    when (result) {
        Ok(user) -> {
            // Good: Use string interpolation
            log.info("Created user: \${user.name}");
        }
        Err(error) -> {
            log.error("Failed: $error");
        }
    }
}`
    },
    {
        id: 42,
        title: "Congratulations!",
        content: `
            <h3 class="text-lg font-semibold mb-2">You're Ready!</h3>
            <p class="mb-4">You've completed the Stratos tutorial! You now know:</p>
            <ul class="list-disc list-inside ml-2 mb-4 space-y-2">
                <li>Variables, types, and string interpolation</li>
                <li>Control flow with if, while, for, and when</li>
                <li>Functions, lambdas, and the pipe operator</li>
                <li>Collections: Arrays, Maps, destructuring</li>
                <li>Error handling with Result, Optional, and try-catch</li>
                <li>OOP: Classes, interfaces, inheritance, structs, enums</li>
                <li>Concurrency: Goroutines, channels, WaitGroups</li>
                <li>Async/await for asynchronous programming</li>
                <li>Standard library modules</li>
            </ul>
            <p class="mb-4">Continue learning by building your own projects!</p>
        `,
        initialCode: `package main;

use log;

fn main() {
    log.info("Congratulations on completing the Stratos tutorial!");

    val skills = [
        "Variables & Types",
        "Control Flow",
        "Functions & Lambdas",
        "Collections",
        "Error Handling",
        "OOP & Structs",
        "Concurrency",
        "Async/Await"
    ];

    print("\\nYou've learned:");
    for val skill in skills {
        print("  - $skill");
    }

    print("\\nYou're ready to build amazing things with Stratos!");
}`
    }
];
