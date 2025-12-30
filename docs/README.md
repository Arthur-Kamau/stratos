# Stratos Documentation

This directory contains the VuePress-based documentation for the Stratos programming language.

## Setup

Install dependencies:

```bash
npm install
```

## Development

Start the development server:

```bash
npm run docs:dev
```

The documentation will be available at `http://localhost:8080`.

## Build

Build the static site:

```bash
npm run docs:build
```

The built site will be in `src/.vuepress/dist/`.

## Project Structure

```
docs/
├── package.json           # Node.js dependencies and scripts
├── src/                   # Documentation source files
│   ├── README.md         # Homepage
│   ├── guide/            # User guides
│   │   ├── README.md
│   │   ├── getting-started.md
│   │   ├── installation.md
│   │   ├── basics.md
│   │   ├── control-flow.md
│   │   ├── functions.md
│   │   ├── null-safety.md
│   │   ├── oop.md
│   │   ├── packages.md
│   │   ├── async.md
│   │   ├── error-handling.md
│   │   └── dependency-management.md
│   ├── examples/         # Code examples
│   │   ├── README.md
│   │   ├── basics.md
│   │   ├── control-flow.md
│   │   ├── functions.md
│   │   ├── null-safety.md
│   │   ├── oop.md
│   │   └── packages.md
│   ├── reference/        # API reference
│   │   ├── cli.md
│   │   └── stdlib.md
│   └── .vuepress/        # VuePress configuration
│       ├── config.js     # Site configuration
│       └── public/       # Static assets
│           └── images/
│               └── logo.svg
```

## Writing Documentation

### Markdown Extensions

VuePress supports several markdown extensions:

#### Custom Containers

```markdown
::: tip
This is a tip
:::

::: warning
This is a warning
:::

::: danger
This is a danger warning
:::

::: info
This is an info box
:::
```

#### Code Blocks with Syntax Highlighting

```markdown
\`\`\`stratos
fn main() {
    print("Hello, Stratos!");
}
\`\`\`
```

#### Tables

```markdown
| Feature | Description |
|---------|-------------|
| Type Safety | Strong static typing |
| Async | Native async/await |
```

### Linking Between Pages

Use relative paths to link between documentation pages:

```markdown
[Getting Started](/guide/getting-started)
[Examples](/examples/)
[CLI Reference](/reference/cli)
```

## Deployment

### GitHub Pages

1. Build the site:

```bash
npm run docs:build
```

2. Deploy the `src/.vuepress/dist/` directory to GitHub Pages.

### Custom Server

The built site in `src/.vuepress/dist/` is a static site that can be served by any web server:

```bash
cd src/.vuepress/dist
python3 -m http.server 8000
```

## Contributing

When adding new documentation:

1. Create the markdown file in the appropriate directory
2. Add it to the sidebar in `src/.vuepress/config.js`
3. Test locally with `npm run docs:dev`
4. Build and verify with `npm run docs:build`

## License

MIT License - see the main project LICENSE file.
