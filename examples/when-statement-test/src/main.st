// Comprehensive When Statement Pattern Tests
// Tests all possible when statement combinations

package main;

use log;

// Test enum for Result pattern matching
enum Result<T, E> {
    Ok(T),
    Err(E)
}

// Test enum for Option pattern matching
enum Option<T> {
    Some(T),
    None
}

// Test class for pattern matching
class Point {
    var x: int;
    var y: int;

    constructor(x: int, y: int) {
        this.x = x;
        this.y = y;
    }
}

fn main() {
    log.info("=== When Statement Pattern Tests ===\n");

    // Test 1: Basic value matching
    testBasicValueMatching();

    // Test 2: Multiple values matching
    testMultipleValueMatching();

    // Test 3: Range matching
    testRangeMatching();

    // Test 4: Guards/conditions
    testGuardMatching();

    // Test 5: Result pattern matching
    testResultPatternMatching();

    // Test 6: Option pattern matching
    testOptionPatternMatching();

    // Test 7: When as expression
    testWhenAsExpression();

    // Test 8: When without subject (condition chains)
    testWhenWithoutSubject();

    // Test 9: Boolean matching
    testBooleanMatching();

    // Test 10: Nested when statements
    testNestedWhen();

    log.info("\n=== All When Pattern Tests Completed ===");
}

// Test 1: Basic value matching
fn testBasicValueMatching() {
    log.info("Test 1: Basic Value Matching");

    val userType = "admin";

    when (userType) {
        "admin" -> {
            log.info("  ✓ Matched 'admin'");
        }
        "editor" -> {
            log.info("  Matched 'editor'");
        }
        "viewer" -> {
            log.info("  Matched 'viewer'");
        }
        else -> {
            log.info("  Matched else");
        }
    }
}

// Test 2: Multiple values matching
fn testMultipleValueMatching() {
    log.info("\nTest 2: Multiple Value Matching");

    val day = "Monday";

    when (day) {
        "Monday", "Tuesday", "Wednesday", "Thursday", "Friday" -> {
            log.info("  ✓ Matched weekday: " + day);
        }
        "Saturday", "Sunday" -> {
            log.info("  Matched weekend");
        }
        else -> {
            log.info("  Invalid day");
        }
    }
}

// Test 3: Range matching
fn testRangeMatching() {
    log.info("\nTest 3: Range Matching");

    val score = 85;

    val grade = when (score) {
        90..100 -> "A"
        80..89 -> "B"
        70..79 -> "C"
        60..69 -> "D"
        else -> "F"
    };

    log.info("  ✓ Score " + score + " = Grade " + grade);
}

// Test 4: Guards/conditions
fn testGuardMatching() {
    log.info("\nTest 4: Guard Matching");

    val age = 25;
    val hasLicense = true;

    when (age) {
        _ if age >= 18 and hasLicense -> {
            log.info("  ✓ Can drive (age: " + age + ", has license)");
        }
        _ if age >= 18 -> {
            log.info("  Can vote but not drive");
        }
        else -> {
            log.info("  Minor");
        }
    }
}

// Test 5: Result pattern matching
fn testResultPatternMatching() {
    log.info("\nTest 5: Result Pattern Matching");

    val successResult = Result.Ok(42);
    val failureResult = Result.Err("Something went wrong");

    when (successResult) {
        Ok(value) -> {
            log.info("  ✓ Success: " + value);
        }
        Err(error) -> {
            log.info("  Error: " + error);
        }
    }

    when (failureResult) {
        Ok(value) -> {
            log.info("  Success: " + value);
        }
        Err(error) -> {
            log.info("  ✓ Error: " + error);
        }
    }
}

// Test 6: Option pattern matching
fn testOptionPatternMatching() {
    log.info("\nTest 6: Option Pattern Matching");

    val someValue = Option.Some(100);
    val noneValue = Option.None;

    when (someValue) {
        Some(value) -> {
            log.info("  ✓ Some value: " + value);
        }
        None -> {
            log.info("  None");
        }
    }

    when (noneValue) {
        Some(value) -> {
            log.info("  Some value: " + value);
        }
        None -> {
            log.info("  ✓ None value");
        }
    }
}

// Test 7: When as expression
fn testWhenAsExpression() {
    log.info("\nTest 7: When as Expression");

    val age = 20;

    val status = when (age) {
        _ if age < 13 -> "Child"
        _ if age < 18 -> "Teenager"
        _ if age < 65 -> "Adult"
        else -> "Senior"
    };

    log.info("  ✓ Age " + age + " -> Status: " + status);
}

// Test 8: When without subject (condition chains)
fn testWhenWithoutSubject() {
    log.info("\nTest 8: When Without Subject");

    val temperature = 25;
    val humidity = 60;

    val weather = when {
        temperature > 30 and humidity > 70 -> "Hot and humid"
        temperature > 30 -> "Hot and dry"
        temperature < 10 -> "Cold"
        temperature >= 20 and temperature <= 30 -> "Pleasant"
        else -> "Moderate"
    };

    log.info("  ✓ Temp: " + temperature + "°C, Humidity: " + humidity + "% -> " + weather);
}

// Test 9: Boolean matching
fn testBooleanMatching() {
    log.info("\nTest 9: Boolean Matching");

    val isLoggedIn = true;

    when (isLoggedIn) {
        true -> {
            log.info("  ✓ User is logged in");
        }
        false -> {
            log.info("  User is not logged in");
        }
    }
}

// Test 10: Nested when statements
fn testNestedWhen() {
    log.info("\nTest 10: Nested When Statements");

    val userRole = "admin";
    val userActive = true;

    when (userRole) {
        "admin" -> {
            when (userActive) {
                true -> {
                    log.info("  ✓ Active admin user");
                }
                false -> {
                    log.info("  Inactive admin user");
                }
            }
        }
        "user" -> {
            when (userActive) {
                true -> {
                    log.info("  Active regular user");
                }
                false -> {
                    log.info("  Inactive regular user");
                }
            }
        }
        else -> {
            log.info("  Unknown role");
        }
    }
}
