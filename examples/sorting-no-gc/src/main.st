package main;

// Sorting algorithms example demonstrating Stratos without GC
// Note: Array mutation via index assignment is limited,
// so this demo shows search and statistics algorithms instead.

fn printArray(label: string, arr: array<int>) {
    print("${label}: [");
    for i in 0..arr.length() {
        if (i > 0) {
            print(", ");
        }
        print("${arr[i]}");
    }
    println("]");
}

// Find minimum in array
fn findMin(arr: array<int>): int {
    var min = arr[0];
    for i in 1..arr.length() {
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}

// Find maximum in array
fn findMax(arr: array<int>): int {
    var max = arr[0];
    for i in 1..arr.length() {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

// Calculate sum
fn arraySum(arr: array<int>): int {
    var sum = 0;
    for item in arr {
        sum = sum + item;
    }
    return sum;
}

// Check if array contains value
fn contains(arr: array<int>, target: int): bool {
    for item in arr {
        if (item == target) {
            return true;
        }
    }
    return false;
}

// Linear search
fn linearSearch(arr: array<int>, target: int): int {
    for i in 0..arr.length() {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

// Count occurrences
fn countOccurrences(arr: array<int>, target: int): int {
    var count = 0;
    for item in arr {
        if (item == target) {
            count = count + 1;
        }
    }
    return count;
}

// Check if sorted
fn isSorted(arr: array<int>): bool {
    for i in 0..(arr.length() - 1) {
        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}

fn main() {
    println("===========================================");
    println("   Array Algorithms Demo (No GC)");
    println("===========================================");
    println("");

    val data: array<int> = [64, 34, 25, 12, 22, 11, 90, 45, 33, 77];
    printArray("Data", data);
    println("");

    // Statistics
    println("=== Array Statistics ===");
    println("Length: ${data.length()}");
    println("Minimum: ${findMin(data)}");
    println("Maximum: ${findMax(data)}");
    println("Sum: ${arraySum(data)}");
    println("Is sorted: ${isSorted(data)}");
    println("");

    // Already sorted array
    val sorted: array<int> = [11, 12, 22, 25, 33, 34, 45, 64, 77, 90];
    printArray("Sorted", sorted);
    println("Is sorted: ${isSorted(sorted)}");
    println("");

    // Search
    println("=== Search Operations ===");
    val targets = [25, 90, 11, 50];
    for target in targets {
        val idx = linearSearch(data, target);
        if (idx >= 0) {
            println("Found ${target} at index ${idx}");
        } else {
            println("${target} not found");
        }
    }
    println("");

    // Count occurrences
    println("=== Count Operations ===");
    val withDups: array<int> = [1, 2, 3, 2, 4, 2, 5, 3, 2];
    printArray("Array with duplicates", withDups);
    println("Count of 2: ${countOccurrences(withDups, 2)}");
    println("Count of 3: ${countOccurrences(withDups, 3)}");
    println("Count of 9: ${countOccurrences(withDups, 9)}");
    println("Contains 4: ${contains(withDups, 4)}");
    println("Contains 9: ${contains(withDups, 9)}");

    println("");
    println("===========================================");
    println("   Demo Complete!");
    println("===========================================");
}
