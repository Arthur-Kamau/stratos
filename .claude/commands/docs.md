# Stratos Documentation

Work with the Stratos documentation site.

## Arguments
- $ARGUMENTS: Action (e.g., "dev", "build", "add-page", "search", "update")

## Instructions

The documentation is at `docs/` and uses VitePress (or similar static site generator).

### Structure
```
docs/
├── src/               # Documentation source files
├── guides/            # User guides
├── developer/         # Developer documentation
├── user/              # User-facing docs
├── design/            # Language design documents
├── devtools/          # DevTools documentation
├── changelog/         # Release changelogs
├── package.json       # Dependencies
└── pnpm-lock.yaml     # Lock file (uses pnpm)
```

### Actions

**"dev"** — Start documentation dev server:
```bash
cd docs && pnpm install && pnpm run dev
```

**"build"** — Build documentation:
```bash
cd docs && pnpm run build
```

**"add-page <topic>"** — Add a new documentation page:
1. Read existing docs structure
2. Create new markdown file in the appropriate directory
3. Add to navigation/sidebar configuration

**"search <term>"** — Search documentation content:
1. Search across all markdown files in docs/ for the term
2. Return relevant sections

**"update"** — Check for outdated documentation:
1. Compare docs content against current codebase features
2. Identify any documented features that have changed
3. Flag any new features missing from docs

### Key Documentation Areas
- `docs/guides/` — Getting started, installation, tutorials
- `docs/developer/` — Compiler internals, contributing
- `docs/user/` — Language reference, stdlib API
- `docs/design/` — Language design decisions and specs
- `docs/devtools/` — DevTools usage and development
- `docs/changelog/` — Version history
