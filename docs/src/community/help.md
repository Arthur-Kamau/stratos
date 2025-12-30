---
title: Getting Help
description: How to get help with Stratos
---

# Getting Help

If you encounter a problem, issue, or mistake with Stratos, there are several ways to get help.

## Quick Links

- 🐛 **Bug Reports**: [GitHub Issues](https://github.com/stratos-lang/stratos/issues)
- 💬 **Quick Questions**: [Discord Server](https://discord.gg/stratos)
- 📖 **Documentation**: Browse the [guides](/guide/) and [reference](/reference/)
- 💡 **Feature Requests**: [GitHub Discussions](https://github.com/stratos-lang/stratos/discussions)

## Before Asking for Help

1. **Check the documentation**
   - Browse the [Getting Started Guide](/guide/getting-started)
   - Search the [language guides](/guide/)
   - Review the [CLI reference](/reference/cli)

2. **Search existing issues**
   - Check [GitHub Issues](https://github.com/stratos-lang/stratos/issues)
   - Look for similar problems or questions
   - Review closed issues for solutions

3. **Try the examples**
   - Review the [examples section](/examples/)
   - Look at the sample projects in the repository
   - Test with minimal code to isolate the problem

## How to Report Issues

When reporting an issue on GitHub:

### Include This Information

1. **Stratos version**
   ```bash
   stratos --version
   ```

2. **Operating system and version**
   ```bash
   # Linux/macOS
   uname -a

   # Windows
   ver
   ```

3. **Minimal reproduction code**
   - Simplify your code to the smallest example that shows the problem
   - Include complete, runnable code when possible

4. **Expected behavior**
   - Describe what you expected to happen

5. **Actual behavior**
   - Describe what actually happened
   - Include error messages and stack traces

6. **Steps to reproduce**
   - Provide clear, numbered steps to reproduce the issue

### Example Issue Report

```markdown
**Stratos Version**: 0.1.0
**OS**: Ubuntu 22.04

**Description**
The compiler crashes when using null-safe operator with nested objects.

**Code to Reproduce**
```stratos
package main;

fn main() {
    val user: User? = getUser();
    val name = user?.profile?.name;  // Compiler crashes here
}
```

**Expected Behavior**
Code should compile successfully with null-safe chaining.

**Actual Behavior**
Compiler segfaults with error:
```
Segmentation fault (core dumped)
```

**Steps to Reproduce**
1. Create file with code above
2. Run `stratos compile test.st`
3. Observe crash
```

## Getting Help on Discord

Our [Discord server](https://discord.gg/stratos) is great for:
- Quick questions
- General discussion
- Troubleshooting help
- Community chat

**Channels:**
- `#general` - General discussion
- `#help` - Ask for help
- `#showcase` - Share your projects
- `#contributing` - Discuss contributions

**Discord Tips:**
- Use code blocks for code snippets (triple backticks)
- Provide context for your question
- Be patient - community members are volunteers
- Follow up with solutions you find

## Community Support

### StackOverflow

Tag your questions with `stratos` on [StackOverflow](https://stackoverflow.com/questions/tagged/stratos).

**Good questions include:**
- Clear problem description
- Minimal reproducible code
- What you've tried so far
- Expected vs actual results

### GitHub Discussions

Use [GitHub Discussions](https://github.com/stratos-lang/stratos/discussions) for:
- Feature proposals
- Design discussions
- General questions
- Show and tell

## Documentation Improvements

Found something unclear in the documentation?
- [Open an issue](https://github.com/stratos-lang/stratos/issues/new)
- Or submit a PR to improve the docs!

The documentation source is in the `docs/` directory.

## Common Issues

### Installation Problems

**Issue**: `stratos: command not found`

**Solution**: Ensure Stratos is in your PATH:
```bash
export PATH="$PATH:/usr/local/stratos/bin"
```

### Compilation Errors

**Issue**: "Package not found" errors

**Solution**: Run `stratos get` to fetch dependencies:
```bash
stratos get
```

### Runtime Issues

**Issue**: Segmentation fault during execution

**Solution**: Try compiling with verbose output:
```bash
stratos compile -v your-file.st
```

Check the [troubleshooting section](/guide/getting-started#troubleshooting) for more solutions.

## Contact Information

- **General Questions**: contact@araizen.com
- **Security Issues**: security@araizen.com
- **GitHub**: [stratos-lang/stratos](https://github.com/stratos-lang/stratos)

## Contributing

If you've solved a problem, consider:
- Updating the documentation
- Sharing your solution on Discord
- Contributing a fix via pull request

See the [Contributing Guide](/community/contributing) for details.

---

We're here to help! Don't hesitate to reach out. 🤝
