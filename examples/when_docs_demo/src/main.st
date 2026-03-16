package main;

fn classifyNumber(num: int) string {
    if (num < 0) {
        return "Negative";
    } else if (num == 0) {
        return "Zero";
    } else if (num <= 10) {
        return "Small Positive";
    } else if (num <= 100) {
        return "Medium Positive";
    }
    return "Large Positive";
}

fn getAccess(user: string) string {
    when (user) {
        "admin" -> {
            return "full";
        }
        "editor" -> {
            return "write";
        }
        "viewer" -> {
            return "read";
        }
        else -> {
            return "none";
        }
    }
    return "none";
}

fn main() {
    // Test classification
    val numbers = [-5, 0, 7, 42, 150];

    for val num in numbers {
        val classification = classifyNumber(num);
        println(num + " is " + classification);
    }

    // User type access control
    println("\nAccess Control:");
    val users = ["admin", "editor", "viewer", "guest"];

    for val user in users {
        val access = getAccess(user);
        println(user + " has " + access + " access");
    }
}
