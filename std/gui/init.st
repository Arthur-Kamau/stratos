package gui;

/// GUI module for Stratos — native widgets rendered with SDL2/Skia.
///
/// Usage (unqualified — recommended):
///   use gui;
///   val app = App("My App", 800, 600);
///   app.root(Center([Text("Hello, GUI!")]));
///   app.run();
///
/// Usage (qualified):
///   use gui;
///   val app = gui.App("My App", 800, 600);
///   app.root(gui.Center([gui.Text("Hello, GUI!")]));
///   app.run();

// ============================================================
// Native function declarations
// ============================================================

// App lifecycle
fn __gui_app_create(title: string, width: int, height: int) bool;
fn __gui_app_run() bool;
fn __gui_app_quit() bool;
fn __gui_app_set_root(widgetId: int) bool;
fn __gui_app_set_theme(theme: string) bool;
fn __gui_app_set_fps(fps: int) bool;

// Widget creation
fn __gui_text_create(text: string) int;
fn __gui_text_set_content(id: int, text: string) bool;
fn __gui_text_set_font_size(id: int, size: float) bool;
fn __gui_text_set_color(id: int, r: int, g: int, b: int) bool;
fn __gui_text_set_bold(id: int) bool;
fn __gui_text_set_align(id: int, align: string) bool;
fn __gui_text_set_weight(id: int, weight: string) bool;
fn __gui_text_set_italic(id: int, italic: bool) bool;
fn __gui_text_set_font_family(id: int, family: string) bool;

// Generic typography (works on Text, Button, Icon, etc.)
fn __gui_widget_set_font_size(id: int, size: float) bool;
fn __gui_widget_set_text_color(id: int, r: int, g: int, b: int) bool;
fn __gui_widget_set_font_weight(id: int, weight: string) bool;
fn __gui_widget_set_font_family(id: int, family: string) bool;
fn __gui_widget_set_italic(id: int, italic: bool) bool;

fn __gui_button_create(label: string) int;
fn __gui_button_set_color(id: int, r: int, g: int, b: int) bool;

fn __gui_box_create() int;
fn __gui_image_create(path: string) int;

fn __gui_textfield_create(placeholder: string) int;
fn __gui_textfield_get_value(id: int) string;
fn __gui_textfield_set_value(id: int, value: string) bool;

fn __gui_textarea_create(placeholder: string) int;
fn __gui_textarea_get_value(id: int) string;
fn __gui_textarea_set_value(id: int, value: string) bool;

fn __gui_checkbox_create(label: string) int;
fn __gui_checkbox_is_checked(id: int) bool;

fn __gui_radiobutton_create(label: string, group: string) int;
fn __gui_radiobutton_is_selected(id: int) bool;

fn __gui_dropdown_create(placeholder: string) int;
fn __gui_dropdown_add_item(id: int, item: string) bool;
fn __gui_dropdown_get_selected(id: int) string;
fn __gui_dropdown_get_selected_index(id: int) int;
fn __gui_dropdown_set_selected(id: int, index: int) bool;

fn __gui_icon_create(icon: string, size: float) int;
fn __gui_icon_set_color(id: int, r: int, g: int, b: int) bool;

fn __gui_slider_create(min: float, max: float) int;
fn __gui_slider_get_value(id: int) float;

fn __gui_switch_create() int;
fn __gui_switch_is_on(id: int) bool;

fn __gui_appbar_create(title: string) int;

fn __gui_dialog_create(title: string) int;
fn __gui_dialog_open(id: int, open: bool) bool;

fn __gui_drawer_create(width: float) int;
fn __gui_drawer_open(id: int, open: bool) bool;
fn __gui_drawer_is_open(id: int) bool;

fn __gui_tabbar_create() int;
fn __gui_tabbar_add_tab(id: int, label: string) bool;
fn __gui_tabbar_get_active(id: int) int;
fn __gui_tabbar_set_active(id: int, index: int) bool;

fn __gui_menu_create() int;
fn __gui_menu_add_item(id: int, label: string) bool;
fn __gui_menu_add_separator(id: int) bool;
fn __gui_menu_show(id: int, x: float, y: float) bool;
fn __gui_menu_hide(id: int) bool;

fn __gui_padding_create(all: float) int;

