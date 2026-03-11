package main;

use gui;

fn main() {
    val app = App("Scaffold Demo", 800, 600);

    // AppBar
    val appbar = AppBar("My App");

    // Progress bar
    val progress = ProgressBar(0.65);
    setSize(progress, 300.0, 6.0);

    // Cards with content
    val card1 = Card([
        Column(8.0, [
            BoldText("Welcome", 18.0),
            Text("This is a Card widget with elevation and shadow."),
            Divider(),
            progress
        ])
    ]);
    setSize(card1, 350.0, 150.0);

    // Chips in a Wrap layout
    val chip1 = Chip("Flutter");
    val chip2 = Chip("Compose");
    val chip3 = Chip("Stratos");
    setChipSelected(chip3, true);

    val chips = Wrap(8.0, [chip1, chip2, chip3]);

    // Badge
    val badge = Badge("3");

    // Expansion panel
    val panel = ExpansionPanel("More Details", [
        Padding(12.0, [
            Text("This content is hidden until expanded.")
        ])
    ]);

    // Body content
    val body = Padding(16.0, [
        Column(12.0, [
            card1,
            Row(8.0, [Text("Tags:"), chips]),
            Row(8.0, [Text("Notifications:"), badge]),
            panel
        ])
    ]);

    // FAB
    val fab = FAB("+");
    onClick(fab, fn() void {
        println("FAB clicked!");
    });

    // Scaffold combines appbar + body + FAB
    val scaffold = ScaffoldWithFAB(appbar, body, fab);

    app.root(scaffold);
    app.run();
}
