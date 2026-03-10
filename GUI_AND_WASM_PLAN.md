# Stratos GUI, HTML Transpilation & WASM Plan

> Status: **Phase 2 Complete** | Last updated: 2026-03-10

## Decisions

| Question | Decision |
|----------|----------|
| Rendering backend | Skia + SDL2 |
| Architecture | Phase 1: `std/gui` library → Phase 2: `.stui` DSL (compiles to std/gui) |
| HTML compilation | Default: compile to HTML DOM. Flag/config: render to `<canvas>` |
| WASM target | Existing LLVM IR → wasm32 backend |
| GC | Use GC (keep it simple) |
| Priority | GUI library → WASM → HTML transpilation |

---

## Phase 1: Native GUI Library (`std/gui`)

**Goal**: A standard library module that wraps Skia+SDL2, exposing widgets, layout, and rendering to Stratos code.

### 1.1 — Vendor & Build Skia + SDL2
- [x] Add SDL2 as a build dependency (system package: libsdl2-dev, libsdl2-ttf-dev, libsdl2-image-dev)
- [ ] Add Skia as a build dependency (pre-built binaries or build from source) — *deferred: SDL2 renderer works now, Skia backend structured as optional upgrade*
- [x] Update `src/CMakeLists.txt` and `src/build.sh` to link SDL2
- [x] Create `src/src/runtime/gui/` directory for C++ GUI backend code
- [x] Verify build works on Linux — **BUILD SUCCESSFUL**

### 1.2 — C++ GUI Backend (`src/src/runtime/gui/`)
Core rendering layer in C++, called from NativeRegistry.

- [x] `SDL2Renderer.cpp` + `Renderer.h` — IRenderer interface with SDL2 backend (drawRect, drawText, drawImage, rounded rects, shadows, circles)
- [x] `App.cpp` + `App.h` — Window creation, event loop, theme, SDL2 event translation
- [x] `Event.h` — Full event system (mouse, keyboard, text input, window events)
- [x] `Widget.cpp` + `Widget.h` — Base widget + all layout widgets (Row, Column, Stack, Grid, Center, Padding, ScrollView, Spacer)
- [x] Theme system — Light/dark presets, colors, fonts, spacing, elevation
- [ ] `Animation.cpp/h` — Basic animation system (tweens, transitions)

### 1.3 — Widget Library (C++ implementations)
Each widget implemented in C++ and registered in NativeRegistry.

**Core widgets:**
- [x] `Text` — styled text rendering (font, size, color, weight, alignment)
- [x] `Button` — clickable with label, color states (hover, pressed, disabled), shadow
- [x] `Image` — load and display images (PNG, JPG via SDL2_image)
- [ ] `Icon` — icon rendering (built-in icon set or custom)
- [x] `Box` / `Container` — generic container with background, border, shadow, padding
- [x] `Spacer` — flexible space

**Input widgets:**
- [x] `TextField` — single-line text input with cursor, selection
- [ ] `TextArea` — multi-line text input
- [x] `Checkbox` — toggle with label
- [ ] `RadioButton` — single-select group
- [x] `Switch` — toggle switch
- [x] `Slider` — range selector with thumb and track
- [ ] `Dropdown` / `Select` — dropdown menu

**Layout widgets:**
- [x] `Row` — horizontal layout (flexbox-like, main/cross axis alignment)
- [x] `Column` — vertical layout (flexbox-like, main/cross axis alignment)
- [x] `Stack` — z-axis stacking (overlapping)
- [x] `Grid` — 2D grid layout
- [x] `ScrollView` — scrollable container with scrollbar
- [x] `ListView` — virtualized list for large datasets
- [x] `Padding` / `Center` — spacing wrappers

**Navigation:**
- [x] `AppBar` — top bar with title and actions
- [ ] `Drawer` — slide-in side panel
- [ ] `TabBar` — tabbed navigation
- [x] `Dialog` / `Modal` — overlay dialogs with backdrop
- [ ] `Menu` / `ContextMenu` — popup menus

### 1.4 — NativeRegistry Integration
- [x] Create `initGui()` in `GuiNatives.cpp` (separate file)
- [x] Register all widget constructors and methods as native functions (~50 functions)
- [ ] Register event callback mechanism (onClick, onChange, onSubmit, etc.) — *placeholder exists, needs interpreter callback bridge*
- [x] Register layout functions
- [x] Register window lifecycle functions (create, run, quit, setRoot, setTheme, setFPS)
- [x] Add `initGui()` call to `initializeStdlib()`

