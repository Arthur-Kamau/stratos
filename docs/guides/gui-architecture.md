---
title: GUI Architecture Patterns
description: How to structure Stratos GUI apps with clean separation of logic and UI
---

# GUI Architecture Patterns

This guide covers how to structure Stratos GUI applications to keep logic separate from UI, avoid spaghetti code, and scale beyond simple demos. It covers mixing `.st` (logic) and `.stui` (UI) files in a single project.

## The Problem

A small counter app works fine as a single `.stui` file with inline logic. But as apps grow, putting all state, business logic, validation, API calls, and UI into one file becomes unmaintainable. You need a pattern.

## Core Principle: Logic in `.st`, UI in `.stui`

Stratos supports both `.st` and `.stui` files in the same project. The recommended split:

| File type | Contains | Analogy |
|-----------|----------|---------|
| `.st` | State (Signals), computed values, business logic, API calls, data models | React hooks, SwiftUI ViewModels, Flutter BLoC |
| `.stui` | Declarative UI components, layout, event handler wiring | JSX/TSX, SwiftUI View, Flutter Widget tree |

This is the same separation used by SolidJS (signals in `.ts`, components in `.tsx`), SwiftUI (ViewModel in `.swift`, View in `.swift`), and Compose (ViewModel + Composable functions).

## How Imports Work

STUI files can import `.st` modules using `import`. The runtime resolves modules by searching `src/<name>/` for `.st` files.

```stui
import "models";      // loads src/models/init.st (or all .st files in src/models/)
import "stores/auth";  // loads src/stores/auth/init.st
import "std/time";     // loads standard library module
```

Functions, classes, and variables defined in imported `.st` files are available in the STUI file using `module.name` syntax:

```stui
import "counter_logic";

component Counter {
    view {
        Text("Count: ${counter_logic.getCount()}")
        Button("+") {
            onClick: { counter_logic.increment(); }
        }
    }
}
```

## Patterns by Complexity

### Pattern 1: Single-File Component

**When**: Tiny widgets, demos, prototypes.

```
my-app/
├── stratos.conf
└── src/
    └── main.stui
```

```stui
// src/main.stui
import "std/gui";

component Counter {
    state count: int = 0;

    view {
        Column(spacing: 10) {
            Text("Count: ${count}")
            Button("+1") {
                onClick: { count += 1; }
            }
        }
    }
}

component App {
    view {
        Window(title: "Counter", width: 400, height: 300) {
            Center {
                Counter()
            }
        }
    }
}
```

**Verdict**: Fine for < 50 lines. No separation needed.

---

### Pattern 2: Logic Module

**When**: A component has non-trivial state or business logic (validation, computed values, side effects). This is the **recommended default** for most apps.

```
my-app/
├── stratos.conf
└── src/
    ├── counter/
    │   └── init.st        # state + logic
    └── main.stui           # UI
```

**`src/counter/init.st`** — All state and logic:

```stratos
package counter;

use gui;

// --- State ---
val count = Signal(0);
val doubled = Computed(fn() any => count.get() as int * 2);
val isPositive = Computed(fn() any => count.get() as int > 0);

// --- Actions ---
fn increment() void {
    count.set(count.get() as int + 1);
}

fn decrement() void {
    val c = count.get() as int;
    if c > 0 {
        count.set(c - 1);
    }
}

fn reset() void {
    count.set(0);
}

// --- Side effects ---
createEffect(fn() void {
    println("[counter] value: ${count.get()}, doubled: ${doubled.get()}");
});
```

**`src/main.stui`** — Pure UI, no logic:

```stui
import "std/gui";
import "counter";

component CounterView {
    view {
        Column(spacing: 12) {
            Text("Count: ${counter.count.get()}") {
                fontSize: 28;
                fontWeight: "bold";
            }
            Text("Doubled: ${counter.doubled.get()}") {
                fontSize: 16;
                color: rgb(100, 100, 100);
            }
            Row(spacing: 8) {
                Button("-") {
                    onClick: { counter.decrement(); }
                }
                Button("Reset") {
                    onClick: { counter.reset(); }
                }
                Button("+") {
                    onClick: { counter.increment(); }
                }
            }
        }
    }
}

component App {
    view {
        Window(title: "Counter", width: 400, height: 300) {
            Center {
                CounterView()
            }
        }
    }
}
```

