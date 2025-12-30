# Stratos Memory Management Guide

## Overview

Stratos uses **automatic memory management** through C++ reference counting (`std::shared_ptr`). Objects are automatically freed when they are no longer referenced.

## How It Works

### Reference Counting

Every object has a reference count that tracks how many references point to it:

```stratos
class Node {
    var value: int;
    var next: Node;
}

fn main() {
    val a = Node();  // Reference count = 1
    val b = a;        // Reference count = 2
    // When a and b go out of scope, count = 0, object is freed
}
```

### Automatic Cleanup

Objects are freed **immediately** when their reference count reaches zero:

- ✅ **Deterministic**: Objects are freed as soon as they're no longer needed
- ✅ **No GC pauses**: No stop-the-world garbage collection
- ✅ **Predictable**: Memory is freed in a predictable order

## Important Limitations

### ⚠️ Circular References Cause Memory Leaks

**Problem**: When two objects reference each other, they keep each other alive forever.

```stratos
class Node {
    var next: Node;
}

fn main() {
    val a = Node();
    val b = Node();

    a.next = b;  // a → b (b.refcount = 2)
    b.next = a;  // b → a (a.refcount = 2)

    // When main exits:
    // - a goes out of scope (a.refcount = 1, still alive!)
    // - b goes out of scope (b.refcount = 1, still alive!)
    // MEMORY LEAK: Both objects remain in memory forever
}
```

**Diagram**:
```
a (refcount 1) ──→ b (refcount 1)
       ↑              │
       └──────────────┘
```

### Solutions to Avoid Cycles

#### 1. Break the Cycle Before Scope Exit

```stratos
fn main() {
    val a = Node();
    val b = Node();

    a.next = b;
    b.next = a;

    // Break the cycle before exiting
    a.next = null;  // or b.next = null
}
```

#### 2. Use Parent-Child Relationships

For tree structures, only store downward references:

```stratos
class TreeNode {
    var value: int;
    var left: TreeNode;   // Parent → child only
    var right: TreeNode;
    // Don't store parent reference!
}
```

#### 3. Use Weak References (Future Feature)

Future Stratos versions may support weak references:

```stratos
class Node {
    var next: Node;      // Strong reference
    weak var prev: Node; // Weak reference (doesn't increment refcount)
}
```

## Data Structures and Memory Safety

### Safe Patterns

**✅ Trees** (with parent-child only):
```stratos
class TreeNode {
    var left: TreeNode;
    var right: TreeNode;
}
```

**✅ Arrays/Lists** (linear references):
```stratos
class LinkedList {
    var head: Node;
}

class Node {
    var value: int;
    var next: Node;  // Only forward references
}
```

**✅ Temporary References**:
```stratos
fn process(node: Node) {
    // Local variables are automatically cleaned up
    val temp = node;
}
```

### Unsafe Patterns

**❌ Doubly Linked Lists** (bidirectional references):
```stratos
class Node {
    var next: Node;
    var prev: Node;  // DANGER: Creates cycles!
}
```

**❌ Circular Buffers**:
```stratos
class CircularBuffer {
    var nodes: Array<Node>;
    var head: int;
    // If nodes reference each other, leak!
}
```

**❌ Graph Structures** (arbitrary connections):
```stratos
class GraphNode {
    var neighbors: Array<GraphNode>;  // DANGER: Cycles likely!
}
```

## Best Practices

### 1. Design for Acyclic Graphs

Whenever possible, design data structures as trees or DAGs (Directed Acyclic Graphs).

### 2. Manual Cleanup for Cycles

If you must use cyclic structures, clean them up manually:

```stratos
class Graph {
    var nodes: Array<Node>;

    fn cleanup() {
        // Break all cycles before destruction
        for node in nodes {
            node.neighbors = [];
        }
    }
}

fn main() {
    val graph = Graph();
    // ... use graph ...
    graph.cleanup();  // Clean up before exiting
}
```

### 3. Prefer Indices Over References

Instead of storing object references, store indices:

```stratos
class Node {
    var neighborIndices: Array<int>;  // Safe: no cycles!
}

class Graph {
    var nodes: Array<Node>;

    fn getNeighbor(nodeIdx: int, neighborIdx: int) -> Node {
        val neighborRealIdx = nodes[nodeIdx].neighborIndices[neighborIdx];
        return nodes[neighborRealIdx];
    }
}
```

### 4. Short-Lived Objects

Reference counting works best with short-lived objects:

```stratos
fn process() {
    val temp = Object();
    // Use temp...
    // Automatically freed when function returns
}
```

## Performance Characteristics

### Strengths ✅

- **No GC pauses**: No stop-the-world collection
- **Deterministic cleanup**: Objects freed immediately
- **Low latency**: Predictable performance
- **Cache friendly**: Objects freed in order of use

### Weaknesses ⚠️

- **Reference counting overhead**: Atomic operations on every assignment
- **Cannot handle cycles**: Requires programmer discipline
- **Larger object overhead**: Each object stores a reference count

## Comparison with Other Languages

| Feature | Stratos | Python | JavaScript | Java |
|---------|---------|--------|------------|------|
| GC Pauses | ✅ None | ⚠️ 10-100ms | ⚠️ 5-50ms | ⚠️ 10-1000ms |
| Cycle Handling | ❌ Leaks | ✅ Detects | ✅ Detects | ✅ Detects |
| Deterministic | ✅ Yes | ❌ No | ❌ No | ❌ No |
| Latency | ✅ Predictable | ⚠️ Spiky | ⚠️ Mostly good | ❌ Spiky |
| Peak Memory | ✅ Low | ⚠️ Medium | ⚠️ Medium | ❌ High |

## Debugging Memory Issues

### Signs of a Memory Leak

- Memory usage grows over time
- Program slows down after running for a while
- Objects that should be freed remain in memory

### How to Detect Cycles

1. **Review your data structures**: Look for bidirectional references
2. **Check class definitions**: Do any classes reference each other?
3. **Trace object lifetimes**: Should objects outlive their scope?

### Tools (Future)

Future versions of Stratos may include:
- Memory profiler
- Cycle detector
- Leak detection tools
- Weak reference support

## Future Improvements

The Stratos team is considering:

1. **Weak References**: Allow non-owning references that don't increment refcount
2. **Cycle Detection**: Periodic cycle detection and warnings
3. **Mark-and-Sweep GC**: Optional full garbage collector for cycle collection
4. **Memory Profiler**: Tools to detect leaks and cycles

## Summary

**Key Takeaways:**

- ✅ Stratos uses reference counting for automatic memory management
- ⚠️ Circular references cause memory leaks
- ✅ Design acyclic data structures whenever possible
- ✅ Manual cleanup for unavoidable cycles
- ✅ Great for low-latency, short-lived objects
- ⚠️ Requires programmer awareness of reference cycles

For questions or issues, please file a bug report or consult the Stratos community.
