# Dependency Test Project

This project demonstrates Stratos's dependency management system.

## Setup

1. Fetch dependencies:
   ```bash
   stratos get
   ```

2. Run the project:
   ```bash
   stratos run src/main.st
   ```

## Dependencies

This project depends on `stratos-sample-lib`, which provides utility functions.

## Features Demonstrated

- HOCON-format dependency specification
- Local dependency resolution
- Transitive dependency support
- Lock file generation
- Dependency validation

## Testing Commands

```bash
# Fetch all dependencies
stratos get

# Fetch with verbose output
stratos get -v

# Update dependencies
stratos get --update

# Verify lock file
stratos get --verify
```

## Expected Output

```
=== Testing Stratos Dependency Management ===

Hello, Stratos User from sample-lib!
10 + 20 = 30
5 * 6 = 30
5! = 120
max(42, 17) = 42
min(42, 17) = 17

=== All tests completed successfully! ===
```