// Layout creation
fn __gui_row_create(spacing: float) int;
fn __gui_row_set_alignment(id: int, main: string, cross: string) bool;
fn __gui_column_create(spacing: float) int;
fn __gui_column_set_alignment(id: int, main: string, cross: string) bool;
fn __gui_stack_create() int;
fn __gui_center_create() int;
fn __gui_spacer_create(size: float) int;
fn __gui_scrollview_create() int;
fn __gui_grid_create(columns: int, spacing: float) int;

// Widget operations
fn __gui_widget_add_child(parentId: int, childId: int) bool;
fn __gui_widget_set_size(id: int, width: float, height: float) bool;
fn __gui_widget_set_padding(id: int, all: float) bool;
fn __gui_widget_set_margin(id: int, all: float) bool;
fn __gui_widget_set_background(id: int, r: int, g: int, b: int) bool;
fn __gui_widget_set_border(id: int, width: float, r: int, g: int, b: int) bool;
fn __gui_widget_set_border_radius(id: int, radius: float) bool;
fn __gui_widget_set_shadow(id: int) bool;
fn __gui_widget_set_visible(id: int, visible: bool) bool;
fn __gui_widget_set_enabled(id: int, enabled: bool) bool;
fn __gui_widget_set_flex(id: int, flex: float) bool;
fn __gui_widget_set_opacity(id: int, opacity: float) bool;

// Event handlers (closure-based)
fn __gui_widget_set_onclick(id: int, callback: any) int;
fn __gui_widget_set_onchange(id: int, callback: any) int;
fn __gui_widget_set_onsubmit(id: int, callback: any) int;
fn __gui_widget_set_onmouseenter(id: int, callback: any) int;
fn __gui_widget_set_onmouseleave(id: int, callback: any) int;
fn __gui_widget_set_onkeypress(id: int, callback: any) int;

// State management (legacy)
fn __gui_state_create(initial: any) int;
fn __gui_state_get(id: int) any;
fn __gui_state_set(id: int, value: any) bool;
fn __gui_state_watch(stateId: int, callbackId: int) bool;

// Signal system (advanced reactivity)
fn __gui_signal_create(initial: any) int;
fn __gui_signal_get(id: int) any;
fn __gui_signal_set(id: int, value: any) bool;
fn __gui_signal_peek(id: int) any;
fn __gui_effect_create(closure: any) int;
fn __gui_effect_dispose(id: int) bool;
fn __gui_computed_create(closure: any) int;
fn __gui_computed_get(id: int) any;
fn __gui_batch(closure: any) bool;
fn __gui_untrack(closure: any) any;
fn __gui_on_cleanup(closure: any) bool;

// Router
fn __gui_router_create() int;
fn __gui_router_add_route(routerId: int, path: string, builder: any) bool;
fn __gui_router_add_guard(routerId: int, path: string, guardFn: any) bool;
fn __gui_router_set_not_found(routerId: int, builder: any) bool;
fn __gui_router_navigate(routerId: int, path: string) bool;
fn __gui_router_back(routerId: int) bool;
fn __gui_router_forward(routerId: int) bool;
fn __gui_router_replace(routerId: int, path: string) bool;
fn __gui_router_get_param(routerId: int, name: string) string;
fn __gui_router_get_query(routerId: int, name: string) string;
fn __gui_router_get_path(routerId: int) string;
fn __gui_router_get_widget(routerId: int) int;

fn __gui_destroy_widget(id: int) bool;
fn __gui_destroy_all() bool;

// Font management
fn __gui_font_register_file(family: string, path: string, variant: string) bool;
fn __gui_font_register_url(family: string, url: string, variant: string) bool;
fn __gui_font_set_default(family: string) bool;

// ============================================================
// App class
// ============================================================

/// Application window and event loop.
///
/// Example:
///   val app = gui.App("My App", 800, 600);
///   app.root(gui.Text("Hello!"));
///   app.run();
class App {
    var title: string;
    var width: int;
    var height: int;
    var rootWidget: int;

    constructor(title: string, width: int, height: int) {
        this.title = title;
        this.width = width;
        this.height = height;
        this.rootWidget = 0;
        __gui_app_create(title, width, height);
    }

    /// Set the root widget of the application.
    fn root(widget: int) void {
        this.rootWidget = widget;
        __gui_app_set_root(widget);
    }

