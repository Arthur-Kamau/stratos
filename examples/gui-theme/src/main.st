package main;

use gui;

fn main() {
    val app = App("Theme Demo", 600, 400);
    app.setTheme("dark");

    val title = BoldText("Theme Switcher", 24.0);
    val subtitle = Text("Toggle between light and dark themes");
    val lightBtn = ColoredButton("Light Theme", 255, 193, 7);
    val darkBtn = ColoredButton("Dark Theme", 55, 71, 79);

    onClick(lightBtn, fn() void {
        app.setTheme("light");
    });

    onClick(darkBtn, fn() void {
        app.setTheme("dark");
    });

    val buttons = Row(12.0, [lightBtn, darkBtn]);
    val input = TextField("Sample input field");
    val cb = Checkbox("Dark mode enabled");
    val slider = Slider(0.0, 100.0);

    val content = Column(16.0, [title, subtitle, buttons, input, cb, slider]);
    setPadding(content, 32.0);
    val root = Center([content]);

    app.root(root);
    app.run();
}
