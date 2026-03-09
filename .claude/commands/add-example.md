# Add a Stratos Example Project

Create a new example project demonstrating a language feature or use case.

## Arguments
- $ARGUMENTS: Example name and description (e.g., "generics-demo A demo of generic types and functions")

## Instructions

1. Parse the example name (first word) and description (rest) from $ARGUMENTS.

2. Create the example directory structure:
```
examples/<example-name>/
├── stratos.conf
└── src/
    └── main.st
```

3. Generate `stratos.conf`:
```hocon
project {
  name = <example_name_with_underscores>
  version = "1.0.0"
  description = "<description>"
  type = executable
}

build {
  entry = src/main.st
  output = build/<example_name>
}
```

4. Generate `src/main.st` with meaningful example code that demonstrates the feature described.
   - Include clear comments explaining what the example shows
   - Use idiomatic Stratos patterns
   - Import relevant std modules
   - Make the output self-documenting (println statements explaining what's happening)

5. Test the example:
   ```bash
   ./src/build/stratos run ./examples/<example-name>/
   ```

6. Report the result and show the generated code.

### Naming Conventions
- Use kebab-case for directory names: `my-feature-demo`
- Use snake_case for project names in stratos.conf: `my_feature_demo`
- Suffix with `-demo`, `-test`, or descriptive name
