package main;

use gui;

fn main() {
    val app = App("Layout Demo", 900, 700);

    val header = AppBar("Layout Showcase");

    val rowLabel = BoldText("Row Layout", 16.0);
    val row = Row(8.0, [Button("A"), Button("B"), Button("C"), Spacer(), Button("End")]);

    val colLabel = BoldText("Column Layout", 16.0);
    val col = Column(4.0, [Text("Line 1"), Text("Line 2"), Text("Line 3")]);

    val gridLabel = BoldText("Grid Layout (3 columns)", 16.0);
    val grid = Grid(3, 8.0, [
        Button("1"), Button("2"), Button("3"),
        Button("4"), Button("5"), Button("6")
    ]);

    val stackLabel = BoldText("Stack Layout", 16.0);
    val stack = Stack([
        Container(200, 200, 230, 8.0, []),
        Container(100, 150, 210, 8.0, [])
    ]);
    setSize(stack, 200.0, 100.0);

    val scrollLabel = BoldText("ScrollView", 16.0);
    val scrollContent = Column(4.0, [
        Text("Scroll item 1"), Text("Scroll item 2"), Text("Scroll item 3"),
        Text("Scroll item 4"), Text("Scroll item 5"), Text("Scroll item 6"),
        Text("Scroll item 7"), Text("Scroll item 8"), Text("Scroll item 9"),
        Text("Scroll item 10")
    ]);
    val scroll = ScrollView([scrollContent]);
    setSize(scroll, 300.0, 120.0);

    val spacerLabel = BoldText("Spacer", 16.0);
    val spacerDemo = Row(0.0, [Text("Left"), Spacer(), Text("Right")]);
    setSize(spacerDemo, 400.0, 30.0);

    val content = Column(16.0, [
        rowLabel, row,
        colLabel, col,
        gridLabel, grid,
        stackLabel, stack,
        scrollLabel, scroll,
        spacerLabel, spacerDemo
    ]);
    setPadding(content, 24.0);

    val page = Column(0.0, [header, content]);
    app.root(page);
    app.run();
}
