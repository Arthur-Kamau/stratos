package main;

use gui;

fn main() {
    // === Store demo ===
    println("=== Store Demo ===");

    val store = Store();

    // Set some values
    store.set("count", 0);
    store.set("name", "Stratos");

    // Subscribe to changes on "count"
    val subId = store.subscribe("count", fn(value: any) void {
        println("  count changed to: ${value}");
    });

    // Read initial values
    println("Initial count: ${store.get("count")}");
    println("Initial name: ${store.get("name")}");

    // Update — triggers subscriber
    store.set("count", 1);
    store.set("count", 2);
    store.set("count", 3);

    // Unsubscribe and update — no notification
    store.unsubscribe(subId);
    store.set("count", 99);
    println("Final count (after unsub): ${store.get("count")}");

    println("");

    // === Context demo ===
    println("=== Context Demo ===");

    val themeCtx = Context("theme", "light");
    val userCtx  = Context("user", "anonymous");

    // Read defaults
    println("Default theme: ${themeCtx.consume()}");
    println("Default user: ${userCtx.consume()}");

    // Provide new values
    themeCtx.provide("dark");
    userCtx.provide("alice");

    // Consume updated values
    println("Updated theme: ${themeCtx.consume()}");
    println("Updated user: ${userCtx.consume()}");

    println("");
    println("Done!");
}