    /// Start the event loop (blocks until window is closed).
    fn run() void {
        __gui_app_run();
    }

    /// Stop the event loop.
    fn quit() void {
        __gui_app_quit();
    }

    /// Set the color theme ("light" or "dark").
    fn setTheme(theme: string) void {
        __gui_app_set_theme(theme);
    }

    /// Set target frames per second.
    fn setFPS(fps: int) void {
        __gui_app_set_fps(fps);
    }
}

// ============================================================
// State management
// ============================================================

/// Reactive state container.
///
/// Example:
///   val count = State(0);
///   println(count.get());
///   count.set(count.get() + 1);
class State {
    var id: int;

    constructor(initial: any) {
        this.id = __gui_signal_create(initial);
    }

    fn get() any {
        return __gui_signal_get(this.id);
    }

    fn set(value: any) void {
        __gui_signal_set(this.id, value);
    }
}

/// Derived state that computes a value from other state.
/// Wraps Computed (Signal-based) for automatic dependency tracking.
class DerivedState {
    var id: int;

    constructor(compute: any) {
        this.id = __gui_computed_create(compute);
    }

    fn get() any {
        return __gui_computed_get(this.id);
    }
}

// ============================================================
// Signal — fine-grained reactive primitive
// ============================================================

/// Reactive signal with auto-tracking.
/// When read inside a createEffect() callback, the effect automatically
/// re-runs when this signal changes.
///
/// Example:
///   val count = Signal(0);
///   createEffect(fn() void {
///       println("Count changed: ${count.get()}");
///   });
///   count.set(1);  // effect re-runs automatically
class Signal {
    var id: int;

    constructor(initial: any) {
        this.id = __gui_signal_create(initial);
    }

    /// Get the value. If called inside an effect, subscribes the effect.
    fn get() any {
        return __gui_signal_get(this.id);
    }

    /// Set the value. Notifies all subscribed effects.
    fn set(value: any) void {
        __gui_signal_set(this.id, value);
    }

    /// Read without tracking (won't subscribe the current effect).
    fn peek() any {
        return __gui_signal_peek(this.id);
    }
}

/// Computed (memoized) signal that derives from other signals.
/// Automatically recomputes when any dependency changes.
///
/// Example:
///   val count = Signal(5);
///   val doubled = Computed(fn() any => count.get() as int * 2);
///   println(doubled.get());  // 10
///   count.set(10);
///   println(doubled.get());  // 20
class Computed {
    var id: int;

    constructor(compute: any) {
        this.id = __gui_computed_create(compute);
    }

    /// Get the cached value. Recomputes if dependencies changed.
    fn get() any {
        return __gui_computed_get(this.id);
    }
}

/// Create a reactive side-effect that auto-tracks signal dependencies.
/// Returns an effect ID that can be passed to disposeEffect().
///
/// Example:
///   val name = Signal("World");
///   val effectId = createEffect(fn() void {
///       println("Hello, ${name.get()}!");
///   });
///   name.set("Stratos");  // prints "Hello, Stratos!"
fn createEffect(closure: any) int {
    return __gui_effect_create(closure);
}

/// Dispose (stop) a previously created effect.
fn disposeEffect(id: int) void {
    __gui_effect_dispose(id);
}

/// Batch multiple signal updates — effects only run once at the end.
///
/// Example:
///   batch(fn() void {
///       x.set(1);
///       y.set(2);
///       z.set(3);
///       // effects that depend on x, y, z only run once here
///   });
fn batch(closure: any) void {
    __gui_batch(closure);
}

/// Read signals without tracking — the current effect won't subscribe.
///
/// Example:
///   createEffect(fn() void {
///       val tracked = count.get();     // subscribes
///       val ignored = untrack(fn() any => other.get());  // doesn't subscribe
///   });
fn untrack(closure: any) any {
    return __gui_untrack(closure);
}

/// Register a cleanup function for the current effect.
/// Runs before the effect re-executes or when disposed.
///
/// Example:
///   createEffect(fn() void {
///       val timer = startTimer();
///       onCleanup(fn() void { stopTimer(timer); });
///   });
fn onCleanup(closure: any) void {
    __gui_on_cleanup(closure);
}

// ============================================================
// Widget helper — adds children to a parent
// ============================================================

