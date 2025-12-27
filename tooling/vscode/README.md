# Stratos Language Extension for VS Code

Official Visual Studio Code extension for the Stratos programming language, providing rich language support including syntax highlighting, code completion, error detection, and more.

## Features

### 🎨 Syntax Highlighting
- Full syntax highlighting for Stratos code
- Support for keywords, types, functions, and comments
- Custom Stratos Dark theme

### ✨ Intelligent Code Completion
- **Keyword completion**: `val`, `var`, `fn`, `class`, `if`, `while`, `for`, `when`, etc.
- **Standard library completion**: All stdlib modules and functions
  - `math.*` - Mathematical functions (sin, cos, sqrt, pow, etc.)
  - `strings.*` - String manipulation (toUpper, toLower, trim, split, etc.)
  - `log.*` - Logging functions (debug, info, warn, error, fatal)
  - `io.*` - File I/O operations
  - `os.*` - Operating system functions
  - `net.*` - HTTP client functions
  - `crypto.*` - Cryptographic functions
  - `async.*` - Async operations
  - `collections.*` - Data structures (Map, Set, List, Queue)
  - And more!
- **Context-aware suggestions**: Type `module.` to see available functions
- **Snippet support**: Quick templates for common patterns

### 🔍 Error Detection
- **Compile-time errors**: Real-time syntax and type checking
- **Runtime error warnings**:
  - Division by zero detection
  - Potential null pointer access
  - Negative array indices
  - And more!

### 📝 Code Snippets
- Pre-defined snippets for common Stratos patterns
- Function definitions
- Class declarations
- Control flow structures

## Installation

### From VSIX
1. Download the latest `stratos-language-*.vsix` file
2. Open VS Code
3. Go to Extensions (Ctrl+Shift+X)
4. Click the `...` menu → "Install from VSIX..."
5. Select the downloaded file

### From Source
```bash
cd tooling/vscode
npm install
npm run compile
vsce package
code --install-extension stratos-language-*.vsix
```

## Configuration

Configure the extension in VS Code settings:

```json
{
  "stratosLanguageServer.compilerPath": "/usr/local/bin/stratos",
  "stratosLanguageServer.maxNumberOfProblems": 1000
}
```

### Settings

| Setting | Type | Default | Description |
|---------|------|---------|-------------|
| `stratosLanguageServer.compilerPath` | string | `""` | Path to Stratos compiler. Leave empty for auto-detection. |
| `stratosLanguageServer.maxNumberOfProblems` | number | `1000` | Maximum number of problems to report. |

## Usage

### Code Completion

Start typing and press `Ctrl+Space` to trigger completion:

```stratos
use math;

fn main() {
    val x = math.  // Press Ctrl+Space to see math functions
    //          ^
}
```

### Import Modules

Type `use ` and press `Ctrl+Space` to see available stdlib modules:

```stratos
use   // Shows: math, strings, log, io, os, etc.
```

### Snippets

Type a keyword and press `Tab` to expand:

- `fn` → Function template
- `class` → Class template
- `if` → If statement template
- `for` → For loop template
- `when` → Pattern matching template
- `try` → Try-catch template

### Error Detection

The extension automatically:
- ✅ Validates syntax as you type
- ✅ Shows compiler errors inline
- ✅ Warns about potential runtime errors
- ✅ Highlights problematic code

## Examples

### Standard Library Autocomplete

```stratos
use math;
use strings;

fn main() {
    // Type "math." to see all math functions
    val angle = math.toRadians(45.0);
    val result = math.sin(angle);

    // Type "strings." to see all string functions
    val text = "  hello  ";
    val cleaned = strings.trim(text);
    val upper = strings.toUpper(cleaned);
}
```

### Runtime Error Detection

```stratos
fn divide(a: int, b: int) int {
    return a / b;  // ⚠️ If b is 0, potential division by zero
}

val result = array[-1];  // ⚠️ Warning: Negative array index

val user: User? = getUser();
user.name;  // ⚠️ Warning: Potential null access, use user?.name
```

## Development & Testing

### Building from Source

```bash
# Install dependencies
npm install

# Compile TypeScript
npm run compile

# Watch mode (auto-compile on changes)
npm run watch

# Package extension
npm install -g vsce
vsce package
```

### Testing the Extension

1. **Open the Extension in VS Code**:
   - Open VS Code
   - Go to File > Open Folder... and select `tooling/vscode`

2. **Launch the Extension Development Host**:
   - Press `F5`
   - This opens a new VS Code window with your extension loaded

3. **Test the Features**:
   - Open any `.st` file
   - Test syntax highlighting
   - Try code completion (Ctrl+Space)
   - Check error detection

### Project Structure

```
vscode/
├── src/
│   ├── extension.ts       # Extension entry point
│   └── server/
│       └── server.ts      # Language server implementation
├── syntaxes/              # Syntax highlighting grammar
├── snippets/              # Code snippets
├── themes/                # Color themes
└── package.json           # Extension manifest
```

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Troubleshooting

### Autocomplete not working

1. Make sure the extension is activated (check status bar)
2. Try reloading VS Code (Ctrl+Shift+P → "Reload Window")
3. Check Output panel (View → Output → Select "Stratos Language Server")

### Compiler errors not showing

1. Verify Stratos compiler is installed: `stratos --version`
2. Set compiler path in settings if not in PATH
3. Check file is saved with `.st` extension

### Extension not loading

1. Check VS Code version is 1.75.0 or higher
2. Look for errors in Developer Tools (Help → Toggle Developer Tools)
3. Reinstall the extension

## Known Issues

- Autocomplete for user-defined functions not yet implemented
- Go to definition feature in development
- Hover documentation in development

## Roadmap

- [ ] Go to definition
- [ ] Find all references
- [ ] Rename symbol
- [ ] Hover documentation
- [ ] Code formatting
- [ ] Debugging support
- [ ] User function autocomplete
- [ ] Import organization

## License

MIT License - See LICENSE file for details

## Links

- [Stratos Language](https://github.com/stratos-lang/stratos)
- [Documentation](https://stratos-lang.org/docs)
- [Report Issues](https://github.com/stratos-lang/stratos/issues)

## Changelog

### Version 0.1.1
- ✨ Added standard library autocomplete (all 10 modules)
- ✨ Runtime error detection (division by zero, null access, etc.)
- ✨ Context-aware completion (module.function)
- ✨ Configurable compiler path
- 🐛 Improved error message parsing
- 🐛 Better syntax error highlighting

### Version 0.1.0
- Initial release
- Basic syntax highlighting
- Keyword completion
- Theme support
