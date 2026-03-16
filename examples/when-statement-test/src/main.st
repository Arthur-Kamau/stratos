// When Statement Pattern Tests

package main;

use log;

// Test 1: Basic value matching
fn testBasicValueMatching() {
    log.info("Test 1: Basic Value Matching");

    val userType = "admin";

    when (userType) {
        "admin" -> {
            log.info("  Matched 'admin'");
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

// Test 2: Integer matching
fn testIntegerMatching() {
    log.info("\nTest 2: Integer Matching");

    val code = 200;

    when (code) {
        200 -> {
            log.info("  OK (200)");
        }
        404 -> {
            log.info("  Not Found (404)");
        }
        500 -> {
            log.info("  Server Error (500)");
        }
        else -> {
            log.info("  Other code: " + code);
        }
    }
}

// Test 3: Boolean matching
fn testBooleanMatching() {
    log.info("\nTest 3: Boolean Matching");

    val isLoggedIn = true;

    when (isLoggedIn) {
        true -> {
            log.info("  User is logged in");
        }
        false -> {
            log.info("  User is not logged in");
        }
    }
}

// Test 4: Nested when statements
fn testNestedWhen() {
    log.info("\nTest 4: Nested When Statements");

    val userRole = "admin";
    val userActive = true;

    when (userRole) {
        "admin" -> {
            when (userActive) {
                true -> {
                    log.info("  Active admin user");
                }
                false -> {
                    log.info("  Inactive admin user");
                }
            }
        }
        "user" -> {
            log.info("  Regular user");
        }
        else -> {
            log.info("  Unknown role");
        }
    }
}

// Test 5: When as statement (no return value)
fn testWhenStatement() {
    log.info("\nTest 5: When Statement");

    val animal = "cat";

    when (animal) {
        "dog" -> {
            log.info("  Woof!");
        }
        "cat" -> {
            log.info("  Meow!");
        }
        "bird" -> {
            log.info("  Tweet!");
        }
        else -> {
            log.info("  Unknown animal");
        }
    }
}

fn main() {
    log.info("=== When Statement Pattern Tests ===\n");

    testBasicValueMatching();
    testIntegerMatching();
    testBooleanMatching();
    testNestedWhen();
    testWhenStatement();

    log.info("\n=== All When Pattern Tests Completed ===");
}