### 1.5 — Stratos Standard Library Module (`std/gui/`)
- [x] `std/gui/init.st` — unified module with App class, all widget constructors, style helpers, cleanup functions
- *Decided against splitting into multiple .st files — single init.st is cleaner and matches other std modules*

**API design (target usage):**
```stratos
package main;
import "std/gui";

fn main() {
    val app = gui.App("My App", 800, 600);

    val counter = gui.State(0);

    app.root(
        gui.Column({
            alignment: "center",
            spacing: 16,
        }, [
            gui.Text("Count: ${counter.get()}", { fontSize: 24 }),
            gui.Row({ spacing: 8 }, [
                gui.Button("Increment", { onClick: fn() { counter.set(counter.get() + 1); } }),
                gui.Button("Reset", { onClick: fn() { counter.set(0); } }),
            ]),
        ])
    );

    app.run();
}
```

### 1.6 — State Management
- [x] `State<T>` — reactive state container in C++ (get/set, triggers re-render via listener pattern)
- [ ] `DerivedState<T>` — computed from other states
- [x] Dirty-flag diffing — markDirty() propagates up, needsRedraw_ in App
- [x] Observer pattern for state → widget binding (listener callbacks)
- [ ] Expose State to Stratos via NativeRegistry (currently C++ only)

### 1.7 — Examples
- [x] `examples/gui-hello/` — minimal window with text, buttons, inputs
- [x] `examples/gui-counter/` — counter with styled buttons
- [ ] `examples/gui-form/` — form with text fields, checkboxes, submit
- [ ] `examples/gui-layout/` — layout showcase (row, column, grid)
- [x] `examples/gui-todo/` — todo list with checkboxes, text field, app bar
- [ ] `examples/gui-theme/` — dark/light theme switching

---

## Phase 2: `.stui` DSL (Declarative UI Language)

**Goal**: A new file extension `.stui` with declarative UI syntax (inspired by Jetpack Compose / QML) that compiles down to `std/gui` calls.

### 2.1 — Language Design

**Target syntax:**
```stui
// counter.stui
import "std/gui";

component Counter {
    state count: int = 0;

    view {
        Column(alignment: "center", spacing: 16) {
            Text("Count: ${count}") {
                fontSize: 24;
                color: theme.primary;
            }

            Row(spacing: 8) {
                Button("Increment") {
                    onClick: { count += 1; }
                }
                Button("Reset") {
                    onClick: { count = 0; }
                }
            }
        }
    }
}

component App {
    view {
        Window(title: "Counter App", width: 800, height: 600) {
            Counter()
        }
    }
}
```

**Key DSL features:**
- `component` — defines a reusable UI component (compiles to a class)
- `state` — reactive state variable (compiles to `gui.State<T>`)
- `view { }` — declarative widget tree (compiles to nested `gui.*` calls)
- Widget properties as key-value blocks
- Event handlers inline
- Component composition (call components like functions)
- Conditional rendering: `if`/`when` inside `view`
- List rendering: `for item in list { WidgetFor(item) }`

### 2.2 — Compiler Pipeline for `.stui`

```
.stui → STUI Lexer → STUI Tokens → STUI Parser → STUI AST
  → STUI Transpiler → Stratos AST (std/gui calls) → normal pipeline
```

- [x] `src/include/stratos/STUIToken.h` — STUI-specific token types
- [x] `src/include/stratos/STUILexer.h` — tokenizer for `.stui` syntax
- [x] `src/src/stui/STUILexer.cpp` — implementation (keywords, strings, interpolation, comments)
- [x] `src/include/stratos/STUIAST.h` — STUI AST nodes (WidgetNode, ComponentDecl, StateDecl, PropDecl, etc.)
- [x] `src/include/stratos/STUIParser.h` — parser for component/view/state declarations
- [x] `src/src/stui/STUIParser.cpp` — recursive descent parser (widgets, properties, events, expressions)
- [x] `src/include/stratos/STUITranspiler.h` — converts STUI AST → Stratos AST
- [x] `src/src/stui/STUITranspiler.cpp` — emits `std/gui` calls, generates main() from App component
- [x] Update `src/src/main.cpp` — detect `.stui` extension, route through STUI pipeline
- [x] Update `src/CMakeLists.txt` — include stui source files
- [x] Update `src/build.sh` — include stui source files
- [ ] Support `stui` block in `stratos.conf` for UI entry points

### 2.3 — CLI Integration
- [x] `stratos run app.stui` — compile & run a `.stui` file
- [ ] `stratos build` — detect `.stui` files in project, compile them
- [ ] `stratos fmt` — format `.stui` files
- [x] `stratos check app.stui` — validate `.stui` syntax and list components
- [x] `stratos compile app.stui` — compile `.stui` to IR