fn addChildren(parentId: int, children: array<int>) int {
    for val child in children {
        __gui_widget_add_child(parentId, child);
    }
    return parentId;
}

// ============================================================
// Text widget
// ============================================================

/// Create a text label.
fn Text(text: string) int {
    return __gui_text_create(text);
}

/// Create a styled text label.
fn StyledText(text: string, fontSize: float, r: int, g: int, b: int) int {
    val id = __gui_text_create(text);
    __gui_text_set_font_size(id, fontSize);
    __gui_text_set_color(id, r, g, b);
    return id;
}

/// Create bold text.
fn BoldText(text: string, fontSize: float) int {
    val id = __gui_text_create(text);
    __gui_text_set_font_size(id, fontSize);
    __gui_text_set_bold(id);
    return id;
}

// ============================================================
// Button widget
// ============================================================

/// Create a clickable button.
fn Button(label: string) int {
    return __gui_button_create(label);
}

/// Create a colored button.
fn ColoredButton(label: string, r: int, g: int, b: int) int {
    val id = __gui_button_create(label);
    __gui_button_set_color(id, r, g, b);
    return id;
}

// ============================================================
// Container / Box
// ============================================================

/// Create a generic container box.
fn Box(children: array<int>) int {
    val id = __gui_box_create();
    return addChildren(id, children);
}

/// Create a styled container.
fn Container(r: int, g: int, b: int, borderRadius: float, children: array<int>) int {
    val id = __gui_box_create();
    __gui_widget_set_background(id, r, g, b);
    __gui_widget_set_border_radius(id, borderRadius);
    return addChildren(id, children);
}

// ============================================================
// Image widget
// ============================================================

/// Display an image from a file path.
fn Image(path: string) int {
    return __gui_image_create(path);
}

/// Display an image with a fixed size.
fn SizedImage(path: string, width: float, height: float) int {
    val id = __gui_image_create(path);
    __gui_widget_set_size(id, width, height);
    return id;
}

// ============================================================
// Input widgets
// ============================================================

/// Create a single-line text input field.
fn TextField(placeholder: string) int {
    return __gui_textfield_create(placeholder);
}

/// Get the current value of a text field.
fn getTextFieldValue(id: int) string {
    return __gui_textfield_get_value(id);
}

/// Set the value of a text field.
fn setTextFieldValue(id: int, value: string) void {
    __gui_textfield_set_value(id, value);
}

/// Create a multi-line text input area.
fn TextArea(placeholder: string) int {
    return __gui_textarea_create(placeholder);
}

/// Get the current value of a text area.
fn getTextAreaValue(id: int) string {
    return __gui_textarea_get_value(id);
}

/// Set the value of a text area.
fn setTextAreaValue(id: int, value: string) void {
    __gui_textarea_set_value(id, value);
}

/// Create a checkbox with a label.
fn Checkbox(label: string) int {
    return __gui_checkbox_create(label);
}

/// Check if a checkbox is checked.
fn isChecked(id: int) bool {
    return __gui_checkbox_is_checked(id);
}

/// Create a radio button with a label and group name.
/// Only one radio button per group can be selected at a time.
fn RadioButton(label: string, group: string) int {
    return __gui_radiobutton_create(label, group);
}

/// Check if a radio button is selected.
fn isRadioSelected(id: int) bool {
    return __gui_radiobutton_is_selected(id);
}

/// Create a dropdown/select widget.
fn Dropdown(placeholder: string) int {
    return __gui_dropdown_create(placeholder);
}

/// Add an item to a dropdown.
fn addDropdownItem(id: int, item: string) void {
    __gui_dropdown_add_item(id, item);
}

/// Get the selected item text from a dropdown.
fn getDropdownSelected(id: int) string {
    return __gui_dropdown_get_selected(id);
}

/// Get the selected index from a dropdown (-1 if none).
fn getDropdownSelectedIndex(id: int) int {
    return __gui_dropdown_get_selected_index(id);
}

/// Set the selected index of a dropdown.
fn setDropdownSelected(id: int, index: int) void {
    __gui_dropdown_set_selected(id, index);
}

/// Create an icon widget (Unicode character or symbol).
fn Icon(icon: string, size: float) int {
    return __gui_icon_create(icon, size);
}

