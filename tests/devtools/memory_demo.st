package main;

use log;

// Test Memory profiling with GC
class Node {
    var value: int;
    var next: Optional<Node>;

    fn init(v: int) {
        val this.value = v;
        val this.next = None;
    }
}

fn main() {
    log.info("Memory profiling demo starting");
    log.info("Creating objects to trigger GC...");

    // Create many objects to trigger garbage collection
    val count = 0;
    while (count < 200) {
        val node = Node(count);

        // Create a circular reference that GC will break
        if (count % 10 == 0) {
            val node2 = Node(count + 1);
            val node.next = node2;
            // This creates a cycle: node -> node2 and both go out of scope
            log.debug("Created circular reference");
        }

        val count = count + 1;
    }

    log.info("Object creation complete");
    log.info("Check the Memory tab in DevTools to see:");
    log.info("- Object count");
    log.info("- Memory usage");
    log.info("- GC collection events");
    log.info("- Cycles broken by GC");
}