### 2.4 — Examples
- [x] `examples/stui-hello/` — minimal .stui app (Text, Button, Column)
- [x] `examples/stui-counter/` — counter with state, increment/decrement/reset
- [x] `examples/stui-todo/` — todo app with TextField, AppBar, event handlers
- [x] `examples/stui-dashboard/` — multi-component dashboard (StatCard, Sidebar, Grid)

---

## Phase 3: WASM Compilation Target

**Goal**: Compile Stratos programs to WebAssembly using existing LLVM IR → wasm32.

### 3.1 — LLVM wasm32 Backend
- [ ] Update `IRGenerator.cpp` — add `wasm32-unknown-unknown` target triple
- [ ] Add `--target wasm` flag to `stratos compile`
- [ ] Handle wasm32-specific calling conventions and data layout
- [ ] Generate wasm-compatible LLVM IR (no system calls, use imports)
- [ ] Add `wasm32-unknown-emscripten` target for Emscripten-backed builds

### 3.2 — WASM Runtime Shims
- [ ] `src/wasm/runtime.js` — JS glue code (memory management, string passing)
- [ ] `src/wasm/stdlib_shims.cpp` — wasm-compatible std function implementations
- [ ] Map `println` → `console.log`, file I/O → browser APIs, etc.
- [ ] HTTP client → `fetch()` API bridge

### 3.3 — Build Pipeline
- [ ] `.ll` → `llc` (wasm32 target) → `.o` → `wasm-ld` → `.wasm`
- [ ] Or: `.ll` → Emscripten → `.wasm` + `.js` + `.html`
- [ ] Add `stratos compile --target wasm <dir>` command
- [ ] Generate HTML shell that loads the `.wasm` module
- [ ] Support `stratos.conf` target option:
  ```hocon
  build {
      target = "wasm"  // or "native" (default)
  }
  ```

### 3.4 — Testing
- [ ] Test basic programs compile to valid wasm
- [ ] Test in Node.js (via `node --experimental-wasm-modules`)
- [ ] Test in browser (serve HTML shell, verify execution)
- [ ] Benchmark: wasm vs native interpreter

---

## Phase 4: HTML Transpilation (Default Web Target)

**Goal**: Compile `std/gui` widgets and `.stui` files to HTML/CSS/JS DOM elements by default, with canvas rendering as an option.

### 4.1 — HTML Code Generator
- [ ] `src/src/codegen/HTMLGenerator.cpp/h` — traverses GUI AST, emits HTML/CSS/JS
- [ ] Widget → HTML element mapping:

| Stratos Widget | HTML Output |
|---------------|-------------|
| `Text` | `<span>` / `<p>` / `<h1>`-`<h6>` |
| `Button` | `<button>` |
| `TextField` | `<input type="text">` |
| `TextArea` | `<textarea>` |
| `Checkbox` | `<input type="checkbox">` |
| `RadioButton` | `<input type="radio">` |
| `Switch` | `<label class="switch"><input type="checkbox">` |
| `Slider` | `<input type="range">` |
| `Dropdown` | `<select>` |
| `Image` | `<img>` |
| `Row` | `<div style="display:flex;flex-direction:row">` |
| `Column` | `<div style="display:flex;flex-direction:column">` |
| `Grid` | `<div style="display:grid">` |
| `Stack` | `<div style="position:relative">` + absolute children |
| `ScrollView` | `<div style="overflow:auto">` |
| `Container`/`Box` | `<div>` |
| `AppBar` | `<header>` / `<nav>` |
| `Dialog` | `<dialog>` |
| `ListView` | `<ul>` / virtual scroll div |

- [ ] CSS generation — map theme/style properties to CSS
- [ ] JS generation — event handlers, state reactivity (minimal runtime)
- [ ] Generate `index.html` + `styles.css` + `app.js` bundle

### 4.2 — Reactive Runtime (JS)
- [ ] `src/web/runtime.js` — minimal reactive framework (~2KB)
  - State management (Proxy-based or signal-based reactivity)
  - DOM diffing / patching (or direct DOM manipulation)
  - Event delegation
  - Component lifecycle (mount, update, unmount)

### 4.3 — Canvas Rendering Mode (Optional)
- [ ] Compile Skia to WASM (via CanvasKit / Skia WASM build)
- [ ] Or: Use SDL2 + Emscripten → `<canvas>` rendering
- [ ] Toggle via `stratos.conf`:
  ```hocon
  build {
      target = "web"
      web {
          renderer = "html"    // default: DOM elements
          // renderer = "canvas"  // Skia/SDL canvas rendering
      }
  }
  ```
