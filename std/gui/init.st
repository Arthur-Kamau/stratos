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
fn __gui_text_set_font_size(id: int, size: float) bool;
fn __gui_text_set_color(id: int, r: int, g: int, b: int) bool;
fn __gui_text_set_bold(id: int) bool;
fn __gui_text_set_align(id: int, align: string) bool;

fn __gui_button_create(label: string) int;
fn __gui_button_set_color(id: int, r: int, g: int, b: int) bool;

fn __gui_box_create() int;
fn __gui_image_create(path: string) int;

fn __gui_textfield_create(placeholder: string) int;
fn __gui_textfield_get_value(id: int) string;
fn __gui_textfield_set_value(id: int, value: string) bool;

fn __gui_checkbox_create(label: string) int;
fn __gui_checkbox_is_checked(id: int) bool;

fn __gui_slider_create(min: float, max: float) int;
fn __gui_slider_get_value(id: int) float;

fn __gui_switch_create() int;
fn __gui_switch_is_on(id: int) bool;

fn __gui_appbar_create(title: string) int;

fn __gui_dialog_create(title: string) int;
fn __gui_dialog_open(id: int, open: bool) bool;

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
fn __gui_widget_set_onclick(id: int) bool;

fn __gui_destroy_widget(id: int) bool;
fn __gui_destroy_all() bool;

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
///
/// Example:
///   val label = gui.Text("Hello, World!");
///   val big = gui.Text("Title", { fontSize: 24.0, bold: true });
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
///
/// Example:
///   val btn = gui.Button("Click Me");
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

/// Create a checkbox with a label.
fn Checkbox(label: string) int {
    return __gui_checkbox_create(label);
}

/// Check if a checkbox is checked.
fn isChecked(id: int) bool {
    return __gui_checkbox_is_checked(id);
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
///
/// Example:
///   val row = gui.Row(8.0, [
///       gui.Text("Left"),
///       gui.Spacer(),
///       gui.Text("Right"),
///   ]);
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
///
/// Example:
///   val col = gui.Column(16.0, [
///       gui.Text("Title"),
///       gui.Text("Subtitle"),
///       gui.Button("Action"),
///   ]);
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
