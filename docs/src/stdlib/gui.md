---
title: gui
---

# Package: gui

Native GUI framework for Stratos — widgets, layout, reactivity, routing, and state management rendered with SDL2.

For architecture patterns on structuring apps (separating logic from UI, mixing `.st` and `.stui` files), see the [GUI Architecture Patterns](../guides/gui-architecture.md) guide.

## Usage

```stratos
use gui;

val app = App("My App", 800, 600);
app.root(Center([Text("Hello, GUI!")]));
app.run();
```

## Classes

### `App`

Application window and event loop.

```stratos
val app = App("My App", 800, 600);
app.root(Text("Hello!"));
app.setTheme("dark");
app.setFPS(60);
app.run();
app.quit();
```

**Methods:**

| Method | Description |
|--------|-------------|
| `root(widget: int) void` | Set the root widget |
| `run() void` | Start the event loop (blocks until window closes) |
| `quit() void` | Stop the event loop |
| `setTheme(theme: string) void` | Set color theme: `"light"` or `"dark"` |
| `setFPS(fps: int) void` | Set target frames per second |

**Source:** [std/gui/init.st:305](std/gui/init.st#L305)

---

### `Signal`

Reactive signal with auto-tracking. When read inside a `createEffect()` callback, the effect automatically re-runs when the signal changes.

```stratos
val count = Signal(0);
createEffect(fn() void {
    println("Count: ${count.get()}");
});
count.set(1);  // effect re-runs automatically
```

**Methods:**

| Method | Description |
|--------|-------------|
| `get() any` | Get the value. Subscribes current effect if inside one |
| `set(value: any) void` | Set the value. Notifies all subscribed effects |
| `peek() any` | Read without tracking (won't subscribe current effect) |

**Source:** [std/gui/init.st:400](std/gui/init.st#L400)

---

### `Computed`

Memoized signal that derives from other signals. Automatically recomputes when dependencies change.

```stratos
val count = Signal(5);
val doubled = Computed(fn() any => count.get() as int * 2);
println(doubled.get());  // 10
count.set(10);
println(doubled.get());  // 20
```

**Methods:**

| Method | Description |
|--------|-------------|
| `get() any` | Get the cached value. Recomputes if dependencies changed |

**Source:** [std/gui/init.st:432](std/gui/init.st#L432)

---

### `State`

Reactive state container (wraps Signal internally for backward compatibility).

```stratos
val count = State(0);
println(count.get());
count.set(count.get() + 1);
```

**Methods:**

| Method | Description |
|--------|-------------|
| `get() any` | Get the current value |
| `set(value: any) void` | Set the value and trigger re-render |

**Source:** [std/gui/init.st:356](std/gui/init.st#L356)

---

### `DerivedState`

Computed state that derives from other state (wraps Computed internally).

**Methods:**

| Method | Description |
|--------|-------------|
| `get() any` | Get the computed value |

**Source:** [std/gui/init.st:374](std/gui/init.st#L374)

---

### `Router`

Client-side router for single-page app navigation. Supports path params, query params, wildcards, guards, and history.

```stratos
val router = Router();
router.route("/", fn() int => HomePage());
router.route("/user/:id", fn() int => UserPage(router.param("id")));
router.notFound(fn() int => NotFoundPage());
router.navigate("/");
```

**Methods:**

| Method | Description |
|--------|-------------|
| `route(path: string, builder: any) void` | Register a route. Patterns: `"/"`, `"/user/:id"`, `"/files/*"` |
| `guard(path: string, guardFn: any) void` | Add a guard function (returns bool) |
| `notFound(builder: any) void` | Set the 404 handler |
| `navigate(path: string) void` | Navigate to a path |
| `back() void` | Go back in history |
| `forward() void` | Go forward in history |
| `replace(path: string) void` | Replace current route (no history entry) |
| `param(name: string) string` | Get a route parameter (e.g., `:id`) |
| `query(name: string) string` | Get a query parameter (e.g., `?q=hello`) |
| `currentPath() string` | Get the current route path |
| `widget() int` | Get the widget ID of the current route |

**Source:** [std/gui/init.st:1050](std/gui/init.st#L1050)

---

### `Store`

Reactive key-value container with fine-grained per-key subscriptions.

```stratos
val store = Store();
store.set("user", "Alice");
store.set("count", 0);
val name = store.get("user");
val subId = store.subscribe("count", fn(value: any) void {
    println("Count changed: ${value}");
});
```

**Methods:**

| Method | Description |
|--------|-------------|
| `get(key: string) any` | Read a field |
| `set(key: string, value: any) void` | Write a field, notifying subscribers |
| `subscribe(key: string, callback: any) int` | Subscribe to a key. Returns subscription ID |
| `unsubscribe(subscriptionId: int) void` | Remove a subscription |

**Source:** [std/gui/init.st:1127](std/gui/init.st#L1127)

---

### `Context`

Named context for dependency injection (provider/consumer pattern).

```stratos
val ThemeCtx = Context("theme", "light");
ThemeCtx.provide("dark");
val theme = ThemeCtx.consume();
```

**Methods:**

| Method | Description |
|--------|-------------|
| `provide(value: any) void` | Provide a value into this context |
| `consume() any` | Consume the current value |

**Source:** [std/gui/init.st:1159](std/gui/init.st#L1159)

---

## Reactivity Functions

### `createEffect(closure: any) int`

Create a reactive side-effect that auto-tracks signal dependencies. Returns an effect ID.

```stratos
val name = Signal("World");
val effectId = createEffect(fn() void {
    println("Hello, ${name.get()}!");
});
name.set("Stratos");  // prints "Hello, Stratos!"
```

**Source:** [std/gui/init.st:454](std/gui/init.st#L454)

---

### `disposeEffect(id: int) void`

Dispose (stop) a previously created effect.

**Source:** [std/gui/init.st:459](std/gui/init.st#L459)

---

### `batch(closure: any) void`

Batch multiple signal updates — effects only run once at the end.

```stratos
batch(fn() void {
    x.set(1);
    y.set(2);
    z.set(3);
    // effects run once here, not three times
});
```

**Source:** [std/gui/init.st:472](std/gui/init.st#L472)

---

### `untrack(closure: any) any`

Read signals without tracking — the current effect won't subscribe.

```stratos
createEffect(fn() void {
    val tracked = count.get();     // subscribes
    val ignored = untrack(fn() any => other.get());  // doesn't subscribe
});
```

**Source:** [std/gui/init.st:483](std/gui/init.st#L483)

---

### `onCleanup(closure: any) void`

Register a cleanup function for the current effect. Runs before re-execution or disposal.

```stratos
createEffect(fn() void {
    val timer = startTimer();
    onCleanup(fn() void { stopTimer(timer); });
});
```

**Source:** [std/gui/init.st:495](std/gui/init.st#L495)

---

## Core Widgets

### `Text(text: string) int`

Create a text label.

**Source:** [std/gui/init.st:515](std/gui/init.st#L515)

---

### `StyledText(text: string, fontSize: float, r: int, g: int, b: int) int`

Create a styled text label with font size and color.

**Source:** [std/gui/init.st:520](std/gui/init.st#L520)

---

### `BoldText(text: string, fontSize: float) int`

Create bold text.

**Source:** [std/gui/init.st:528](std/gui/init.st#L528)

---

### `ItalicText(text: string, fontSize: float) int`

Create italic text.

**Source:** [std/gui/init.st:924](std/gui/init.st#L924)

---

### `MonoText(text: string, fontSize: float) int`

Create monospace text (code-style).

**Source:** [std/gui/init.st:932](std/gui/init.st#L932)

---

### `StyledTextFull(text: string, fontSize: float, r: int, g: int, b: int, weight: string, family: string) int`

Create text with full typography control.

**Source:** [std/gui/init.st:914](std/gui/init.st#L914)

---

### `Button(label: string) int`

Create a clickable button.

**Source:** [std/gui/init.st:540](std/gui/init.st#L540)

---

### `ColoredButton(label: string, r: int, g: int, b: int) int`

Create a colored button.

**Source:** [std/gui/init.st:545](std/gui/init.st#L545)

---

### `Image(path: string) int`

Display an image from a file path.

**Source:** [std/gui/init.st:574](std/gui/init.st#L574)

---

### `SizedImage(path: string, width: float, height: float) int`

Display an image with a fixed size.

**Source:** [std/gui/init.st:579](std/gui/init.st#L579)

---

### `Icon(icon: string, size: float) int`

Create an icon widget (Unicode character or symbol).

**Source:** [std/gui/init.st:666](std/gui/init.st#L666)

---

### `Box(children: array<int>) int`

Create a generic container box.

**Source:** [std/gui/init.st:556](std/gui/init.st#L556)

---

### `Container(r: int, g: int, b: int, borderRadius: float, children: array<int>) int`

Create a styled container with background color and rounded corners.

**Source:** [std/gui/init.st:562](std/gui/init.st#L562)

---

## Input Widgets

### `TextField(placeholder: string) int`

Create a single-line text input field.

**Source:** [std/gui/init.st:590](std/gui/init.st#L590)

---

### `getTextFieldValue(id: int) string`

Get the current value of a text field.

**Source:** [std/gui/init.st:595](std/gui/init.st#L595)

---

### `setTextFieldValue(id: int, value: string) void`

Set the value of a text field.

**Source:** [std/gui/init.st:600](std/gui/init.st#L600)

---

### `TextArea(placeholder: string) int`

Create a multi-line text input area.

**Source:** [std/gui/init.st:605](std/gui/init.st#L605)

---

### `getTextAreaValue(id: int) string`

Get the current value of a text area.

**Source:** [std/gui/init.st:610](std/gui/init.st#L610)

---

### `setTextAreaValue(id: int, value: string) void`

Set the value of a text area.

**Source:** [std/gui/init.st:615](std/gui/init.st#L615)

---

### `Checkbox(label: string) int`

Create a checkbox with a label.

**Source:** [std/gui/init.st:620](std/gui/init.st#L620)

---

### `isChecked(id: int) bool`

Check if a checkbox is checked.

**Source:** [std/gui/init.st:625](std/gui/init.st#L625)

---

### `RadioButton(label: string, group: string) int`

Create a radio button. Only one per group can be selected.

**Source:** [std/gui/init.st:631](std/gui/init.st#L631)

---

### `isRadioSelected(id: int) bool`

Check if a radio button is selected.

**Source:** [std/gui/init.st:636](std/gui/init.st#L636)

---

### `Dropdown(placeholder: string) int`

Create a dropdown/select widget.

**Source:** [std/gui/init.st:641](std/gui/init.st#L641)

---

### `addDropdownItem(id: int, item: string) void`

Add an item to a dropdown.

**Source:** [std/gui/init.st:646](std/gui/init.st#L646)

---

### `getDropdownSelected(id: int) string`

Get the selected item text from a dropdown.

**Source:** [std/gui/init.st:651](std/gui/init.st#L651)

---

### `getDropdownSelectedIndex(id: int) int`

Get the selected index (-1 if none).

**Source:** [std/gui/init.st:656](std/gui/init.st#L656)

---

### `setDropdownSelected(id: int, index: int) void`

Set the selected index of a dropdown.

**Source:** [std/gui/init.st:661](std/gui/init.st#L661)

---

### `Slider(min: float, max: float) int`

Create a range slider.

**Source:** [std/gui/init.st:671](std/gui/init.st#L671)

---

### `getSliderValue(id: int) float`

Get the current slider value.

**Source:** [std/gui/init.st:676](std/gui/init.st#L676)

---

### `Switch() int`

Create a toggle switch.

**Source:** [std/gui/init.st:681](std/gui/init.st#L681)

---

### `isSwitchOn(id: int) bool`

Check if a switch is on.

**Source:** [std/gui/init.st:686](std/gui/init.st#L686)

---

## Layout Widgets

### `Row(spacing: float, children: array<int>) int`

Arrange children in a horizontal row.

**Source:** [std/gui/init.st:695](std/gui/init.st#L695)

---

### `AlignedRow(spacing: float, mainAlign: string, crossAlign: string, children: array<int>) int`

Create a row with alignment options. Alignment values: `"start"`, `"center"`, `"end"`, `"spaceBetween"`, `"spaceAround"`.

**Source:** [std/gui/init.st:701](std/gui/init.st#L701)

---

### `Column(spacing: float, children: array<int>) int`

Arrange children in a vertical column.

**Source:** [std/gui/init.st:708](std/gui/init.st#L708)

---

### `AlignedColumn(spacing: float, mainAlign: string, crossAlign: string, children: array<int>) int`

Create a column with alignment options.

**Source:** [std/gui/init.st:714](std/gui/init.st#L714)

---

### `Stack(children: array<int>) int`

Stack children on top of each other (z-axis).

**Source:** [std/gui/init.st:721](std/gui/init.st#L721)

---

### `Center(children: array<int>) int`

Center a child widget.

**Source:** [std/gui/init.st:727](std/gui/init.st#L727)

---

### `Padding(all: float, children: array<int>) int`

Wrap children with padding on all sides.

**Source:** [std/gui/init.st:733](std/gui/init.st#L733)

---

### `Spacer() int`

Create flexible space between widgets.

**Source:** [std/gui/init.st:739](std/gui/init.st#L739)

---

### `FixedSpacer(size: float) int`

Create a fixed-size spacer.

**Source:** [std/gui/init.st:744](std/gui/init.st#L744)

---

### `ScrollView(children: array<int>) int`

Create a scrollable container.

**Source:** [std/gui/init.st:749](std/gui/init.st#L749)

---

### `Grid(columns: int, spacing: float, children: array<int>) int`

Create a grid layout.

**Source:** [std/gui/init.st:755](std/gui/init.st#L755)

---

### `Wrap(spacing: float, children: array<int>) int`

Create a flow layout that wraps to the next line.

**Source:** [std/gui/init.st:1325](std/gui/init.st#L1325)

---

### `WrapCustom(spacing: float, runSpacing: float, children: array<int>) int`

Create a wrap layout with custom run spacing.

**Source:** [std/gui/init.st:1331](std/gui/init.st#L1331)

---

## Navigation Widgets

### `AppBar(title: string) int`

Create a top app bar with a title.

**Source:** [std/gui/init.st:765](std/gui/init.st#L765)

---

### `Dialog(title: string, children: array<int>) int`

Create a modal dialog.

**Source:** [std/gui/init.st:770](std/gui/init.st#L770)

---

### `openDialog(id: int, open: bool) void`

Open or close a dialog.

**Source:** [std/gui/init.st:776](std/gui/init.st#L776)

---

### `Drawer(width: float, children: array<int>) int`

Create a side drawer panel.

**Source:** [std/gui/init.st:781](std/gui/init.st#L781)

---

### `openDrawer(id: int, open: bool) void`

Open or close a drawer.

**Source:** [std/gui/init.st:787](std/gui/init.st#L787)

---

### `isDrawerOpen(id: int) bool`

Check if a drawer is open.

**Source:** [std/gui/init.st:792](std/gui/init.st#L792)

---

### `TabBar(tabs: array<string>) int`

Create a tab bar with labeled tabs.

**Source:** [std/gui/init.st:797](std/gui/init.st#L797)

---

### `getActiveTab(id: int) int`

Get the active tab index.

**Source:** [std/gui/init.st:806](std/gui/init.st#L806)

---

### `setActiveTab(id: int, index: int) void`

Set the active tab index.

**Source:** [std/gui/init.st:811](std/gui/init.st#L811)

---

### `Menu() int`

Create a context menu.

**Source:** [std/gui/init.st:816](std/gui/init.st#L816)

---

### `addMenuItem(id: int, label: string) void`

Add a menu item.

**Source:** [std/gui/init.st:821](std/gui/init.st#L821)

---

### `addMenuSeparator(id: int) void`

Add a separator line to a menu.

**Source:** [std/gui/init.st:826](std/gui/init.st#L826)

---

### `showMenu(id: int, x: float, y: float) void`

Show a menu at a position.

**Source:** [std/gui/init.st:831](std/gui/init.st#L831)

---

### `hideMenu(id: int) void`

Hide a menu.

**Source:** [std/gui/init.st:836](std/gui/init.st#L836)

---

### `Link(text: string, path: string, router: any) int`

Create a navigation link button.

**Source:** [std/gui/init.st:1115](std/gui/init.st#L1115)

---

## Composite Widgets

### `Card(children: array<int>) int`

Create an elevated container with rounded corners and shadow (elevation 2.0).

**Source:** [std/gui/init.st:1211](std/gui/init.st#L1211)

---

### `ElevatedCard(elevation: float, children: array<int>) int`

Create a card with custom elevation.

**Source:** [std/gui/init.st:1217](std/gui/init.st#L1217)

---

### `Divider() int`

Create a horizontal divider line.

**Source:** [std/gui/init.st:1223](std/gui/init.st#L1223)

---

### `VerticalDivider() int`

Create a vertical divider line.

**Source:** [std/gui/init.st:1228](std/gui/init.st#L1228)

---

### `ThickDivider(thickness: float) int`

Create a divider with custom thickness.

**Source:** [std/gui/init.st:1233](std/gui/init.st#L1233)

---

### `Badge(label: string) int`

Create a badge (e.g., notification count).

**Source:** [std/gui/init.st:1238](std/gui/init.st#L1238)

---

### `setBadgeLabel(id: int, label: string) void`

Update the badge label.

**Source:** [std/gui/init.st:1243](std/gui/init.st#L1243)

---

### `Tooltip(message: string, child: int) int`

Create a tooltip that shows a message on hover.

**Source:** [std/gui/init.st:1248](std/gui/init.st#L1248)

---

### `Chip(label: string) int`

Create a chip (small tag/filter element).

**Source:** [std/gui/init.st:1255](std/gui/init.st#L1255)

---

### `setChipSelected(id: int, selected: bool) void`

Set chip selected state.

**Source:** [std/gui/init.st:1260](std/gui/init.st#L1260)

---

### `isChipSelected(id: int) bool`

Check if a chip is selected.

**Source:** [std/gui/init.st:1265](std/gui/init.st#L1265)

---

### `FAB(icon: string) int`

Create a floating action button.

**Source:** [std/gui/init.st:1270](std/gui/init.st#L1270)

---

### `ProgressBar(value: float) int`

Create a linear progress bar (0.0 to 1.0).

**Source:** [std/gui/init.st:1275](std/gui/init.st#L1275)

---

### `setProgress(id: int, value: float) void`

Update the progress bar value.

**Source:** [std/gui/init.st:1280](std/gui/init.st#L1280)

---

### `setProgressIndeterminate(id: int, indeterminate: bool) void`

Set a progress bar to indeterminate mode (animated).

**Source:** [std/gui/init.st:1285](std/gui/init.st#L1285)

---

### `CircularProgress() int`

Create an indeterminate circular progress indicator.

**Source:** [std/gui/init.st:1290](std/gui/init.st#L1290)

---

### `CircularProgressValue(value: float) int`

Create a circular progress with a specific value (0.0 to 1.0).

**Source:** [std/gui/init.st:1295](std/gui/init.st#L1295)

---

### `setCircularProgress(id: int, value: float) void`

Update circular progress value.

**Source:** [std/gui/init.st:1300](std/gui/init.st#L1300)

---

### `SnackBar(message: string) int`

Create a snackbar (temporary message at bottom).

**Source:** [std/gui/init.st:1305](std/gui/init.st#L1305)

---

### `showSnackBar(id: int) void`

Show a snackbar.

**Source:** [std/gui/init.st:1310](std/gui/init.st#L1310)

---

### `hideSnackBar(id: int) void`

Hide a snackbar.

**Source:** [std/gui/init.st:1315](std/gui/init.st#L1315)

---

### `setSnackBarMessage(id: int, message: string) void`

Update the snackbar message.

**Source:** [std/gui/init.st:1320](std/gui/init.st#L1320)

---

### `ExpansionPanel(title: string, children: array<int>) int`

Create a collapsible/expandable section.

**Source:** [std/gui/init.st:1337](std/gui/init.st#L1337)

---

### `setExpanded(id: int, expanded: bool) void`

Set expansion panel expanded state.

**Source:** [std/gui/init.st:1343](std/gui/init.st#L1343)

---

### `isExpanded(id: int) bool`

Check if an expansion panel is expanded.

**Source:** [std/gui/init.st:1348](std/gui/init.st#L1348)

---

### `Scaffold(appBarId: int, bodyId: int) int`

Create a scaffold layout (appbar + body).

**Source:** [std/gui/init.st:1353](std/gui/init.st#L1353)

---

### `ScaffoldWithFAB(appBarId: int, bodyId: int, fabId: int) int`

Create a scaffold with a floating action button.

**Source:** [std/gui/init.st:1361](std/gui/init.st#L1361)

---

### `BottomNavBar() int`

Create a bottom navigation bar.

**Source:** [std/gui/init.st:1370](std/gui/init.st#L1370)

---

### `addNavItem(id: int, icon: string, label: string) void`

Add an item to a bottom nav bar.

**Source:** [std/gui/init.st:1375](std/gui/init.st#L1375)

---

### `getActiveNavItem(id: int) int`

Get the active nav item index.

**Source:** [std/gui/init.st:1380](std/gui/init.st#L1380)

---

### `setActiveNavItem(id: int, index: int) void`

Set the active nav item index.

**Source:** [std/gui/init.st:1385](std/gui/init.st#L1385)

---

## Event Handlers

### `onClick(id: int, callback: any) void`

Set a click handler on a widget.

**Source:** [std/gui/init.st:850](std/gui/init.st#L850)

---

### `onChange(id: int, callback: any) void`

Set a change handler on a text input or dropdown.

**Source:** [std/gui/init.st:855](std/gui/init.st#L855)

---

### `onSubmit(id: int, callback: any) void`

Set a submit handler on a text field.

**Source:** [std/gui/init.st:860](std/gui/init.st#L860)

---

### `onMouseEnter(id: int, callback: any) void`

Set a mouse enter handler.

**Source:** [std/gui/init.st:865](std/gui/init.st#L865)

---

### `onMouseLeave(id: int, callback: any) void`

Set a mouse leave handler.

**Source:** [std/gui/init.st:870](std/gui/init.st#L870)

---

### `onKeyPress(id: int, callback: any) void`

Set a key press handler.

**Source:** [std/gui/init.st:875](std/gui/init.st#L875)

---

### `setText(id: int, text: string) void`

Update the text content of a Text widget.

**Source:** [std/gui/init.st:845](std/gui/init.st#L845)

---

## Typography Functions

### `setFontSize(id: int, size: float) void`

Set font size on any widget (Text, Button, etc.).

**Source:** [std/gui/init.st:884](std/gui/init.st#L884)

---

### `setTextColor(id: int, r: int, g: int, b: int) void`

Set text color on any widget.

**Source:** [std/gui/init.st:889](std/gui/init.st#L889)

---

### `setFontWeight(id: int, weight: string) void`

Set font weight. Values: `"thin"`, `"light"`, `"regular"`, `"medium"`, `"semibold"`, `"bold"`, `"extrabold"`, `"black"`.

**Source:** [std/gui/init.st:894](std/gui/init.st#L894)

---

### `setFontFamily(id: int, family: string) void`

Set font family (e.g., `"sans-serif"`, `"monospace"`, `"serif"`).

**Source:** [std/gui/init.st:899](std/gui/init.st#L899)

---

### `setItalic(id: int, italic: bool) void`

Set italic style.

**Source:** [std/gui/init.st:904](std/gui/init.st#L904)

---

### `setTextAlign(id: int, align: string) void`

Set text alignment: `"left"`, `"center"`, `"right"`.

**Source:** [std/gui/init.st:909](std/gui/init.st#L909)

---

## Font Management

### `registerFontFile(family: string, path: string) void`

Register a font from a local `.ttf` or `.otf` file.

```stratos
registerFontFile("MyFont", "/path/to/MyFont-Regular.ttf");
```

**Source:** [std/gui/init.st:949](std/gui/init.st#L949)

---

### `registerFontVariant(family: string, path: string, variant: string) void`

Register a font variant. Variants: `"regular"`, `"bold"`, `"italic"`, `"bolditalic"`, `"variable"`.

**Source:** [std/gui/init.st:954](std/gui/init.st#L954)

---

### `registerFontFromURL(family: string, url: string) void`

Register a font from a URL (downloads and caches automatically).

**Source:** [std/gui/init.st:963](std/gui/init.st#L963)

---

### `registerFontVariantFromURL(family: string, url: string, variant: string) void`

Register a font variant from a URL.

**Source:** [std/gui/init.st:968](std/gui/init.st#L968)

---

### `setDefaultFont(family: string) void`

Set the default font family for all text rendering.

**Source:** [std/gui/init.st:977](std/gui/init.st#L977)

---

## Style Functions

### `setSize(id: int, width: float, height: float) void`

Set the size of a widget.

**Source:** [std/gui/init.st:986](std/gui/init.st#L986)

---

### `setPadding(id: int, all: float) void`

Set padding on all sides.

**Source:** [std/gui/init.st:991](std/gui/init.st#L991)

---

### `setMargin(id: int, all: float) void`

Set margin on all sides.

**Source:** [std/gui/init.st:996](std/gui/init.st#L996)

---

### `setBackground(id: int, r: int, g: int, b: int) void`

Set background color (RGB).

**Source:** [std/gui/init.st:1001](std/gui/init.st#L1001)

---

### `setBorder(id: int, width: float, r: int, g: int, b: int) void`

Set border with width and color.

**Source:** [std/gui/init.st:1006](std/gui/init.st#L1006)

---

### `setBorderRadius(id: int, radius: float) void`

Set border radius (rounded corners).

**Source:** [std/gui/init.st:1011](std/gui/init.st#L1011)

---

### `setShadow(id: int) void`

Add a drop shadow.

**Source:** [std/gui/init.st:1016](std/gui/init.st#L1016)

---

### `setVisible(id: int, visible: bool) void`

Set visibility.

**Source:** [std/gui/init.st:1021](std/gui/init.st#L1021)

---

### `setEnabled(id: int, enabled: bool) void`

Set enabled/disabled state.

**Source:** [std/gui/init.st:1026](std/gui/init.st#L1026)

---

### `setFlex(id: int, flex: float) void`

Set flex grow factor.

**Source:** [std/gui/init.st:1031](std/gui/init.st#L1031)

---

### `setOpacity(id: int, opacity: float) void`

Set opacity (0.0 to 1.0).

**Source:** [std/gui/init.st:1036](std/gui/init.st#L1036)

---

## Lifecycle Hooks

### `onMount(widgetId: int, callback: any) void`

Register a callback that runs once after a widget is added to the tree.

**Source:** [std/gui/init.st:1182](std/gui/init.st#L1182)

---

### `onDestroy(widgetId: int, callback: any) void`

Register a callback that runs when a widget is removed from the tree.

**Source:** [std/gui/init.st:1187](std/gui/init.st#L1187)

---

### `onUpdate(widgetId: int, callback: any) void`

Register a callback that runs when a widget is re-rendered.

**Source:** [std/gui/init.st:1192](std/gui/init.st#L1192)

---

### `triggerMount(widgetId: int) void`

Trigger mount callbacks for a widget.

**Source:** [std/gui/init.st:1197](std/gui/init.st#L1197)

---

### `triggerDestroy(widgetId: int) void`

Trigger destroy callbacks for a widget.

**Source:** [std/gui/init.st:1202](std/gui/init.st#L1202)

---

## Form System

### `Form(fields: array<int>) int`

Create a form from an array of field widget IDs.

**Source:** [std/gui/init.st:1424](std/gui/init.st#L1424)

---

### `FormWithNames(names: array<string>, fieldIds: array<int>) int`

Create a form with named fields.

**Source:** [std/gui/init.st:1433](std/gui/init.st#L1433)

---

### `FormField(label: string, inputId: int) int`

Create a form field with a label wrapping an input widget.

**Source:** [std/gui/init.st:1394](std/gui/init.st#L1394)

---

### `addRequired(fieldId: int, errorMsg: string) void`

Add a required validation rule to a form field.

**Source:** [std/gui/init.st:1399](std/gui/init.st#L1399)

---

### `addMinLength(fieldId: int, minLen: int, errorMsg: string) void`

Add a minimum length validation rule.

**Source:** [std/gui/init.st:1404](std/gui/init.st#L1404)

---

### `addMaxLength(fieldId: int, maxLen: int, errorMsg: string) void`

Add a maximum length validation rule.

**Source:** [std/gui/init.st:1409](std/gui/init.st#L1409)

---

### `addEmailValidation(fieldId: int, errorMsg: string) void`

Add an email validation rule.

**Source:** [std/gui/init.st:1414](std/gui/init.st#L1414)

---

### `getFieldError(fieldId: int) string`

Get the current error message of a form field.

**Source:** [std/gui/init.st:1419](std/gui/init.st#L1419)

---

### `validateForm(formId: int) bool`

Validate all fields in a form.

**Source:** [std/gui/init.st:1444](std/gui/init.st#L1444)

---

### `resetForm(formId: int) void`

Reset all field errors in a form.

**Source:** [std/gui/init.st:1449](std/gui/init.st#L1449)

---

### `submitForm(formId: int) void`

Submit a form (validates first, then calls onSubmit if valid).

**Source:** [std/gui/init.st:1454](std/gui/init.st#L1454)

---

### `onFormSubmit(formId: int, callback: any) void`

Set the submit callback for a form.

**Source:** [std/gui/init.st:1459](std/gui/init.st#L1459)

---

## Gesture Detection

### `GestureDetector(child: int) int`

Create a gesture detector that wraps a child widget. Detects double-tap, long-press, drag, and swipe gestures.

**Source:** [std/gui/init.st:1469](std/gui/init.st#L1469)

---

### `onDoubleTap(id: int, callback: any) void`

Set a double-tap handler.

**Source:** [std/gui/init.st:1476](std/gui/init.st#L1476)

---

### `onLongPress(id: int, callback: any) void`

Set a long-press handler.

**Source:** [std/gui/init.st:1481](std/gui/init.st#L1481)

---

### `onDragStart(id: int, callback: any) void`

Set a drag start handler. Callback: `fn(x: float, y: float) void`

**Source:** [std/gui/init.st:1486](std/gui/init.st#L1486)

---

### `onDrag(id: int, callback: any) void`

Set a drag handler. Callback: `fn(x: float, y: float, dx: float, dy: float) void`

**Source:** [std/gui/init.st:1491](std/gui/init.st#L1491)

---

### `onDragEnd(id: int, callback: any) void`

Set a drag end handler.

**Source:** [std/gui/init.st:1496](std/gui/init.st#L1496)

---

### `onSwipe(id: int, callback: any) void`

Set a swipe handler. Callback: `fn(direction: string) void`. Direction: `"left"`, `"right"`, `"up"`, `"down"`.

**Source:** [std/gui/init.st:1502](std/gui/init.st#L1502)

---

## Error Boundary

### `ErrorBoundary(children: array<int>) int`

Create an error boundary that catches child rendering errors.

**Source:** [std/gui/init.st:1512](std/gui/init.st#L1512)

---

### `ErrorBoundaryWithFallback(fallbackId: int, children: array<int>) int`

Create an error boundary with a fallback widget.

**Source:** [std/gui/init.st:1521](std/gui/init.st#L1521)

---

### `onError(id: int, callback: any) void`

Set the error callback. Callback: `fn(message: string) void`

**Source:** [std/gui/init.st:1532](std/gui/init.st#L1532)

---

### `triggerError(id: int, message: string) void`

Manually trigger an error.

**Source:** [std/gui/init.st:1537](std/gui/init.st#L1537)

---

### `resetError(id: int) void`

Reset an error boundary to its normal state.

**Source:** [std/gui/init.st:1542](std/gui/init.st#L1542)

---

### `hasError(id: int) bool`

Check if an error boundary currently has an error.

**Source:** [std/gui/init.st:1547](std/gui/init.st#L1547)

---

## Responsive / MediaQuery

### `getWindowWidth() float`

Get the current window width.

**Source:** [std/gui/init.st:1556](std/gui/init.st#L1556)

---

### `getWindowHeight() float`

Get the current window height.

**Source:** [std/gui/init.st:1561](std/gui/init.st#L1561)

---

### `getOrientation() string`

Get the current orientation: `"portrait"` or `"landscape"`.

**Source:** [std/gui/init.st:1566](std/gui/init.st#L1566)

---

### `getBreakpoint() string`

Get the current breakpoint: `"mobile"`, `"tablet"`, or `"desktop"`.

**Source:** [std/gui/init.st:1571](std/gui/init.st#L1571)

---

### `Responsive(mobileBuilder: any, tabletBuilder: any, desktopBuilder: any) int`

Create a responsive widget that shows different content based on screen size.

**Source:** [std/gui/init.st:1577](std/gui/init.st#L1577)

---

### `ResponsiveMD(mobileBuilder: any, desktopBuilder: any) int`

Create a responsive widget with only mobile and desktop builders. Tablet falls back to desktop.

**Source:** [std/gui/init.st:1587](std/gui/init.st#L1587)

---

## Async Widgets

### `FutureBuilder(loadingId: int, successId: int, errorId: int) int`

Create a FutureBuilder that shows different widgets based on async state.

**Source:** [std/gui/init.st:1600](std/gui/init.st#L1600)

---

### `setAsyncState(id: int, state: string) void`

Set the async state: `"loading"`, `"success"`, or `"error"`.

**Source:** [std/gui/init.st:1609](std/gui/init.st#L1609)

---

### `getAsyncState(id: int) string`

Get the current async state.

**Source:** [std/gui/init.st:1614](std/gui/init.st#L1614)

---

### `Suspense(fallbackId: int, children: array<int>) int`

Create a Suspense widget that shows a fallback until content is ready.

**Source:** [std/gui/init.st:1619](std/gui/init.st#L1619)

---

### `setSuspenseReady(id: int, ready: bool) void`

Set whether a Suspense widget's content is ready to show.

**Source:** [std/gui/init.st:1629](std/gui/init.st#L1629)

---

## Transition Animations

### `AnimatedContainer(duration: float, children: array<int>) int`

Create an animated container that smoothly transitions size and color.

**Source:** [std/gui/init.st:1638](std/gui/init.st#L1638)

---

### `animateWidth(id: int, width: float) void`

Animate the container width to a target value.

**Source:** [std/gui/init.st:1644](std/gui/init.st#L1644)

---

### `animateHeight(id: int, height: float) void`

Animate the container height to a target value.

**Source:** [std/gui/init.st:1649](std/gui/init.st#L1649)

---

### `FadeTransition(duration: float, children: array<int>) int`

Create a fade transition wrapper.

**Source:** [std/gui/init.st:1654](std/gui/init.st#L1654)

---

### `fadeIn(id: int) void`

Fade the widget in (opacity 0 -> 1).

**Source:** [std/gui/init.st:1660](std/gui/init.st#L1660)

---

### `fadeOut(id: int) void`

Fade the widget out (opacity 1 -> 0).

**Source:** [std/gui/init.st:1665](std/gui/init.st#L1665)

---

### `SlideTransition(direction: string, duration: float, children: array<int>) int`

Create a slide transition wrapper. Direction: `"left"`, `"right"`, `"up"`, `"down"`.

**Source:** [std/gui/init.st:1671](std/gui/init.st#L1671)

---

### `slideIn(id: int) void`

Slide the widget in from off-screen.

**Source:** [std/gui/init.st:1677](std/gui/init.st#L1677)

---

### `slideOut(id: int) void`

Slide the widget out off-screen.

**Source:** [std/gui/init.st:1682](std/gui/init.st#L1682)

---

### `ScaleTransition(duration: float, children: array<int>) int`

Create a scale transition wrapper.

**Source:** [std/gui/init.st:1687](std/gui/init.st#L1687)

---

### `scaleIn(id: int) void`

Scale the widget in (0 -> 1).

**Source:** [std/gui/init.st:1693](std/gui/init.st#L1693)

---

### `scaleOut(id: int) void`

Scale the widget out (1 -> 0).

**Source:** [std/gui/init.st:1698](std/gui/init.st#L1698)

---

## Cleanup

### `destroy(id: int) void`

Destroy a single widget.

**Source:** [std/gui/init.st:1707](std/gui/init.st#L1707)

---

### `destroyAll() void`

Destroy all widgets and the app.

**Source:** [std/gui/init.st:1712](std/gui/init.st#L1712)

---

## Complete Example

```stratos
use gui;

fn main() {
    val app = App("Counter App", 400, 300);
    app.setTheme("light");

    val count = Signal(0);
    val label = Text("Count: 0");

    createEffect(fn() void {
        setText(label, "Count: ${count.get()}");
    });

    val incBtn = Button("+ Increment");
    onClick(incBtn, fn() void {
        count.set(count.get() as int + 1);
    });

    val decBtn = Button("- Decrement");
    onClick(decBtn, fn() void {
        count.set(count.get() as int - 1);
    });

    app.root(Center([
        Column(10.0, [
            BoldText("Counter Demo", 24.0),
            label,
            Row(10.0, [decBtn, incBtn])
        ])
    ]));

    app.run();
}
```
