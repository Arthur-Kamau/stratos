package main;

use gui;

fn main() {
    val app = App("Todo App", 500, 600);
    app.setTheme("light");

    val bar = AppBar("My Todos");
    val input = TextField("Add a new todo...");
    val addBtn = Button("Add");
    val inputRow = Row(8.0, [input, addBtn]);

    val cb1 = Checkbox("Learn Stratos GUI");
    val cb2 = Checkbox("Build a todo app");
    val cb3 = Checkbox("Add dark mode");
    val todoList = Column(4.0, [cb1, cb2, cb3]);
    val scroll = ScrollView([todoList]);

    val body = Column(16.0, [inputRow, scroll]);
    setPadding(body, 16.0);

    val root = Column(0.0, [bar, body]);

    app.root(root);
    app.run();
}
