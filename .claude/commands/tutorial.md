# Stratos Tutorial App

Work with the interactive Stratos tutorial web application.

## Arguments
- $ARGUMENTS: Action (e.g., "dev", "build", "add-lesson", "inspect")

## Instructions

The Stratos tutorial is a SvelteKit web application at `stratos-tutorial/` that provides an interactive learning experience with Monaco editor integration.

### Tech Stack
- **Framework**: SvelteKit
- **Editor**: Monaco Editor (VS Code's editor component)
- **Styling**: PostCSS
- **Package Manager**: bun (bun.lock present)
- **Build**: Vite

### Actions

**"dev"** — Start the development server:
```bash
cd stratos-tutorial && bun install && bun run dev
```

**"build"** — Build for production:
```bash
cd stratos-tutorial && bun run build
```

**"add-lesson"** — Add a new tutorial lesson:
1. Read existing lessons in `stratos-tutorial/src/` to understand the structure
2. Create a new lesson component following the existing pattern
3. Add it to the lesson navigation/routing

**"inspect"** — Explore the tutorial codebase:
1. Read `stratos-tutorial/package.json` for dependencies and scripts
2. Read `stratos-tutorial/svelte.config.js` for SvelteKit config
3. Explore `stratos-tutorial/src/` for components and routes
4. Check `stratos-tutorial/static/` for static assets

### Key Files
- `stratos-tutorial/package.json` — Dependencies and scripts
- `stratos-tutorial/svelte.config.js` — SvelteKit configuration
- `stratos-tutorial/vite.config.ts` — Vite configuration
- `stratos-tutorial/src/` — Source code (routes, components, lib)
- `stratos-tutorial/static/` — Static assets