**Why this works**:
- `counter/init.st` is a pure logic module — no widgets, no UI. It can be tested independently.
- `main.stui` reads state via `counter.count.get()` and calls actions via `counter.increment()`. It contains zero business logic.
- Adding a new feature (e.g., undo) means editing only `counter/init.st`.

---

### Pattern 3: Feature Modules

**When**: Multiple features with independent state (todo list, auth, settings). Each feature gets its own logic module and component.

```
my-app/
├── stratos.conf
└── src/
    ├── todos/
    │   └── init.st          # todo state + CRUD logic
    ├── auth/
    │   └── init.st          # auth state + login/logout
    ├── views/
    │   ├── TodoList.stui     # todo UI
    │   ├── LoginForm.stui    # auth UI
    │   └── Header.stui       # shared header
    └── main.stui             # app shell + routing
```

**`src/todos/init.st`** — Todo feature logic:

```stratos
package todos;

use gui;

// --- State ---
val items = Signal([]);
val filter = Signal("all");

val visibleItems = Computed(fn() any {
    val all = items.get();
    val f = filter.get() as string;
    when f {
        "active" -> return all.filter(fn(t: any) bool => !t.done);
        "done" -> return all.filter(fn(t: any) bool => t.done);
        else -> return all;
    }
});

val activeCount = Computed(fn() any {
    return items.get().filter(fn(t: any) bool => !t.done).length();
});

// --- Actions ---
fn addTodo(text: string) void {
    val current = items.get() as array;
    current.push({ text: text, done: false });
    items.set(current);
}

fn toggleTodo(index: int) void {
    val current = items.get() as array;
    current[index].done = !current[index].done;
    items.set(current);
}

fn removeTodo(index: int) void {
    val current = items.get() as array;
    current.remove(index);
    items.set(current);
}

fn setFilter(f: string) void {
    filter.set(f);
}
```

**`src/views/TodoList.stui`** — Todo UI component:

```stui
import "std/gui";
import "todos";

component TodoItem {
    props {
        text: string;
        done: bool;
        index: int;
    }

    view {
        Row(spacing: 8) {
            Checkbox(text) {
                checked: done;
                onChange: { todos.toggleTodo(index); }
            }
            Button("x") {
                onClick: { todos.removeTodo(index); }
            }
        }
    }
}

component TodoList {
    state inputText: string = "";

    view {
        Column(spacing: 12) {
            Row(spacing: 8) {
                TextField("What needs to be done?") {
                    value: inputText;
                    onChange: { inputText = value; }
                }
                Button("Add") {
                    onClick: {
                        todos.addTodo(inputText);
                        inputText = "";
                    }
                }
            }

            Row(spacing: 4) {
                Chip("All") { onClick: { todos.setFilter("all"); } }
                Chip("Active") { onClick: { todos.setFilter("active"); } }
                Chip("Done") { onClick: { todos.setFilter("done"); } }
            }

            Text("${todos.activeCount.get()} items left") {
                fontSize: 12;
                color: rgb(120, 120, 120);
            }

            for item in todos.visibleItems.get() {
                TodoItem(text: item.text, done: item.done, index: index)
            }
        }
    }
}
```

**`src/auth/init.st`** — Auth feature logic:

```stratos
package auth;

use gui;

val currentUser = Signal(nil);
val isLoggedIn = Computed(fn() any => currentUser.get() != nil);

fn login(username: string, password: string) bool {
    // validate, call API, etc.
    if username != "" {
        currentUser.set({ name: username });
        return true;
    }
    return false;
}

fn logout() void {
    currentUser.set(nil);
}

fn getUsername() string {
    val user = currentUser.get();
    if user == nil { return "Guest"; }
    return user.name as string;
}
```

**`src/main.stui`** — App shell:

```stui
import "std/gui";
import "auth";
import "views/TodoList";
import "views/LoginForm";
import "views/Header";

component App {
    view {
        Window(title: "Todo App", width: 600, height: 500) {
            Scaffold {
                appBar: AppBar("Todos - ${auth.getUsername()}")
                body: Column(spacing: 0) {
                    if auth.isLoggedIn.get() {
                        TodoList()
                    } else {
                        LoginForm()
                    }
                }
            }
        }
    }
}
```

