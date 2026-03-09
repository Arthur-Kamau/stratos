package main;

use gui;

fn main() {
    val app = App("Hello GUI", 800, 600);

    val title = BoldText("Welcome to Stratos GUI!", 28.0);
    val subtitle = Text("Built with SDL2 + Skia rendering");
    val btn1 = Button("Click Me");
    val btn2 = ColoredButton("Green", 46, 125, 50);
    val btn3 = ColoredButton("Red", 211, 47, 47);
    val buttons = Row(8.0, [btn1, btn2, btn3]);
    val input = TextField("Type something here...");
    val cb = Checkbox("Enable feature");
    val sw = Switch();
    val toggles = Row(16.0, [cb, sw]);
    val slider = Slider(0.0, 100.0);

    val col = Column(16.0, [title, subtitle, buttons, input, toggles, slider]);
    val root = Center([col]);

    app.root(root);
    app.run();
}
