package main;

// Sorting algorithms example demonstrating Stratos without GC
// This project has garbage collection disabled in stratos.conf

// Helper function to print an array
fn printArray(label: string, arr: array<int>) {
    print("${label}: [");
    for i in 0..arr.length() {
        if i > 0 {
            print(", ");
        }
        print("${arr[i]}");
    }
    println("]");
}

// Copy array (since we want to preserve original)
fn copyArray(arr: array<int>): array<int> {
    var result: array<int> = [];
    for item in arr {
        result.push(item);
    }
    return result;
}

// Bubble Sort - O(n^2)
fn bubbleSort(arr: array<int>): array<int> {
    var result = copyArray(arr);
    val n = result.length();

    for i in 0..n {
        for j in 0..(n - i - 1) {
            if result[j] > result[j + 1] {
                // Swap
                val temp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = temp;
            }
        }
    }

    return result;
}

// Selection Sort - O(n^2)
fn selectionSort(arr: array<int>): array<int> {
    var result = copyArray(arr);
    val n = result.length();

    for i in 0..n {
        var minIdx = i;
        for j in (i + 1)..n {
            if result[j] < result[minIdx] {
                minIdx = j;
            }
        }
        if minIdx != i {
            val temp = result[i];
            result[i] = result[minIdx];
            result[minIdx] = temp;
        }
    }

    return result;
}

// Insertion Sort - O(n^2)
fn insertionSort(arr: array<int>): array<int> {
    var result = copyArray(arr);
    val n = result.length();

    for i in 1..n {
        val key = result[i];
        var j = i - 1;

        while j >= 0 && result[j] > key {
            result[j + 1] = result[j];
            j = j - 1;
        }
        result[j + 1] = key;
    }

    return result;
}

// Check if array is sorted
fn isSorted(arr: array<int>): bool {
    for i in 0..(arr.length() - 1) {
        if arr[i] > arr[i + 1] {
            return false;
        }
    }
    return true;
}

// Find minimum in array
fn findMin(arr: array<int>): int {
    var min = arr[0];
    for i in 1..arr.length() {
        if arr[i] < min {
            min = arr[i];
        }
    }
    return min;
}

// Find maximum in array
fn findMax(arr: array<int>): int {
    var max = arr[0];
    for i in 1..arr.length() {
        if arr[i] > max {
            max = arr[i];
        }
    }
    return max;
}

// Calculate sum of array
fn arraySum(arr: array<int>): int {
    var sum = 0;
    for item in arr {
        sum = sum + item;
    }
    return sum;
}

// Reverse array
fn reverseArray(arr: array<int>): array<int> {
    var result: array<int> = [];
    for i in 0..arr.length() {
        result.push(arr[arr.length() - 1 - i]);
    }
    return result;
}

// Binary search (requires sorted array)
fn binarySearch(arr: array<int>, target: int): int {
    var left = 0;
    var right = arr.length() - 1;

    while left <= right {
        val mid = left + (right - left) / 2;

        if arr[mid] == target {
            return mid;
        } else if arr[mid] < target {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1; // Not found
}

// Linear search
fn linearSearch(arr: array<int>, target: int): int {
    for i in 0..arr.length() {
        if arr[i] == target {
            return i;
        }
    }
    return -1;
}

fn main() {
    println("===========================================");
    println("   Sorting Algorithms Demo (No GC)");
    println("===========================================");
    println("");

    // Test data
    val testArray: array<int> = [64, 34, 25, 12, 22, 11, 90, 45, 33, 77];

    printArray("Original", testArray);
    println("");

    // Bubble Sort
    println("=== Bubble Sort ===");
    val bubbleSorted = bubbleSort(testArray);
    printArray("Result", bubbleSorted);
    println("Is sorted: ${isSorted(bubbleSorted)}");
    println("");

    // Selection Sort
    println("=== Selection Sort ===");
    val selectionSorted = selectionSort(testArray);
    printArray("Result", selectionSorted);
    println("Is sorted: ${isSorted(selectionSorted)}");
    println("");

    // Insertion Sort
    println("=== Insertion Sort ===");
    val insertionSorted = insertionSort(testArray);
    printArray("Result", insertionSorted);
    println("Is sorted: ${isSorted(insertionSorted)}");
    println("");

    // Array statistics
    println("=== Array Statistics ===");
    println("Minimum: ${findMin(testArray)}");
    println("Maximum: ${findMax(testArray)}");
    println("Sum: ${arraySum(testArray)}");
    println("");

    // Reverse
    println("=== Reverse Array ===");
    val reversed = reverseArray(testArray);
    printArray("Reversed", reversed);
    println("");

    // Search demonstrations
    println("=== Search Algorithms ===");
    val sortedArray = bubbleSorted;
    printArray("Sorted array", sortedArray);

    val searchTargets = [25, 90, 11, 50];
    for target in searchTargets {
        val linearIdx = linearSearch(sortedArray, target);
        val binaryIdx = binarySearch(sortedArray, target);

        if linearIdx >= 0 {
            println("Found ${target}: linear index=${linearIdx}, binary index=${binaryIdx}");
        } else {
            println("${target} not found in array");
        }
    }

    println("");

    // Test with different array sizes
    println("=== Performance Test (Small Arrays) ===");
    val sizes = [5, 10, 15];

    for size in sizes {
        var arr: array<int> = [];
        // Create descending array (worst case for bubble sort)
        for i in 0..size {
            arr.push(size - i);
        }

        printArray("Input (size ${size})", arr);
        val sorted = insertionSort(arr);
        printArray("Sorted", sorted);
        println("");
    }

    println("===========================================");
    println("   Demo Complete!");
    println("===========================================");
}
