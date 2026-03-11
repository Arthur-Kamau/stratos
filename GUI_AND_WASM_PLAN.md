# Stratos GUI, HTML Transpilation & WASM Plan

> Status: **Phase 5 In Progress** | Last updated: 2026-03-11

## Decisions

| Question | Decision |
|----------|----------|
| Rendering backend | Skia + SDL2 |
| Architecture | Phase 1: `std/gui` library → Phase 2: `.stui` DSL (compiles to std/gui) |
| HTML compilation | Default: compile to HTML DOM. Flag/config: render to `<canvas>` |
| WASM target | Existing LLVM IR → wasm32 backend |
| GC | Use GC (keep it simple) |
| Priority | GUI library → WASM → HTML transpilation → Reactivity → Routing → Global State → Widgets |
| Reactivity model | Solid-inspired signals: `createEffect()` auto-tracks, `batch()` for grouping |
| Routing model | Both declarative (Router/Route components) and imperative (`navigate()`) |
| State model | Stores with fine-grained subscriptions; Context/Provider for DI |

---

## Phase 1: Native GUI Library (`std/gui`) — COMPLETE

**Goal**: A standard library module that wraps Skia+SDL2, exposing widgets, layout, and rendering to Stratos code.

### 1.1 — Vendor & Build Skia + SDL2
- [x] Add SDL2 as a build dependency (system package: libsdl2-dev, libsdl2-ttf-dev, libsdl2-image-dev)
- [ ] Add Skia as a build dependency — *deferred: SDL2 renderer works, Skia is optional upgrade*
- [x] Update `src/CMakeLists.txt` and `src/build.sh` to link SDL2
- [x] Create `src/src/runtime/gui/` directory for C++ GUI backend code
- [x] Verify build works on Linux — **BUILD SUCCESSFUL**

### 1.2 — C++ GUI Backend (`src/src/runtime/gui/`)
- [x] `SDL2Renderer.cpp` + `Renderer.h` — IRenderer interface with SDL2 backend
- [x] `App.cpp` + `App.h` — Window creation, event loop, theme, SDL2 event translation
- [x] `Event.h` — Full event system (mouse, keyboard, text input, window events)
- [x] `Widget.cpp` + `Widget.h` — Base widget + all layout widgets
- [x] Theme system — Light/dark presets, colors, fonts, spacing, elevation
- [x] `Animation.cpp/h` — Tween system with 4 easings (Linear, EaseIn, EaseOut, EaseInOut)

### 1.3 — Widget Library (C++ implementations)

**Core widgets:**
- [x] `Text` — styled text rendering (font, size, color, weight, alignment)
- [x] `Button` — clickable with label, color states (hover, pressed, disabled), shadow
- [x] `Image` — load and display images (PNG, JPG via SDL2_image)
- [x] `Icon` — Unicode icon/symbol rendering with color and size
- [x] `Box` / `Container` — generic container with background, border, shadow, padding
- [x] `Spacer` — flexible space

**Input widgets:**
- [x] `TextField` — single-line text input with cursor, selection
- [x] `TextArea` — multi-line text input
- [x] `Checkbox` — toggle with label
- [x] `RadioButton` — single-select group with deselectGroup()
- [x] `Switch` — toggle switch
- [x] `Slider` — range selector with thumb and track
- [x] `Dropdown` / `Select` — dropdown menu with items

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
- [x] `Drawer` — slide-in side panel with animation
- [x] `TabBar` — tabbed navigation with active tab tracking
- [x] `Dialog` / `Modal` — overlay dialogs with backdrop
- [x] `Menu` / `ContextMenu` — popup menus with separators

### 1.4 — NativeRegistry Integration
- [x] Create `initGui()` in `GuiNatives.cpp` (separate file, ~85 native functions)
- [x] Register all widget constructors and methods
- [x] Register event callback mechanism (onClick, onChange, onSubmit + closure-based bindClick/bindChange)
- [x] Register layout functions
- [x] Register window lifecycle functions (create, run, quit, setRoot, setTheme, setFPS)
- [x] Add `initGui()` call to `initializeStdlib()`

