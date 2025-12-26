// 02_control_flow.st
// If, While, and Pattern Matching

val age = 20;

// If Expression (Ternary logic)
val status = if (age >= 18) "Adult" else "Minor";

// Standard If
if (age > 60) {
    print("Senior");
} else if (age > 18) {
    print("Adult");
} else {
    print("Child");
}

// While Loop
var i = 0;
while (i < 10) {
    print(i);
    i++;
}

// When Expression (Pattern Matching)
val userType = "admin";
when (userType) {
    "admin" -> { print("Full Access"); }
    "editor" -> { print("Edit Access"); }
    else -> { print("Read Only"); }
}
