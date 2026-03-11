package main;

use gui;

fn main() {
    println("=== Signal Basics ===");
    val count = Signal(0);
    println("Initial: ${count.get()}");

    count.set(5);
    println("After set(5): ${count.get()}");
    println("Peek: ${count.peek()}");

    println("");
    println("=== Computed ===");
    val x = Signal(10);
    val doubled = Computed(fn() any => x.get() as int * 2);
    println("x=10, doubled=${doubled.get()}");

    x.set(25);
    println("x=25, doubled=${doubled.get()}");

    println("");
    println("=== Effects ===");
    val name = Signal("World");

    val effectId = createEffect(fn() void {
        println("  Effect: Hello, ${name.get()}!");
    });

    name.set("Stratos");
    name.set("Signals");

    disposeEffect(effectId);
    name.set("Disposed");
    println("  (effect disposed - no output above)");

    println("");
    println("=== Batch ===");
    val a = Signal(1);
    val b = Signal(2);

    val batchEffectId = createEffect(fn() void {
        println("  Batch effect: a=${a.get()}, b=${b.get()}");
    });

    batch(fn() void {
        a.set(10);
        b.set(20);
    });

    disposeEffect(batchEffectId);

    println("");
    println("=== Done ===");
}
