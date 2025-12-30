---
title: Contributing
description: How to contribute to the Stratos programming language project
---

# Contributing to Stratos

There are many ways to contribute to Stratos (Language, Standard Library, Compiler, or Tooling). We welcome contributions from the community!

## Ways to Contribute

### Report Bugs and Issues

- Submit bugs and help us verify fixes as they are checked in
- Use [GitHub Issues](https://github.com/stratos-lang/stratos/issues) to report problems
- Provide clear reproduction steps and environment details

### Code Contributions

- Contribute bug fixes
- Implement new features
- Improve performance
- Enhance documentation

### Community Engagement

- Review source code changes
- Engage with other Stratos users and developers on [StackOverflow](https://stackoverflow.com/questions/tagged/stratos)
- Help each other in the [Stratos Community Discord](https://discord.gg/stratos)
- Join the #Stratos discussion on [Twitter](https://twitter.com/stratoslang)

## Getting Started

### 1. Fork the Repository

```bash
git clone https://github.com/stratos-lang/stratos.git
cd stratos
```

### 2. Set Up Development Environment

Follow the build instructions in the main README to set up your development environment.

### 3. Find an Issue

- Check the [issue tracker](https://github.com/stratos-lang/stratos/issues) for open issues
- Look for issues labeled `good first issue` or `help wanted`
- Comment on the issue to let others know you're working on it

### 4. Make Your Changes

- Create a new branch for your work
- Write clear, commented code
- Follow the existing code style
- Add tests for new functionality
- Update documentation as needed

### 5. Submit a Pull Request

- Push your branch to your fork
- Create a pull request with a clear description
- Reference any related issues
- Wait for review and address feedback

## Development Guidelines

### Code Style

- Follow existing code conventions in the codebase
- Use meaningful variable and function names
- Add comments for complex logic
- Keep functions focused and modular

### Testing

- Write tests for new features
- Ensure all existing tests pass
- Run the test suite before submitting:
  ```bash
  stratos test
  ```

### Commit Messages

- Use clear, descriptive commit messages
- Reference issue numbers when applicable
- Follow conventional commit format when possible

Example:
```
feat: Add pipe operator support

Implements the pipe operator (|>) for function composition.
Resolves #123
```

### Documentation

- Update relevant documentation for new features
- Add code examples where helpful
- Keep documentation clear and concise

## Project Areas

### Compiler

Located in `interpreter/C++/` - the Stratos compiler implementation.

**Contribute by:**
- Fixing compiler bugs
- Improving error messages
- Optimizing compilation performance
- Implementing language features

### Standard Library

Located in `std/` - the Stratos standard library.

**Contribute by:**
- Adding new modules
- Improving existing functions
- Writing documentation
- Adding examples

### Language Design

Located in `design/language/` - language specification and design documents.

**Contribute by:**
- Proposing new language features
- Reviewing language proposals
- Improving specifications
- Providing feedback on design decisions

### Tooling

Located in `tooling/` - IDE plugins and development tools.

**Contribute by:**
- Improving VS Code extension
- Adding features to editor plugins
- Creating new development tools
- Enhancing debugging support

## Community Guidelines

### Code of Conduct

- Be respectful and inclusive
- Welcome newcomers
- Provide constructive feedback
- Focus on what is best for the community

### Communication

- Use clear, professional language
- Be patient with others
- Ask questions when unsure
- Share knowledge generously

## Resources

- [Language Specification](https://github.com/stratos-lang/stratos/tree/master/design/language)
- [Compiler Documentation](https://github.com/stratos-lang/stratos/tree/master/compiler)
- [Standard Library Reference](/reference/stdlib)
- [CLI Reference](/reference/cli)

## Getting Help

If you need help contributing:

- Ask in [GitHub Discussions](https://github.com/stratos-lang/stratos/discussions)
- Join our [Discord server](https://discord.gg/stratos)
- Email the maintainers at contact@araizen.com

## Recognition

Contributors will be:
- Listed in the project's contributor list
- Credited in release notes for significant contributions
- Acknowledged in the community

## License

By contributing to Stratos, you agree that your contributions will be licensed under:
- **Stratos Compiler**: Apache License 2.0
- **Standard Library**: MIT License
- **Example Code**: MIT License

Thank you for contributing to Stratos! 🚀