/// Create a range slider.
fn Slider(min: float, max: float) int {
    return __gui_slider_create(min, max);
}

/// Get the current slider value.
fn getSliderValue(id: int) float {
    return __gui_slider_get_value(id);
}

/// Create a toggle switch.
fn Switch() int {
    return __gui_switch_create();
}

/// Check if a switch is on.
fn isSwitchOn(id: int) bool {
    return __gui_switch_is_on(id);
}

// ============================================================
// Layout widgets
// ============================================================

/// Arrange children in a horizontal row.
fn Row(spacing: float, children: array<int>) int {
    val id = __gui_row_create(spacing);
    return addChildren(id, children);
}

/// Create a row with alignment options.
fn AlignedRow(spacing: float, mainAlign: string, crossAlign: string, children: array<int>) int {
    val id = __gui_row_create(spacing);
    __gui_row_set_alignment(id, mainAlign, crossAlign);
    return addChildren(id, children);
}

/// Arrange children in a vertical column.
fn Column(spacing: float, children: array<int>) int {
    val id = __gui_column_create(spacing);
    return addChildren(id, children);
}

/// Create a column with alignment options.
fn AlignedColumn(spacing: float, mainAlign: string, crossAlign: string, children: array<int>) int {
    val id = __gui_column_create(spacing);
    __gui_column_set_alignment(id, mainAlign, crossAlign);
    return addChildren(id, children);
}

/// Stack children on top of each other (z-axis).
fn Stack(children: array<int>) int {
    val id = __gui_stack_create();
    return addChildren(id, children);
}

/// Center a child widget.
fn Center(children: array<int>) int {
    val id = __gui_center_create();
    return addChildren(id, children);
}

/// Wrap children with padding on all sides.
fn Padding(all: float, children: array<int>) int {
    val id = __gui_padding_create(all);
    return addChildren(id, children);
}

/// Create flexible space between widgets.
fn Spacer() int {
    return __gui_spacer_create(0.0);
}

/// Create a fixed-size spacer.
fn FixedSpacer(size: float) int {
    return __gui_spacer_create(size);
}

/// Create a scrollable container.
fn ScrollView(children: array<int>) int {
    val id = __gui_scrollview_create();
    return addChildren(id, children);
}

/// Create a grid layout.
fn Grid(columns: int, spacing: float, children: array<int>) int {
    val id = __gui_grid_create(columns, spacing);
    return addChildren(id, children);
}

// ============================================================
// Navigation widgets
// ============================================================

/// Create a top app bar with a title.
fn AppBar(title: string) int {
    return __gui_appbar_create(title);
}

/// Create a modal dialog.
fn Dialog(title: string, children: array<int>) int {
    val id = __gui_dialog_create(title);
    return addChildren(id, children);
}

/// Open or close a dialog.
fn openDialog(id: int, open: bool) void {
    __gui_dialog_open(id, open);
}

/// Create a side drawer panel.
fn Drawer(width: float, children: array<int>) int {
    val id = __gui_drawer_create(width);
    return addChildren(id, children);
}

/// Open or close a drawer.
fn openDrawer(id: int, open: bool) void {
    __gui_drawer_open(id, open);
}

/// Check if a drawer is open.
fn isDrawerOpen(id: int) bool {
    return __gui_drawer_is_open(id);
}

/// Create a tab bar with labeled tabs.
fn TabBar(tabs: array<string>) int {
    val id = __gui_tabbar_create();
    for val tab in tabs {
        __gui_tabbar_add_tab(id, tab);
    }
    return id;
}

/// Get the active tab index.
fn getActiveTab(id: int) int {
    return __gui_tabbar_get_active(id);
}

/// Set the active tab index.
fn setActiveTab(id: int, index: int) void {
    __gui_tabbar_set_active(id, index);
}

/// Create a context menu.
fn Menu() int {
    return __gui_menu_create();
}

/// Add a menu item.
fn addMenuItem(id: int, label: string) void {
    __gui_menu_add_item(id, label);
}

/// Add a separator line to a menu.
fn addMenuSeparator(id: int) void {
    __gui_menu_add_separator(id);
}

/// Show a menu at a position.
fn showMenu(id: int, x: float, y: float) void {
    __gui_menu_show(id, x, y);
}

