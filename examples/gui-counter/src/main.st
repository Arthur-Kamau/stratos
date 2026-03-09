package main;

use gui;

fn main() {
    val app = App("Counter", 400, 300);
    app.setTheme("light");

    val title = BoldText("Counter App", 24.0);
    val display = StyledText("0", 48.0, 33, 150, 243);
    val btnMinus = ColoredButton("-", 211, 47, 47);
    val btnReset = Button("Reset");
    val btnPlus = ColoredButton("+", 46, 125, 50);
    val buttons = Row(8.0, [btnMinus, btnReset, btnPlus]);
    val col = Column(16.0, [title, display, buttons]);
    val root = Center([col]);

    app.root(root);
    app.run();
}
