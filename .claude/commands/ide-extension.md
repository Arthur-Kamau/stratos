# IDE Extension Development

Work with the Stratos IDE extensions (VS Code, IntelliJ).

## Arguments
- $ARGUMENTS: IDE name and action (e.g., "vscode build", "intellij inspect", "vscode add-snippet")

## Instructions

### VS Code Extension
Located at `tooling/vscode/`

**Inspect:** Read the extension source files to understand capabilities:
- `tooling/vscode/package.json` — Extension manifest, language contributions
- `tooling/vscode/syntaxes/` — TextMate grammar for syntax highlighting
- `tooling/vscode/snippets/` — Code snippets
- `tooling/vscode/README.md` — Extension documentation

**Build:** Package the VS Code extension:
```bash
cd tooling/vscode
npm install
npx vsce package
```

**Add snippet:** Add a new code snippet to the VS Code extension:
1. Read existing snippets in `tooling/vscode/snippets/`
2. Add the new snippet following the VS Code snippet format
3. Register in `package.json` if needed

**Features to verify/add:**
- Syntax highlighting for all Stratos keywords
- Code snippets for common patterns (fn, class, struct, enum, when, for, etc.)
- File association for `.st` files
- Comment toggling support
- Bracket matching

### IntelliJ Plugin
Located at `tooling/intellij/`

**Inspect:** Read the plugin source files.
- Check `tooling/intellij/` for plugin structure

### Common Tasks

1. **Update syntax highlighting** for new keywords:
   - Add token patterns to the TextMate grammar
   - Test with example files

2. **Add code snippet:**
   ```json
   {
     "Stratos Function": {
       "prefix": "fn",
       "body": [
         "fn ${1:name}(${2:params}) ${3:returnType} {",
         "    ${0}",
         "}"
       ],
       "description": "Stratos function declaration"
     }
   }
   ```