### 1.5 — Stratos Standard Library Module (`std/gui/`)
- [x] `std/gui/init.st` — unified module (823 lines, 70+ functions, App/State/DerivedState classes)

### 1.6 — State Management
- [x] `State<T>` — reactive state container (get/set, triggers re-render via listener pattern)
- [x] `DerivedState<T>` — computed state with recompute() (in init.st)
- [x] Dirty-flag diffing — markDirty() propagates up, needsRedraw_ in App
- [x] Observer pattern for state → widget binding (listener callbacks)
- [x] State exposed to Stratos via NativeRegistry (__gui_state_create/get/set/watch)

### 1.7 — Examples
- [x] `examples/gui-hello/` — minimal window with text, buttons, inputs
- [x] `examples/gui-counter/` — counter with styled buttons
- [x] `examples/gui-todo/` — todo list with checkboxes, text field, app bar

---

## Phase 2: `.stui` DSL (Declarative UI Language) — COMPLETE

**Goal**: A new file extension `.stui` with declarative UI syntax that compiles down to `std/gui` calls.

### 2.1 — Language Design
- [x] Component declarations, state, view blocks, props, event handlers
- [x] Conditional rendering, list rendering, component composition

### 2.2 — Compiler Pipeline for `.stui`
- [x] `STUIToken.h`, `STUILexer.h/.cpp` — tokenizer
- [x] `STUIAST.h`, `STUIParser.h/.cpp` — parser
- [x] `STUITranspiler.h/.cpp` — STUI AST → Stratos AST
- [x] main.cpp — detect `.stui` extension, route through pipeline
- [x] CMakeLists.txt + build.sh — include stui source files
- [ ] Support `stui` block in `stratos.conf` for UI entry points

### 2.3 — CLI Integration
- [x] `stratos run app.stui` — compile & run
- [x] `stratos check app.stui` — validate syntax
- [x] `stratos compile app.stui` — compile to IR
- [ ] `stratos build` — detect `.stui` files in project
- [ ] `stratos fmt` — format `.stui` files

### 2.4 — Examples
- [x] `examples/stui-hello/`, `stui-counter/`, `stui-todo/`, `stui-dashboard/`

---

## Phase 3: WASM Compilation Target — COMPLETE

**Goal**: Compile Stratos programs to WebAssembly using LLVM IR → wasm32.

### 3.1 — LLVM wasm32 Backend
- [x] wasm32-unknown-wasi + wasm32-unknown-emscripten targets
- [x] CompileTarget enum: NATIVE, WASM, WASM_EMSCRIPTEN

### 3.2 — WASM Runtime Shims
- [x] `src/wasm/runtime.js` — StratosRuntime class (memory, strings, printf, WASI, math)
- [x] println → console.log bridge
- [ ] `src/wasm/stdlib_shims.cpp` — wasm-compatible stdlib (deferred)
- [ ] HTTP client → fetch() API bridge (deferred)

### 3.3 — Build Pipeline
- [x] WasmCompiler.cpp/h — IR → clang → wasm-ld → .wasm (+ Emscripten path)
- [x] Tool auto-detection (clang-14..20, wasm-ld-14..20, emcc)

### 3.4 — Testing
- [x] Test IR generation produces valid wasm32 target triple
- [ ] Test in Node.js / browser (requires toolchain)
- [ ] Benchmark: wasm vs native interpreter

---

## Phase 4: HTML Transpilation (Default Web Target) — MOSTLY COMPLETE

**Goal**: Compile `.stui` files to HTML/CSS/JS DOM elements.

### 4.1 — HTML Code Generator
- [x] HTMLGenerator — traverses STUI AST, emits HTML/CSS/JS
- [x] Widget → HTML element mapping (all 26+ widgets)
- [x] CSS generation with custom properties
- [x] JS generation with event handlers and reactive state

### 4.2 — Reactive Runtime (JS)
- [x] createState(name, initial) with get/set/watch
- [x] Event delegation via addEventListener
- [ ] DOM diffing / patching (direct DOM manipulation for now)
- [ ] Component lifecycle (mount, update, unmount)

