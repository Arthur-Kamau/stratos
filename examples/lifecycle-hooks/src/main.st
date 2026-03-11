package main;

use gui;

fn main() {
    val app = App("Lifecycle Hooks Demo", 600, 400);

    val label = Text("Hello, Lifecycle!");
    setFontSize(label, 20.0);

    // Register lifecycle hooks
    onMount(label, fn() void {
        println("[MOUNT] Label widget mounted");
    });

    onDestroy(label, fn() void {
        println("[DESTROY] Label widget destroyed");
    });

    onUpdate(label, fn() void {
        println("[UPDATE] Label widget updated");
    });

    // Trigger mount manually after adding to tree
    val layout = Center([
        Column(12.0, [
            label,
            Text("Check the console for lifecycle events.")
        ])
    ]);

    app.root(layout);

    // Trigger mount after widget is in the tree
    triggerMount(label);

    app.run();
}