/// Hide a menu.
fn hideMenu(id: int) void {
    __gui_menu_hide(id);
}

// ============================================================
// Event handler helpers
// ============================================================

/// Update the text content of a Text widget.
fn setText(id: int, text: string) void {
    __gui_text_set_content(id, text);
}

/// Set a click handler on a widget. Callback is a closure: fn() void { ... }
fn onClick(id: int, callback: any) void {
    __gui_widget_set_onclick(id, callback);
}

/// Set a change handler on a text input or dropdown.
fn onChange(id: int, callback: any) void {
    __gui_widget_set_onchange(id, callback);
}

/// Set a submit handler on a text field.
fn onSubmit(id: int, callback: any) void {
    __gui_widget_set_onsubmit(id, callback);
}

/// Set a mouse enter handler.
fn onMouseEnter(id: int, callback: any) void {
    __gui_widget_set_onmouseenter(id, callback);
}

/// Set a mouse leave handler.
fn onMouseLeave(id: int, callback: any) void {
    __gui_widget_set_onmouseleave(id, callback);
}

/// Set a key press handler.
fn onKeyPress(id: int, callback: any) void {
    __gui_widget_set_onkeypress(id, callback);
}

// ============================================================
// Typography helpers
// ============================================================

/// Set font size on any widget (Text, Button, etc.).
fn setFontSize(id: int, size: float) void {
    __gui_widget_set_font_size(id, size);
}

/// Set text color on any widget (Text, Button, Icon).
fn setTextColor(id: int, r: int, g: int, b: int) void {
    __gui_widget_set_text_color(id, r, g, b);
}

/// Set font weight: "thin","light","regular","medium","semibold","bold","extrabold","black".
fn setFontWeight(id: int, weight: string) void {
    __gui_widget_set_font_weight(id, weight);
}

/// Set font family (e.g. "sans-serif", "monospace", "serif").
fn setFontFamily(id: int, family: string) void {
    __gui_widget_set_font_family(id, family);
}

/// Set italic style.
fn setItalic(id: int, italic: bool) void {
    __gui_widget_set_italic(id, italic);
}

/// Set text alignment on a Text widget: "left", "center", "right".
fn setTextAlign(id: int, align: string) void {
    __gui_text_set_align(id, align);
}

/// Create text with full typography control.
fn StyledTextFull(text: string, fontSize: float, r: int, g: int, b: int, weight: string, family: string) int {
    val id = __gui_text_create(text);
    __gui_text_set_font_size(id, fontSize);
    __gui_text_set_color(id, r, g, b);
    __gui_text_set_weight(id, weight);
    __gui_text_set_font_family(id, family);
    return id;
}

/// Create italic text.
fn ItalicText(text: string, fontSize: float) int {
    val id = __gui_text_create(text);
    __gui_text_set_font_size(id, fontSize);
    __gui_text_set_italic(id, true);
    return id;
}

/// Create monospace text (code-style).
fn MonoText(text: string, fontSize: float) int {
    val id = __gui_text_create(text);
    __gui_text_set_font_size(id, fontSize);
    __gui_text_set_font_family(id, "monospace");
    return id;
}

// ============================================================
// Font management
// ============================================================

/// Register a font from a local .ttf or .otf file.
/// Variant: "regular", "bold", "italic", "bolditalic", "variable"
///
/// Example:
///   registerFontFile("MyFont", "/path/to/MyFont-Regular.ttf");
///   registerFontFile("MyFont", "/path/to/MyFont-Bold.ttf", "bold");
fn registerFontFile(family: string, path: string) void {
    __gui_font_register_file(family, path, "regular");
}

/// Register a font variant from a local file.
fn registerFontVariant(family: string, path: string, variant: string) void {
    __gui_font_register_file(family, path, variant);
}

/// Register a font from a URL (downloads and caches automatically).
/// Supports Google Fonts direct download links.
///
/// Example:
///   registerFontFromURL("Roboto", "https://github.com/google/fonts/raw/main/ofl/roboto/Roboto%5Bwdth%2Cwght%5D.ttf");
fn registerFontFromURL(family: string, url: string) void {
    __gui_font_register_url(family, url, "regular");
}

/// Register a font variant from a URL.
fn registerFontVariantFromURL(family: string, url: string, variant: string) void {
    __gui_font_register_url(family, url, variant);
}

