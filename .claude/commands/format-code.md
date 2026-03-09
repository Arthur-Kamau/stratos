# Format Stratos Code

Format Stratos source files using the built-in formatter.

## Arguments
- $ARGUMENTS: File or directory path, and optional flags (e.g., "src/main.st", ". --check", "examples/")

## Instructions

1. Parse the path and flags from $ARGUMENTS. Default to current directory if no path given.

2. **Check formatting** (dry run):
   ```bash
   ./src/build/stratos fmt <path> --check
   ```

3. **Format in place**:
   ```bash
   ./src/build/stratos fmt <path> -w
   ```

4. **Print formatted output** (no write):
   ```bash
   ./src/build/stratos fmt <path>
   ```

### Style Rules
- 4 spaces indentation (no tabs)
- Opening braces on same line as declaration
- Space after keywords: `if`, `while`, `for`, `fn`, `class`, `when`
- Space around operators: `=`, `+`, `-`, `*`, `/`, `==`, `!=`, `<`, `>`, `->`, `=>`
- Blank line between top-level declarations (functions, classes, imports)
- No trailing whitespace

If the stratos binary is not found, suggest running `/build` first.
