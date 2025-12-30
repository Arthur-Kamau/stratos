# Error Handling

Stratos provides multiple mechanisms for handling errors gracefully, from traditional try/catch blocks to Result types and optional values.

## Try/Catch Blocks

The most common way to handle errors:

```stratos
fn divide(a: int, b: int) int {
    if (b == 0) {
        throw Error("Division by zero");
    }
    return a / b;
}

fn main() {
    try {
        val result = divide(10, 0);
        print("Result: " + result);
    } catch (error) {
        print("Error: " + error.message);
    }
}
```

Output:
```
Error: Division by zero
```

## Error Types

### Built-in Error

```stratos
throw Error("Something went wrong");
```

### Custom Error Types

```stratos
class ValidationError : Error {
    var field: string;

    constructor(message: string, field: string) {
        super(message);
        this.field = field;
    }
}

fn validateEmail(email: string) {
    if (!strings.contains(email, "@")) {
        throw ValidationError("Invalid email format", "email");
    }
}

fn main() {
    try {
        validateEmail("invalid-email");
    } catch (error: ValidationError) {
        print("Validation failed for field: " + error.field);
        print("Message: " + error.message);
    } catch (error: Error) {
        print("General error: " + error.message);
    }
}
```

## Finally Block

Execute code regardless of whether an error occurred:

```stratos
use io;
use log;

fn processFile(filename: string) {
    val file = io.open(filename);

    try {
        val content = file.read();
        print(content);
    } catch (error) {
        log.error("Failed to read file: " + error.message);
    } finally {
        file.close();  // Always executed
        log.info("File closed");
    }
}
```

## Optional Values for Errors

Use Optional types instead of throwing errors:

```stratos
fn parseInt(str: string) int? {
    try {
        return Some(str.toInt());
    } catch {
        return None;
    }
}

fn main() {
    val num1 = parseInt("42");
    val num2 = parseInt("not-a-number");

    when (num1) {
        Some(n) -> print("Parsed: " + n)
        None -> print("Failed to parse")
    }

    when (num2) {
        Some(n) -> print("Parsed: " + n)
        None -> print("Failed to parse")
    }
}
```

Output:
```
Parsed: 42
Failed to parse
```

## Result Type Pattern

A more explicit way to handle success/failure:

```stratos
enum Result<T, E> {
    Ok(T),
    Err(E)
}

fn divide(a: double, b: double) Result<double, string> {
    if (b == 0.0) {
        return Err("Division by zero");
    }
    return Ok(a / b);
}

fn main() {
    val result = divide(10.0, 2.0);

    when (result) {
        Ok(value) -> print("Result: " + value)
        Err(error) -> print("Error: " + error)
    }

    val badResult = divide(10.0, 0.0);

    when (badResult) {
        Ok(value) -> print("Result: " + value)
        Err(error) -> print("Error: " + error)
    }
}
```

Output:
```
Result: 5.0
Error: Division by zero
```

## Error Propagation

### Throwing Errors Up

```stratos
fn readConfig() Config {
    val content = io.readFile("config.json");  // May throw
    return json.decode(content);                // May throw
}

fn initialize() {
    try {
        val config = readConfig();
        print("Config loaded");
    } catch (error) {
        print("Failed to initialize: " + error.message);
    }
}
```

### Using Result Type

```stratos
fn readConfig() Result<Config, string> {
    try {
        val content = io.readFile("config.json");
        val config = json.decode(content);
        return Ok(config);
    } catch (error) {
        return Err(error.message);
    }
}

fn initialize() {
    val result = readConfig();

    when (result) {
        Ok(config) -> {
            print("Config loaded");
            // Use config
        }
        Err(message) -> {
            print("Failed to load config: " + message);
        }
    }
}
```

## Validation Pattern

```stratos
class Validator {
    var errors: Array<string>;

    constructor() {
        this.errors = [];
    }

    fn validate(condition: bool, message: string) {
        if (!condition) {
            this.errors.push(message);
        }
    }

    fn hasErrors() bool {
        return this.errors.length > 0;
    }

    fn getErrors() string {
        return strings.join(this.errors, "\n");
    }
}

class User {
    var name: string;
    var email: string;
    var age: int;

    fn validate() Result<bool, string> {
        val validator = Validator();

        validator.validate(
            this.name.length() > 0,
            "Name is required"
        );

        validator.validate(
            strings.contains(this.email, "@"),
            "Invalid email format"
        );

        validator.validate(
            this.age >= 18,
            "Must be at least 18 years old"
        );

        if (validator.hasErrors()) {
            return Err(validator.getErrors());
        }

        return Ok(true);
    }
}

fn main() {
    val user = User {
        name: "",
        email: "invalid",
        age: 15
    };

    val result = user.validate();

    when (result) {
        Ok(_) -> print("User is valid")
        Err(errors) -> {
            print("Validation errors:");
            print(errors);
        }
    }
}
```

## Async Error Handling

Handling errors in async functions:

```stratos
use net;
use async;
use log;

async fn fetchData(url: string) Result<any, string> {
    try {
        val response = await net.get(url);

        if (response.status() == 200) {
            return Ok(response.json());
        } else {
            return Err("HTTP " + response.status());
        }
    } catch (error) {
        return Err(error.message);
    }
}

fn main() async {
    val result = await fetchData("https://api.example.com/data");

    when (result) {
        Ok(data) -> {
            log.info("Data fetched successfully");
            print(data);
        }
        Err(message) -> {
            log.error("Failed to fetch data: " + message);
        }
    }
}
```

## Panic and Recover

For unrecoverable errors:

```stratos
fn criticalOperation() {
    if (someCondition) {
        panic("Critical failure!");  // Terminates program
    }
}

fn main() {
    try {
        criticalOperation();
    } catch (panic) {
        // Can't recover from panic in normal code
        // Use this only for logging before exit
        log.error("Panic: " + panic.message);
        os.exit(1);
    }
}
```

## Best Practices

::: tip Use appropriate error handling
- Use try/catch for exceptional situations
- Use Optional for expected failures (like parsing)
- Use Result type for operations that can fail in predictable ways
:::

::: tip Provide context
Always include helpful error messages that explain what went wrong and why:

```stratos
throw Error("Failed to connect to database at " + host + ":" + port);
```
:::

::: tip Don't swallow errors
Always handle errors appropriately. Don't catch and ignore:

```stratos
// Bad
try {
    riskyOperation();
} catch {}  // Error ignored!

// Good
try {
    riskyOperation();
} catch (error) {
    log.error("Operation failed: " + error.message);
    // Handle or propagate
}
```
:::

::: warning Avoid panics
Use `panic()` only for truly unrecoverable errors. Prefer throwing recoverable errors.
:::

## Error Handling Quick Reference

| Mechanism | Use Case | Example |
|-----------|----------|---------|
| try/catch | Exceptional errors | `try { ... } catch (e) { ... }` |
| Optional | Expected failures | `fn parse(s: string) int?` |
| Result | Explicit success/fail | `fn divide(a, b) Result<double, string>` |
| throw | Raise error | `throw Error("message")` |
| panic | Unrecoverable error | `panic("critical failure")` |

## Next Steps

- [Async Programming](/guide/async) - Handle errors in async code
- [Null Safety](/guide/null-safety) - Using Optional types
- [Standard Library - log](/reference/stdlib#log) - Logging errors
