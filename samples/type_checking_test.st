package main;

use math;

fn main() {
    // This should work - correct types
    val result1 = math.sqrt(16.0);
    print(result1);

    // This should work - two double arguments
    val result2 = math.pow(2.0, 3.0);
    print(result2);

    // This should FAIL - wrong type
    // val result3 = math.sqrt(16);

    // This should FAIL - wrong number of arguments
    // val result4 = math.sqrt(16.0, 2.0);

    // This should FAIL - first arg wrong type
    val result5 = math.pow(2, 3.0);
}
