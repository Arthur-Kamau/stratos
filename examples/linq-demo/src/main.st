package main;

use linq;

// Sample data class
class Person {
    pub var name: string;
    pub var age: int;
    pub var city: string;

    constructor(name: string, age: int, city: string) {
        this.name = name;
        this.age = age;
        this.city = city;
    }
}

fn main() {
    println("=== Stratos LINQ Demo ===\n");

    // Basic number operations
    println("1. Basic filtering and transformation:");
    val numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

    val evenSquares = linq.from(numbers)
        .where((n) => n % 2 == 0)
        .select((n) => n * n)
        .toArray();

    print("   Even squares: ");
    for (n in evenSquares) {
        print(n.toString() + " ");
    }
    println("");

    // Take and skip
    println("\n2. Take and Skip:");
    val firstFive = linq.from(numbers).take(5).toArray();
    val afterThree = linq.from(numbers).skip(3).toArray();

    print("   First 5: ");
    for (n in firstFive) { print(n.toString() + " "); }
    println("");

    print("   After 3: ");
    for (n in afterThree) { print(n.toString() + " "); }
    println("");

    // Aggregation
    println("\n3. Numeric aggregations:");
    val nums = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0];
    val numQuery = linq.numbers(nums);

    println("   Sum: " + numQuery.sum().toString());
    println("   Average: " + numQuery.average().toString());
    println("   Min: " + numQuery.min().toString());
    println("   Max: " + numQuery.max().toString());

    // Working with objects
    println("\n4. Object queries:");
    val people = [
        Person("Alice", 30, "New York"),
        Person("Bob", 25, "Los Angeles"),
        Person("Charlie", 35, "New York"),
        Person("Diana", 28, "Chicago"),
        Person("Eve", 32, "New York")
    ];

    // Filter by city
    val newYorkers = linq.from(people)
        .where((p) => p.city == "New York")
        .toArray();

    println("   People in New York:");
    for (p in newYorkers) {
        println("     - " + p.name + " (age " + p.age.toString() + ")");
    }

    // Order by age
    println("\n5. Ordering:");
    val byAge = linq.from(people)
        .orderBy((p) => p.age)
        .toArray();

    println("   People by age (ascending):");
    for (p in byAge) {
        println("     - " + p.name + ": " + p.age.toString());
    }

    // Grouping
    println("\n6. Grouping by city:");
    val byCity = linq.from(people).groupBy((p) => p.city);

    for (group in byCity) {
        println("   " + group.key + ":");
        for (p in group.items) {
            println("     - " + p.name);
        }
    }

    // Any and All
    println("\n7. Any and All predicates:");
    val hasAdults = linq.from(people).any((p) => p.age >= 30);
    val allAdults = linq.from(people).all((p) => p.age >= 18);

    println("   Has people 30+: " + hasAdults.toString());
    println("   All adults (18+): " + allAdults.toString());

    // Count
    println("\n8. Counting:");
    val totalCount = linq.from(people).count();
    val over30Count = linq.from(people).countWhere((p) => p.age > 30);

    println("   Total people: " + totalCount.toString());
    println("   Over 30: " + over30Count.toString());

    // Range generation
    println("\n9. Range generation:");
    val range = linq.range(1, 5)
        .select((n) => n * n)
        .toArray();

    print("   Squares 1-5: ");
    for (n in range) { print(n.toString() + " "); }
    println("");

    // Chained operations
    println("\n10. Complex chained query:");
    val result = linq.from(people)
        .where((p) => p.age >= 28)
        .orderByDescending((p) => p.age)
        .take(3)
        .select((p) => p.name + " (" + p.age.toString() + ")")
        .toArray();

    println("   Top 3 oldest (28+):");
    for (s in result) {
        println("     - " + s);
    }

    println("\nDemo complete!");
}