/// Set the default font family for all text rendering.
/// Inter is the default. Call this to change it.
///
/// Example:
///   setDefaultFont("Roboto");
fn setDefaultFont(family: string) void {
    __gui_font_set_default(family);
}

// ============================================================
// Style helpers
// ============================================================

/// Set the size of a widget.
fn setSize(id: int, width: float, height: float) void {
    __gui_widget_set_size(id, width, height);
}

/// Set padding on all sides.
fn setPadding(id: int, all: float) void {
    __gui_widget_set_padding(id, all);
}

/// Set margin on all sides.
fn setMargin(id: int, all: float) void {
    __gui_widget_set_margin(id, all);
}

/// Set background color (RGB).
fn setBackground(id: int, r: int, g: int, b: int) void {
    __gui_widget_set_background(id, r, g, b);
}

/// Set border.
fn setBorder(id: int, width: float, r: int, g: int, b: int) void {
    __gui_widget_set_border(id, width, r, g, b);
}

/// Set border radius (rounded corners).
fn setBorderRadius(id: int, radius: float) void {
    __gui_widget_set_border_radius(id, radius);
}

/// Add a drop shadow.
fn setShadow(id: int) void {
    __gui_widget_set_shadow(id);
}

/// Set visibility.
fn setVisible(id: int, visible: bool) void {
    __gui_widget_set_visible(id, visible);
}

/// Set enabled/disabled state.
fn setEnabled(id: int, enabled: bool) void {
    __gui_widget_set_enabled(id, enabled);
}

/// Set flex grow factor.
fn setFlex(id: int, flex: float) void {
    __gui_widget_set_flex(id, flex);
}

/// Set opacity (0.0 to 1.0).
fn setOpacity(id: int, opacity: float) void {
    __gui_widget_set_opacity(id, opacity);
}

// ============================================================
// Router — Client-side navigation
// ============================================================

/// Router for single-page app navigation.
///   val router = Router();
///   router.route("/", fn() int => HomePage());
///   router.route("/user/:id", fn() int => UserPage(router.param("id")));
///   router.notFound(fn() int => NotFoundPage());
///   router.navigate("/");
class Router {
    var id: int;

    constructor() {
        this.id = __gui_router_create();
    }

    /// Register a route with a path pattern and builder function.
    /// Patterns: "/" (exact), "/user/:id" (param), "/files/*" (wildcard)
    fn route(path: string, builder: any) void {
        __gui_router_add_route(this.id, path, builder);
    }

    /// Add a guard function to a route. Guard returns bool.
    fn guard(path: string, guardFn: any) void {
        __gui_router_add_guard(this.id, path, guardFn);
    }

    /// Set the 404 handler.
    fn notFound(builder: any) void {
        __gui_router_set_not_found(this.id, builder);
    }

    /// Navigate to a path. Matches route and executes builder.
    fn navigate(path: string) void {
        __gui_router_navigate(this.id, path);
    }

    /// Go back in history.
    fn back() void {
        __gui_router_back(this.id);
    }

    /// Go forward in history.
    fn forward() void {
        __gui_router_forward(this.id);
    }

    /// Replace current route without adding to history.
    fn replace(path: string) void {
        __gui_router_replace(this.id, path);
    }

    /// Get a route parameter value (e.g., ":id" → param("id")).
    fn param(name: string) string {
        return __gui_router_get_param(this.id, name);
    }

    /// Get a query parameter value (e.g., "?q=hello" → query("q")).
    fn query(name: string) string {
        return __gui_router_get_query(this.id, name);
    }

    /// Get the current route path.
    fn currentPath() string {
        return __gui_router_get_path(this.id);
    }

    /// Get the widget ID of the current route.
    fn widget() int {
        return __gui_router_get_widget(this.id);
    }
}

/// Convenience: create a navigation link button.
fn Link(text: string, path: string, router: any) int {
    val btn = Button(text);
    onClick(btn, fn() void {
        router.navigate(path);
    });
    return btn;
}

// ============================================================
// Cleanup
// ============================================================

/// Destroy a single widget.
fn destroy(id: int) void {
    __gui_destroy_widget(id);
}

/// Destroy all widgets and the app.
fn destroyAll() void {
    __gui_destroy_all();
}