- [ ] Toggle via CLI flag: `stratos compile --target web --renderer canvas`

### 4.4 — Full Web Build Pipeline
```
.st/.stui → Parse → GUI AST → HTMLGenerator → index.html + app.js + styles.css
                              → OR: WASM + CanvasKit → index.html + app.wasm
```

- [ ] `stratos compile --target web <dir>` — produces `dist/` folder
- [ ] Dev server: `stratos serve <dir>` — local dev server with hot reload (stretch goal)

### 4.5 — Examples
- [ ] `examples/web-hello/` — hello world in browser (HTML mode)
- [ ] `examples/web-counter/` — counter app in browser
- [ ] `examples/web-todo/` — todo app (HTML mode)
- [ ] `examples/web-canvas/` — same app in canvas mode
- [ ] `examples/web-dashboard/` — multi-component web app

---

## Implementation Order

```
Phase 1.1  Vendor Skia + SDL2, update build system
Phase 1.2  C++ GUI backend (Window, Renderer, EventLoop)
Phase 1.3  Core widgets (Text, Button, Box, Row, Column)
Phase 1.4  NativeRegistry integration
Phase 1.5  std/gui Stratos module
Phase 1.6  State management & reactivity
Phase 1.7  Remaining widgets + examples
  ↓
Phase 2.1  Design .stui syntax specification
Phase 2.2  STUI Lexer + Parser + Transpiler
Phase 2.3  CLI integration (.stui support)
Phase 2.4  .stui examples
  ↓
Phase 3.1  LLVM IR wasm32 target triple
Phase 3.2  WASM runtime shims
Phase 3.3  Build pipeline (ll → wasm)
Phase 3.4  Testing & verification
  ↓
Phase 4.1  HTML code generator (widget → DOM mapping)
Phase 4.2  Reactive JS runtime
Phase 4.3  Canvas rendering mode (CanvasKit/Emscripten)
Phase 4.4  Full web build pipeline
Phase 4.5  Web examples
```

## Files to Create/Modify

### New Files
| File | Purpose |
|------|---------|
| `src/src/runtime/gui/Window.cpp/h` | SDL2 window + Skia surface |
| `src/src/runtime/gui/Renderer.cpp/h` | Skia canvas rendering |
| `src/src/runtime/gui/EventLoop.cpp/h` | SDL2 event dispatch |
| `src/src/runtime/gui/Widget.cpp/h` | Base widget class |
| `src/src/runtime/gui/Layout.cpp/h` | Layout engine |
| `src/src/runtime/gui/Theme.cpp/h` | Theming system |
| `src/src/runtime/gui/Animation.cpp/h` | Animation system |
| `src/src/runtime/gui/widgets/*.cpp` | Individual widget implementations |
| `std/gui/init.st` | GUI module entry point |
| `std/gui/widgets.st` | Widget classes |
| `std/gui/layout.st` | Layout classes |
| `std/gui/theme.st` | Theme/styling |
| `std/gui/events.st` | Event types |
| `std/gui/app.st` | App class |
| `src/src/stui/STUILexer.cpp` | .stui tokenizer |
| `src/src/stui/STUIParser.cpp` | .stui parser |
| `src/src/stui/STUITranspiler.cpp` | .stui → Stratos transpiler |
| `src/include/stratos/STUILexer.h` | .stui lexer header |
| `src/include/stratos/STUIParser.h` | .stui parser header |
| `src/include/stratos/STUITranspiler.h` | .stui transpiler header |
| `src/src/codegen/HTMLGenerator.cpp/h` | Widget → HTML generator |
| `src/wasm/runtime.js` | WASM JS glue |
| `src/web/runtime.js` | Reactive DOM runtime |

### Modified Files
| File | Change |
|------|--------|
| `src/CMakeLists.txt` | Add Skia, SDL2 deps, new source files |
| `src/build.sh` | Link Skia + SDL2 |
| `src/src/runtime/NativeRegistry.cpp` | Add `initGui()` |
| `src/src/codegen/IRGenerator.cpp` | Add wasm32 target triple |
| `src/src/main.cpp` | Handle `.stui` extension, `--target wasm/web` flags |
| `src/include/stratos/ProjectConfig.h` | Add `target`, `web.renderer` config fields |
| `src/src/config/ProjectConfig.cpp` | Parse new config fields |