### 4.3 — Canvas Rendering Mode (Optional) — DEFERRED
- [ ] CanvasKit / Emscripten canvas rendering
- [ ] `stratos.conf` renderer toggle
- [ ] CLI flag: `--renderer canvas`

### 4.4 — Full Web Build Pipeline
- [x] `stratos compile --target web <file.stui>` → dist/ folder
- [ ] Dev server: `stratos serve <dir>` with hot reload (stretch goal)

### 4.5 — Examples
- [ ] Web examples (hello, counter, todo, canvas, dashboard)

---

## Phase 5: Advanced Reactivity System — COMPLETE

**Goal**: Solid/Svelte-grade reactivity — auto-tracking effects, computed signals, batch updates, and fine-grained DOM updates. Works across native GUI and HTML web target.

**Reference**: Solid.js `createSignal`/`createEffect`/`createMemo`/`batch`, Svelte 5 `$state`/`$derived`/`$effect`

### 5.1 — Signal Primitives (C++ backend)

Upgrade the existing `State` to a full signal system with auto-dependency tracking.

**C++ implementation** (`src/src/runtime/gui/Signals.cpp/h`):
- [ ] `Signal<T>` class — wraps a value, tracks which Effects read it
  - `get()` — returns value, registers current Effect as subscriber
  - `set(value)` — updates value, notifies all subscribed Effects
  - `peek()` — read without tracking (like Solid's `untrack` for a single read)
- [ ] `Effect` class — auto-tracking reactive side-effect
  - Constructor takes a closure, runs it immediately
  - During execution, any Signal.get() calls register the Effect as a subscriber
  - Re-runs automatically when any tracked Signal changes
  - `dispose()` — unsubscribes from all signals, prevents re-execution
- [ ] `Computed<T>` (Memo) — derived signal that caches until dependencies change
  - Lazy evaluation: only recalculates when read AND a dependency changed
  - Diamond dependency problem: correct topological ordering
- [ ] `batch(fn)` — group multiple signal updates, defer Effect re-runs until batch completes
- [ ] `untrack(fn)` — run a function without tracking signal reads
- [ ] Tracking context stack (thread-local `currentEffect` pointer)
- [ ] Cleanup functions — Effects can register cleanup that runs before re-execution

**Native functions to register:**
```
__gui_signal_create(initial: any) -> int
__gui_signal_get(id: int) -> any
__gui_signal_set(id: int, value: any) -> bool
__gui_signal_peek(id: int) -> any
__gui_effect_create(closure: any) -> int
__gui_effect_dispose(id: int) -> bool
__gui_computed_create(closure: any) -> int
__gui_computed_get(id: int) -> any
__gui_batch(closure: any) -> bool
__gui_untrack(closure: any) -> any
__gui_on_cleanup(closure: any) -> bool
```

### 5.2 — Stratos API (`std/gui/init.st` additions)

```stratos
/// Create a reactive signal.
///   val count = Signal(0);
///   count.set(count.get() + 1);
class Signal {
    var id: int;
    constructor(initial: any) { this.id = __gui_signal_create(initial); }
    fn get() any { return __gui_signal_get(this.id); }
    fn set(value: any) void { __gui_signal_set(this.id, value); }
    fn peek() any { return __gui_signal_peek(this.id); }
}

/// Create a computed (memoized) signal.
///   val doubled = Computed(fn() any => count.get() as int * 2);
class Computed {
    var id: int;
    constructor(compute: any) { this.id = __gui_computed_create(compute); }
    fn get() any { return __gui_computed_get(this.id); }
}

/// Run a side-effect that auto-tracks signal dependencies.
///   val dispose = createEffect(fn() void {
///       println("Count is: ${count.get()}");
///   });
fn createEffect(closure: any) int { return __gui_effect_create(closure); }

/// Dispose an effect (stop it from running).
fn disposeEffect(id: int) void { __gui_effect_dispose(id); }

/// Batch multiple signal updates (effects run once at end).
fn batch(closure: any) void { __gui_batch(closure); }

/// Read signals without tracking dependencies.
fn untrack(closure: any) any { return __gui_untrack(closure); }

/// Register cleanup for current effect (runs before re-execution).
fn onCleanup(closure: any) void { __gui_on_cleanup(closure); }
```

### 5.3 — Backward Compatibility — COMPLETE

- [x] Keep existing `State` class working (wraps Signal internally)
- [x] Keep existing `DerivedState` working (wraps Computed internally)
- [x] Keep `bindClick`/`bindChange`/`bindText` working
- [x] Existing examples must not break

### 5.4 — HTML Target Integration — COMPLETE

Update HTMLGenerator's JS output to use the same signal model:

- [x] Generate `createSignal(initial)` → returns `[get, set]` (Solid-style)
- [x] Generate `createEffect(fn)` → auto-subscribes to signals read inside
- [x] Generate `createMemo(fn)` → cached computed
- [x] Generate `batch(fn)` → deferred DOM updates
- [x] Fine-grained DOM updates — only update the specific DOM node bound to a changed signal, not re-render entire tree
- [x] Component expansion — custom components inlined during HTML generation (no more empty divs)
- [x] Event handler body transformation — `count += 1` → `setCount(count() + 1)`

### 5.5 — STUI Transpiler Updates — COMPLETE

- [x] `state count: int = 0;` compiles to `Signal(0)` instead of `State(0)`
- [x] Automatic effect wrapping for view bindings (e.g., `Text("Count: ${count}")` auto-creates an effect in HTML target)
- [x] Computed state: `computed doubled: int = count * 2;` syntax (STUI lexer, parser, AST, transpiler)

### 5.6 — Examples — COMPLETE

- [x] `examples/signals-basic/` — Signal, Computed, Effect demo
- [x] `examples/signals-batch/` — batch updates demo
- [x] `stui-counter/` uses Signal internally (STUI transpiler now emits Signal() for state declarations)

---

## Phase 6: Routing & Navigation — MOSTLY COMPLETE

**Goal**: Client-side routing for single-page apps — both native GUI and HTML web targets. Inspired by Flutter GoRouter + SolidJS Router.

**Reference**: Flutter `Navigator`/`GoRouter`, Compose `NavHost`/`NavController`, SolidJS `@solidjs/router`

### 6.1 — C++ Router Backend (`src/src/runtime/gui/Router.cpp/h`) — COMPLETE

- [x] `Router` class
  - Route table: `vector<RouteEntry>` — path pattern + widget builder
  - Path matching: exact, parameterized (`:id`), wildcard (`*`)
  - Current route state (path, params, query)
  - Navigation stack (history) with push/pop/replace
  - `navigate(path)` — match route, swap widget tree
  - `back()` / `forward()` — history navigation
  - `replace(path)` — replace current route without adding to history
- [x] `RouteEntry` struct — `{ pattern, paramNames, builder, guard? }`
- [x] Route parameter extraction: `/user/:id` → `params["id"] = "42"`
- [x] Query parameter parsing: `/search?q=hello` → `query["q"] = "hello"`
- [ ] Nested routes: `/dashboard/settings` with parent layout
- [x] Route guards: `beforeEnter(from, to) -> bool` for auth checks
- [x] Redirect support: `redirect("/old", "/new")`
- [x] 404 / fallback route: `notFound(builder)`

### 6.2 — Native Functions

```
__gui_router_create() -> int
__gui_router_add_route(routerId: int, path: string, builderId: int) -> bool
__gui_router_add_guard(routerId: int, path: string, guardFn: any) -> bool
__gui_router_set_not_found(routerId: int, builderId: int) -> bool
__gui_router_navigate(routerId: int, path: string) -> bool
__gui_router_back(routerId: int) -> bool
__gui_router_forward(routerId: int) -> bool
__gui_router_replace(routerId: int, path: string) -> bool
__gui_router_get_param(routerId: int, name: string) -> string
__gui_router_get_query(routerId: int, name: string) -> string
__gui_router_get_path(routerId: int) -> string
__gui_router_get_widget(routerId: int) -> int
```

### 6.3 — Stratos API (`std/gui/init.st` additions)

```stratos
/// Create a Router.
///   val router = Router();
///   router.route("/", fn() int => HomePage());
///   router.route("/user/:id", fn() int => UserPage(router.param("id")));
///   router.notFound(fn() int => NotFoundPage());
///   app.root(router.widget());
class Router {
    var id: int;
    constructor() { this.id = __gui_router_create(); }

    fn route(path: string, builder: any) void {
        __gui_router_add_route(this.id, path, builder);
    }

    fn guard(path: string, guardFn: any) void {
        __gui_router_add_guard(this.id, path, guardFn);
    }

    fn notFound(builder: any) void {
        __gui_router_set_not_found(this.id, builder);
    }

    fn navigate(path: string) void { __gui_router_navigate(this.id, path); }
    fn back() void { __gui_router_back(this.id); }
    fn forward() void { __gui_router_forward(this.id); }
    fn replace(path: string) void { __gui_router_replace(this.id, path); }

    fn param(name: string) string { return __gui_router_get_param(this.id, name); }
    fn query(name: string) string { return __gui_router_get_query(this.id, name); }
    fn currentPath() string { return __gui_router_get_path(this.id); }

    fn widget() int { return __gui_router_get_widget(this.id); }
}

/// Convenience: navigation link button
fn Link(text: string, path: string, router: Router) int {
    val btn = Button(text);
    bindClick(btn, fn() void { router.navigate(path); });
    return btn;
}
```

### 6.4 — HTML Target Integration

- [ ] Generate JS router using `window.history` + `popstate` event
- [ ] Hash-based routing fallback (`#/path`) for static hosting
- [ ] URL ↔ route sync (browser URL bar updates on navigate)
- [ ] `<a>` tags for Link widgets with `preventDefault` + pushState

### 6.5 — STUI Syntax — COMPLETE

- [x] Parse `Router`, `Route`, `NotFound` as widget nodes (parser handles PascalCase generically)
- [x] Transpile to Router class API calls (Router → Router(), Route → router.route(), NotFound → router.notFound())
- [x] Support `navigate()` in event handlers

### 6.6 — Examples — COMPLETE

- [x] `examples/routing-basic/` — multi-page app with links
- [x] `examples/routing-params/` — route parameters and query strings
- [ ] `examples/routing-guards/` — auth guard demo

---

## Phase 7: Global State & Context

**Goal**: App-wide state management and scoped state injection (provider/consumer pattern). Inspired by Svelte stores, SolidJS context, Flutter Provider.

**Reference**: Svelte `writable`/`readable`/`derived` stores, SolidJS `createContext`/`useContext`, Flutter `Provider`/`InheritedWidget`, Compose `CompositionLocal`

### 7.1 — Store (C++ backend)

- [x] `Store` class — reactive key-value container with fine-grained subscriptions
  - `get(key)` — read a field (tracks if inside Effect)
  - `set(key, value)` — update a field, notify subscribers of that key only
  - `subscribe(key, callback)` — subscribe to a specific key
- [ ] `ReadonlyStore` — derived store that computes from other stores (like Svelte `derived`)
- [ ] Store ↔ Signal interop — stores built on top of Signal primitives

### 7.2 — Context / Provider (C++ backend)

- [x] `Context<T>` — named context for dependency injection
  - `createContext(name, defaultValue)` → context ID
  - `provide(contextId, value)` — set value for current widget subtree
  - `consume(contextId)` — read nearest ancestor's provided value
- [ ] Widget tree walking — consume() walks up parent_ chain to find provider
- [x] Multiple contexts — theme context, auth context, router context, etc.

### 7.3 — Native Functions

```
// Store
__gui_store_create() -> int
__gui_store_get(id: int, key: string) -> any
__gui_store_set(id: int, key: string, value: any) -> bool
__gui_store_subscribe(id: int, key: string, callback: any) -> int
__gui_store_unsubscribe(subscriptionId: int) -> bool

// Context
__gui_context_create(name: string, defaultValue: any) -> int
__gui_context_provide(contextId: int, value: any, widgetId: int) -> bool
__gui_context_consume(contextId: int) -> any
```

### 7.4 — Stratos API (`std/gui/init.st` additions)

```stratos
/// Global reactive store.
///   val store = Store();
///   store.set("user", "Alice");
///   store.set("count", 0);
///   val name = store.get("user");
class Store {
    var id: int;
    constructor() { this.id = __gui_store_create(); }
    fn get(key: string) any { return __gui_store_get(this.id, key); }
    fn set(key: string, value: any) void { __gui_store_set(this.id, key, value); }
    fn subscribe(key: string, callback: any) int {
        return __gui_store_subscribe(this.id, key, callback);
    }
    fn unsubscribe(subscriptionId: int) void {
        __gui_store_unsubscribe(subscriptionId);
    }
}

/// Scoped value provider for dependency injection.
///   val ThemeCtx = Context("theme", "light");
///   provide(ThemeCtx, "dark", rootWidget);
///   val theme = consume(ThemeCtx);
class Context {
    var id: int;
    constructor(name: string, defaultValue: any) {
        this.id = __gui_context_create(name, defaultValue);
    }
}

fn provide(ctx: Context, value: any, widgetId: int) void {
    __gui_context_provide(ctx.id, value, widgetId);
}

fn consume(ctx: Context) any {
    return __gui_context_consume(ctx.id);
}
```

### 7.5 — HTML Target Integration

- [ ] Generate JS Store class with `Proxy`-based reactivity
- [ ] Generate Context as module-scoped variable with provider/consumer pattern
- [ ] Fine-grained DOM updates per store key

### 7.6 — STUI Syntax

```stui
// Global store
store AppStore {
    user: string = "Guest";
    count: int = 0;
    theme: string = "dark";
}

component App {
    // Provide context to subtree
    provide theme: string = "dark";

    view {
        Column {
            Text("Hello, ${AppStore.user}")
            Button("Increment") {
                onClick: { AppStore.count += 1; }
            }
        }
    }
}
```

- [ ] Parse `store` declaration as global reactive object
- [ ] Parse `provide` in components
- [ ] Transpile to Store/Context API calls

### 7.7 — Examples

- [ ] `examples/global-store/` — shared counter across components
- [ ] `examples/context-theme/` — theme provider/consumer
- [ ] `examples/store-todo/` — todo app with global store

---

## Phase 8: Lifecycle, Gestures & Missing Widgets

**Goal**: Component lifecycle hooks, gesture detection, and commonly-expected widgets from Flutter/Compose.

### 8.1 — Component Lifecycle Hooks

- [ ] `onMount(fn)` — called once after widget is first rendered
- [ ] `onDestroy(fn)` — called when widget is removed from tree
- [ ] `onUpdate(fn)` — called after widget re-renders due to state change
- [ ] C++ implementation: hook into Widget::addChild / removeChild / paint cycle
- [ ] STUI syntax: `onMount { ... }` block inside component
- [ ] HTML target: generate `connectedCallback` / `disconnectedCallback` (or manual lifecycle tracking)

### 8.2 — Gesture Detection

- [ ] `GestureDetector` wrapper widget — catches gesture events on children
- [ ] Supported gestures:
  - `onTap` — single tap (already have onClick)
  - `onDoubleTap` — double tap
  - `onLongPress` — press and hold
  - `onDragStart/onDrag/onDragEnd` — drag gestures
  - `onSwipe(direction)` — swipe left/right/up/down
  - `onPinch` — pinch-to-zoom (if multi-touch available)
- [ ] C++ Event.h: add gesture recognizer state machine
- [ ] Native functions: `__gui_gesture_create()`, `__gui_gesture_set_on_*`
- [ ] Stratos API: `GestureDetector(child, { onDoubleTap: fn() { ... } })`

### 8.3 — Missing Widgets

**Composite / convenience widgets:**

| Widget | Description | Flutter/Compose equiv |
|--------|-------------|-----------------------|
| `Scaffold` | Composite: AppBar + body + drawer + FAB + bottomNav | `Scaffold` / `Scaffold` |
| `Card` | Elevated container with rounded corners and shadow | `Card` / `Card` |
| `Divider` | Horizontal/vertical line separator | `Divider` / `Divider` |
| `Badge` | Small count/dot indicator | `Badge` / `Badge` |
| `Tooltip` | Hover/long-press info popup | `Tooltip` / `TooltipBox` |
| `Chip` | Small labeled element (tag, filter) | `Chip` / `FilterChip` |
| `FAB` | Floating action button (circular, bottom-right) | `FloatingActionButton` / `FloatingActionButton` |
| `BottomNavBar` | Bottom navigation bar with icons + labels | `BottomNavigationBar` / `NavigationBar` |
| `ProgressBar` | Linear progress indicator | `LinearProgressIndicator` / `LinearProgressIndicator` |
| `CircularProgress` | Spinning/circular progress | `CircularProgressIndicator` / `CircularProgressIndicator` |
| `SnackBar` | Temporary message bar at bottom | `SnackBar` / `Snackbar` |
| `ExpansionPanel` | Collapsible/expandable section | `ExpansionTile` / `AnimatedVisibility` |
| `Wrap` | Flow layout that wraps to next line | `Wrap` / `FlowRow` |
| `AspectRatio` | Constrains child to aspect ratio | `AspectRatio` / `aspectRatio` modifier |

Implementation for each:
- [ ] C++ widget class in Widget.h/Widget.cpp
- [ ] Native function registration in GuiNatives.cpp
- [ ] Stratos helper function in std/gui/init.st
- [ ] HTML element mapping in HTMLGenerator
- [ ] STUI transpiler support

### 8.4 — Error Boundaries

- [ ] `ErrorBoundary` widget — catches errors in child widget tree rendering
- [ ] `fallback` widget shown when error occurs
- [ ] Error propagation: child error → ErrorBoundary catches → shows fallback
- [ ] `onError(fn(error))` callback for logging
- [ ] STUI syntax: `ErrorBoundary { ... } fallback { Text("Something went wrong") }`

### 8.5 — Responsive / MediaQuery

- [ ] `MediaQuery` — provides screen size info to children
  - `width`, `height` of window
  - `orientation` (portrait/landscape)
  - `breakpoint` (mobile/tablet/desktop thresholds)
- [ ] `Responsive` widget — renders different children based on breakpoints
  ```stratos
  Responsive({
      mobile: fn() int => MobileLayout(),
      tablet: fn() int => TabletLayout(),
      desktop: fn() int => DesktopLayout(),
  })
  ```
- [ ] CSS media queries in HTML target

---

## Phase 9: Polish & Developer Experience

**Goal**: Fill remaining gaps for a production-ready GUI framework.

### 9.1 — Form System

- [ ] `Form` widget — groups input widgets, tracks validity
- [ ] `FormField` wrapper — adds label, error message, validation rules
- [ ] Built-in validators: `required`, `minLength`, `maxLength`, `email`, `regex`, `custom(fn)`
- [ ] `form.validate()` → bool, `form.reset()`, `form.values()` → map
- [ ] Submit handling: `form.onSubmit(fn(values) { ... })`

### 9.2 — Async Widget Support

- [ ] `FutureBuilder(future, { loading: ..., success: fn(data) => ..., error: fn(e) => ... })`
- [ ] `Suspense(fallback: LoadingWidget()) { AsyncContent() }` — shows loading while async children resolve
- [ ] Integration with Stratos async/await

### 9.3 — Transition Animations

- [ ] `AnimatedContainer` — smoothly animates size, color, border changes
- [ ] `FadeTransition` — opacity animation
- [ ] `SlideTransition` — slide in/out
- [ ] `ScaleTransition` — scale up/down
- [ ] Route transitions — animate between pages on navigation
- [ ] Hero animations — shared element transitions between routes

### 9.4 — Remaining Infrastructure

- [ ] `stratos serve <dir>` — dev server with hot reload for web target
- [ ] `stratos fmt` for `.stui` files
- [ ] `stratos build` — detect and compile `.stui` files in project
- [ ] `stui` block in `stratos.conf`
- [ ] Web examples: hello, counter, todo, canvas, dashboard

---

## Implementation Order

```
Phase 1    Native GUI Library (std/gui)               ✅ COMPLETE
Phase 2    .stui DSL (Declarative UI Language)         ✅ COMPLETE
Phase 3    WASM Compilation Target                     ✅ COMPLETE
Phase 4    HTML Transpilation (Web Target)             ✅ MOSTLY COMPLETE
  ↓
Phase 5    Advanced Reactivity System                  ✅ COMPLETE
  5.1  Signal/Effect/Computed C++ backend             ✅
  5.2  Stratos API (Signal, Computed, createEffect, batch)  ✅
  5.3  Backward compatibility (State wraps Signal)    ✅
  5.4  HTML target JS signal runtime                  ✅
  5.5  STUI transpiler updates                        ✅
  5.6  Examples                                       ✅
  ↓
Phase 6    Routing & Navigation                       🔄 MOSTLY COMPLETE
  6.1  C++ Router backend                             ✅
  6.2  Native functions                               ✅
  6.3  Stratos Router API                             ✅
  6.4  HTML target (history API + hash routing)        ⬜ TODO
  6.5  STUI Router/Route syntax                       ✅
  6.6  Examples                                       ✅
  ↓
Phase 7    Global State & Context                    MOSTLY COMPLETE
  7.1  Store C++ backend                              ✅
  7.2  Context/Provider C++ backend                   ✅
  7.3  Native functions                               ✅
  7.4  Stratos Store/Context API                      ✅
  7.5  HTML target integration                        ⬜ TODO
  7.6  STUI store/provide syntax                      ⬜ TODO
  7.7  Examples                                       ✅
  ↓
Phase 8    Lifecycle, Gestures & Missing Widgets
  8.1  Lifecycle hooks (onMount, onDestroy, onUpdate)
  8.2  Gesture detection (drag, swipe, long press)
  8.3  Missing widgets (Scaffold, Card, FAB, BottomNav, etc.)
  8.4  Error boundaries
  8.5  Responsive / MediaQuery
  ↓
Phase 9    Polish & DX
  9.1  Form system
  9.2  Async widget support (FutureBuilder, Suspense)
  9.3  Transition animations
  9.4  Infrastructure (serve, fmt .stui, build .stui)
```

## Files to Create/Modify (Phases 5-9)

### New Files
| File | Phase | Purpose |
|------|-------|---------|
| `src/include/stratos/gui/Signals.h` | 5 | Signal, Effect, Computed classes |
| `src/src/runtime/gui/Signals.cpp` | 5 | Signal system implementation |
| `src/include/stratos/gui/Router.h` | 6 | Router class |
| `src/src/runtime/gui/Router.cpp` | 6 | Router implementation |
| `src/include/stratos/gui/Store.h` | 7 | Store and Context classes |
| `src/src/runtime/gui/Store.cpp` | 7 | Store implementation |
| `src/include/stratos/gui/Gestures.h` | 8 | Gesture recognizer |
| `src/src/runtime/gui/Gestures.cpp` | 8 | Gesture implementation |
| `examples/signals-basic/` | 5 | Signal demo |
| `examples/routing-basic/` | 6 | Router demo |
| `examples/global-store/` | 7 | Store demo |

### Modified Files
| File | Phases | Change |
|------|--------|--------|
| `src/src/runtime/gui/GuiNatives.cpp` | 5,6,7,8 | Register new native functions |
| `src/src/runtime/NativeRegistry.cpp` | 5,6,7,8 | Call new init functions |
| `std/gui/init.st` | 5,6,7,8 | Add Signal, Router, Store, new widgets |
| `src/src/stui/STUITranspiler.cpp` | 5,6,7 | Emit Signal instead of State, Router/Route nodes |
| `src/include/stratos/STUITranspiler.h` | 5,6 | New transpiler methods |
| `src/include/stratos/gui/Widget.h` | 8 | New widget classes |
| `src/src/runtime/gui/Widget.cpp` | 8 | New widget implementations |
| `src/build.sh` | 5,6,7,8 | Add new .cpp files to build |
| `src/CMakeLists.txt` | 5,6,7,8 | Add new source files |