**Why this works**:
- Each feature (`todos`, `auth`) is self-contained with its own state and actions.
- UI components in `views/` only import the logic they need.
- Features don't know about each other's UI — they only share state through signals.
- You can add a new feature by creating a new `src/<feature>/init.st` + `src/views/<Feature>.stui` without touching existing code.

---

### Pattern 4: ViewModel Classes

**When**: You need multiple instances of the same logic (e.g., multiple forms, multiple counters), or you want constructor-based dependency injection.

```
my-app/
└── src/
    ├── viewmodels/
    │   └── init.st            # ViewModel classes
    └── main.stui
```

**`src/viewmodels/init.st`**:

```stratos
package viewmodels;

use gui;

class CounterViewModel {
    var count: any;
    var doubled: any;
    var label: string;

    constructor(label: string, initial: int) {
        this.label = label;
        this.count = Signal(initial);
        this.doubled = Computed(fn() any => this.count.get() as int * 2);
    }

    fn increment() void {
        this.count.set(this.count.get() as int + 1);
    }

    fn decrement() void {
        this.count.set(this.count.get() as int - 1);
    }

    fn value() int {
        return this.count.get() as int;
    }
}

class FormViewModel {
    var email: any;
    var password: any;
    var emailError: any;
    var isValid: any;

    constructor() {
        this.email = Signal("");
        this.password = Signal("");
        this.emailError = Computed(fn() any {
            val e = this.email.get() as string;
            if e == "" { return "Required"; }
            if !e.contains("@") { return "Invalid email"; }
            return "";
        });
        this.isValid = Computed(fn() any {
            return this.emailError.get() as string == ""
                && (this.password.get() as string).length() >= 6;
        });
    }

    fn setEmail(value: string) void { this.email.set(value); }
    fn setPassword(value: string) void { this.password.set(value); }
}
```

**`src/main.stui`**:

```stui
import "std/gui";
import "viewmodels";

component Counter {
    props {
        vm: any;
    }

    view {
        Card {
            Column(spacing: 8) {
                Text("${vm.label}: ${vm.value()}")
                Row(spacing: 8) {
                    Button("-") { onClick: { vm.decrement(); } }
                    Button("+") { onClick: { vm.increment(); } }
                }
            }
        }
    }
}

component App {
    view {
        Window(title: "Multi Counter", width: 600, height: 400) {
            Column(spacing: 16) {
                Counter(vm: viewmodels.CounterViewModel("Apples", 0))
                Counter(vm: viewmodels.CounterViewModel("Oranges", 10))
            }
        }
    }
}
```

**Why this works**:
- Each ViewModel instance has its own isolated state.
- The same `Counter` component works with different ViewModel instances.
- ViewModels are testable: create an instance, call methods, assert state.

---

### Pattern 5: Store + Context (App-Wide State)

**When**: State that many unrelated components need (theme, auth, locale, app config).

```
my-app/
└── src/
    ├── app_state/
    │   └── init.st            # global store + contexts
    ├── views/
    │   ├── Sidebar.stui
    │   └── Content.stui
    └── main.stui
```

**`src/app_state/init.st`** — Global state:

```stratos
package app_state;

use gui;

// --- Theme context (DI) ---
val themeCtx = Context("theme", "light");

fn setTheme(theme: string) void {
    themeCtx.provide(theme);
}

fn getTheme() string {
    return themeCtx.consume() as string;
}

fn isDark() bool {
    return themeCtx.consume() as string == "dark";
}

// --- Global app store (key-value) ---
val appStore = Store();

fn initStore() void {
    appStore.set("user", nil);
    appStore.set("notifications", 0);
    appStore.set("sidebarOpen", true);
}

fn toggleSidebar() void {
    val current = appStore.get("sidebarOpen") as bool;
    appStore.set("sidebarOpen", !current);
}

fn setUser(user: any) void {
    appStore.set("user", user);
}

fn addNotification() void {
    val n = appStore.get("notifications") as int;
    appStore.set("notifications", n + 1);
}
```

**`src/main.stui`**:

