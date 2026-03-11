package main;

use gui;

fn main() {
    println("=== Batch Updates Demo ===");
    println("");

    // Create signals for a simple form
    val firstName = Signal("Alice");
    val lastName = Signal("Smith");
    val age = Signal(30);

    // Track how many times the effect runs
    var effectRuns = 0;

    val effectId = createEffect(fn() void {
        effectRuns = effectRuns + 1;
        println("  Effect #${effectRuns}: ${firstName.get()} ${lastName.get()}, age ${age.get()}");
    });

    // Without batch: each set() triggers the effect individually
    println("");
    println("--- Without batch (3 separate updates) ---");
    firstName.set("Bob");
    lastName.set("Jones");
    age.set(25);

    // With batch: all updates trigger the effect only once
    println("");
    println("--- With batch (3 updates, 1 effect run) ---");
    batch(fn() void {
        firstName.set("Charlie");
        lastName.set("Brown");
        age.set(8);
    });

    // Nested batch
    println("");
    println("--- Nested batch ---");
    batch(fn() void {
        firstName.set("Diana");
        batch(fn() void {
            lastName.set("Prince");
            age.set(28);
        });
        // Inner batch completes but outer batch still active
        // so effect hasn't fired yet
    });
    // Now the outer batch completes, effect fires once

    // Cleanup and untrack demo
    println("");
    println("--- Untrack: read without subscribing ---");
    val secret = Signal("hidden");
    val tracker = createEffect(fn() void {
        val peekVal = untrack(fn() any => secret.get());
        println("  Untrack effect ran (secret read without tracking)");
    });

    println("  Setting secret (effect should NOT re-run):");
    secret.set("revealed");
    println("  (no effect output = correct!)");

    // Cleanup
    disposeEffect(effectId);
    disposeEffect(tracker);

    println("");
    println("=== Done ===");
}
