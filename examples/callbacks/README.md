# Callbacks Example

This example demonstrates callback functions and lambda expressions in Stratos.

## Features Demonstrated

- **Lambda expressions** using arrow syntax `=>`
- **Closures** that capture variables from outer scope
- **Higher-order functions** that accept functions as parameters
- **Functions returning functions**
- **Nested function declarations**

## Running the Example

```bash
stratos run examples/callbacks
```

## Expected Output

```
--- Callback Showcase ---
10 * 2 = 20
3 * 5 = 15
5 + 10 = 15
--- Done ---
```

## Code Highlights

### Simple Callbacks

```stratos
fn apply(value: int, op: Function) int {
    return op(value);
}

val res1 = apply(10, (x) => x * 2);
// Result: 20
```

### Closures

```stratos
val factor = 5;
val res2 = apply(3, (x) => x * factor);
// Result: 15 (captures 'factor' from outer scope)
```

### Higher-Order Functions

```stratos
fn makeAdder(n: int) Function {
    return (x) => x + n;
}

val add10 = makeAdder(10);
val result = add10(5);  // Result: 15
```

## Important Notes

- Use `=>` (fat arrow) for lambda expressions
- Avoid using keywords like `val`, `var`, `fn` as parameter names
- Lambdas can have single-expression bodies or block bodies with `return`