```stui
import "std/gui";
import "app_state";
import "views/Sidebar";
import "views/Content";

component App {
    view {
        Window(title: "Dashboard", width: 1024, height: 768) {
            Row(spacing: 0) {
                if app_state.appStore.get("sidebarOpen") {
                    Sidebar()
                }
                Content()
            }
        }
    }
}
```

---

### Pattern 6: Full App (Router + Stores + Features)

**When**: Multi-page SPA with routing, auth, and multiple features.

```
my-app/
├── stratos.conf
└── src/
    ├── app_state/
    │   └── init.st            # global store, theme, auth
    ├── features/
    │   ├── dashboard/
    │   │   └── init.st        # dashboard logic
    │   ├── users/
    │   │   └── init.st        # user CRUD logic
    │   └── settings/
    │       └── init.st        # settings logic
    ├── views/
    │   ├── DashboardPage.stui
    │   ├── UserListPage.stui
    │   ├── UserDetailPage.stui
    │   ├── SettingsPage.stui
    │   └── Layout.stui        # shared layout shell
    └── main.stui              # entry: routing
```

**`src/main.stui`**:

```stui
import "std/gui";
import "app_state";
import "views/DashboardPage";
import "views/UserListPage";
import "views/UserDetailPage";
import "views/SettingsPage";
import "views/Layout";

component App {
    view {
        Window(title: "Admin Panel", width: 1200, height: 800) {
            Router {
                Route(path: "/") {
                    Layout {
                        DashboardPage()
                    }
                }
                Route(path: "/users") {
                    Layout {
                        UserListPage()
                    }
                }
                Route(path: "/users/:id") {
                    Layout {
                        UserDetailPage()
                    }
                }
                Route(path: "/settings") {
                    Layout {
                        SettingsPage()
                    }
                }
                NotFound {
                    Text("404 - Page not found")
                }
            }
        }
    }
}
```

---

## Rules of Thumb

| Rule | Rationale |
|------|-----------|
| **No business logic in `.stui` files** | Event handlers should be one-liners that call a function from a `.st` module. If you're writing `if/else` or loops in an `onClick`, extract it. |
| **No widget creation in `.st` logic files** | Logic modules define state and functions — never `Text()`, `Button()`, or layout. The exception is returning widget IDs from builder functions for Router routes. |
| **One feature = one logic module** | `src/todos/init.st` owns all todo state and actions. Don't split one feature's signals across multiple modules. |
| **Signals are the interface** | `.stui` reads state via `module.signal.get()` and calls actions via `module.action()`. This is the only contract between logic and UI. |
| **Keep `state` in STUI for local-only UI state** | Ephemeral state like "is this dropdown open" or "current input text" can stay as `state` in the component. Shared or persistent state belongs in `.st`. |
| **Use Computed, not manual tracking** | Derived values should be `Computed()`, not manually recalculated in event handlers. |
| **Use Effects for side effects, not event handlers** | Logging, persistence, analytics — use `createEffect()` in your `.st` module. Don't sprinkle `println()` calls in STUI event handlers. |

## Choosing a Pattern

```
Is your app a single screen with < 50 lines of state?
  └─ Yes → Pattern 1 (Single File)

Does it have non-trivial logic (validation, computed values, API calls)?
  └─ Yes → Pattern 2 (Logic Module)

Does it have multiple independent features?
  └─ Yes → Pattern 3 (Feature Modules)

Do you need multiple instances of the same stateful component?
  └─ Yes → Pattern 4 (ViewModel Classes)

Do you have app-wide state (theme, auth, config)?
  └─ Yes → Pattern 5 (Store + Context)

Is it a multi-page app with routing?
  └─ Yes → Pattern 6 (Full App)
```

These patterns compose — a full app typically uses Pattern 3 (feature modules) + Pattern 5 (global store) + Pattern 6 (routing) together.

## `stratos.conf` for Mixed Projects

The entry point can be either `.st` or `.stui`:

```hocon
project {
    name = my_app
    version = "1.0.0"
}

build {
    entry = src/main.stui
    output = build/my_app
}
```

The `use`/`import` system automatically finds `.st` files in `src/<module>/init.st` or loads all `.st` files in `src/<module>/`. No additional configuration is needed to mix file types.
