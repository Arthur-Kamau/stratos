// Collections module - Enhanced data structures
// Inspired by Kotlin's collections and Go's container package

package collections;

// ============================================================================
// Types
// ============================================================================

class List<T> {
    var items: Array<T>;
    var size: int;

    pub fn add(item: T) void;
    pub fn get(index: int) T;
    pub fn set(index: int, item: T) void;
    pub fn remove(index: int) T;
    pub fn clear() void;
    pub fn isEmpty() bool;
    pub fn length() int;

    // Higher-order functions
    pub fn map(fn: Function<T, R>) List<R>;
    pub fn filter(predicate: Function<T, bool>) List<T>;
    pub fn reduce(initial: R, fn: Function<R, T, R>) R;
    pub fn forEach(fn: Function<T, void>) void;
    pub fn find(predicate: Function<T, bool>) T?;
    pub fn any(predicate: Function<T, bool>) bool;
    pub fn all(predicate: Function<T, bool>) bool;
    pub fn sort(comparator: Function<T, T, int>) List<T>;
}

class Map<K, V> {
    var keys: Array<K>;
    var values: Array<V>;
    var size: int;

    pub fn put(key: K, value: V) void;
    pub fn get(key: K) V?;
    pub fn remove(key: K) V?;
    pub fn containsKey(key: K) bool;
    pub fn clear() void;
    pub fn isEmpty() bool;
    pub fn length() int;
    pub fn keys() List<K>;
    pub fn values() List<V>;
    pub fn entries() List<Pair<K, V>>;
}

class Set<T> {
    var items: Array<T>;
    var size: int;

    pub fn add(item: T) bool;
    pub fn remove(item: T) bool;
    pub fn contains(item: T) bool;
    pub fn clear() void;
    pub fn isEmpty() bool;
    pub fn length() int;
    pub fn union(other: Set<T>) Set<T>;
    pub fn intersection(other: Set<T>) Set<T>;
    pub fn difference(other: Set<T>) Set<T>;
}

class Queue<T> {
    var items: Array<T>;
    var front: int;
    var rear: int;
    var size: int;

    pub fn enqueue(item: T) void;
    pub fn dequeue() T?;
    pub fn peek() T?;
    pub fn isEmpty() bool;
    pub fn length() int;
}

class Stack<T> {
    var items: Array<T>;
    var top: int;

    pub fn push(item: T) void;
    pub fn pop() T?;
    pub fn peek() T?;
    pub fn isEmpty() bool;
    pub fn length() int;
}

class Pair<K, V> {
    var first: K;
    var second: V;

    constructor(k: K, v: V) {
        this.first = k;
        this.second = v;
    }
}

// ============================================================================
// Native function declarations
// ============================================================================

// List creation
fn newList<T>() List<T>;
fn listOf<T>(items: Array<T>) List<T>;

// Map creation
fn newMap<K, V>() Map<K, V>;
fn mapOf<K, V>(entries: Array<Pair<K, V>>) Map<K, V>;

// Set creation
fn newSet<T>() Set<T>;
fn setOf<T>(items: Array<T>) Set<T>;

// Queue creation
fn newQueue<T>() Queue<T>;

// Stack creation
fn newStack<T>() Stack<T>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Range generation
fn range(start: int, end: int) List<int> {
    val result = newList<int>();
    for i in start..end {
        result.add(i);
    }
    return result;
}

// Generate list with function
fn generate<T>(count: int, fn: Function<int, T>) List<T> {
    val result = newList<T>();
    for i in 0..count {
        result.add(fn(i));
    }
    return result;
}

// Flatten nested lists
fn flatten<T>(lists: List<List<T>>) List<T> {
    val result = newList<T>();
    lists.forEach((list) => {
        list.forEach((item) => {
            result.add(item);
        });
    });
    return result;
}

// Zip two lists
fn zip<A, B>(listA: List<A>, listB: List<B>) List<Pair<A, B>> {
    val result = newList<Pair<A, B>>();
    val minLen = math.min(listA.length(), listB.length());

    for i in 0..minLen {
        result.add(Pair(listA.get(i), listB.get(i)));
    }

    return result;
}

// Group by key
fn groupBy<T, K>(list: List<T>, keyFn: Function<T, K>) Map<K, List<T>> {
    val result = newMap<K, List<T>>();

    list.forEach((item) => {
        val key = keyFn(item);
        if !result.containsKey(key) {
            result.put(key, newList<T>());
        }
        result.get(key).add(item);
    });

    return result;
}

// Partition by predicate
fn partition<T>(list: List<T>, predicate: Function<T, bool>) Pair<List<T>, List<T>> {
    val trueList = newList<T>();
    val falseList = newList<T>();

    list.forEach((item) => {
        if predicate(item) {
            trueList.add(item);
        } else {
            falseList.add(item);
        }
    });

    return Pair(trueList, falseList);
}

// Take first n elements
fn take<T>(list: List<T>, n: int) List<T> {
    val result = newList<T>();
    val count = math.min(n, list.length());

    for i in 0..count {
        result.add(list.get(i));
    }

    return result;
}

// Drop first n elements
fn drop<T>(list: List<T>, n: int) List<T> {
    val result = newList<T>();

    for i in n..list.length() {
        result.add(list.get(i));
    }

    return result;
}

// Chunk list into smaller lists
fn chunk<T>(list: List<T>, size: int) List<List<T>> {
    val result = newList<List<T>>();
    var current = newList<T>();

    list.forEach((item) => {
        current.add(item);
        if current.length() == size {
            result.add(current);
            current = newList<T>();
        }
    });

    if !current.isEmpty() {
        result.add(current);
    }

    return result;
}

// Distinct elements
fn distinct<T>(list: List<T>) List<T> {
    val seen = newSet<T>();
    val result = newList<T>();

    list.forEach((item) => {
        if seen.add(item) {
            result.add(item);
        }
    });

    return result;
}

// Count occurrences
fn countBy<T>(list: List<T>, predicate: Function<T, bool>) int {
    var count = 0;
    list.forEach((item) => {
        if predicate(item) {
            count = count + 1;
        }
    });
    return count;
}
