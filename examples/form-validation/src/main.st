package main;
use gui;

fn main() {
    val app = App("Form Validation", 600, 500);

    // Create input fields
    val nameInput = TextField("Enter your name");
    val emailInput = TextField("Enter your email");

    // Wrap in form fields with labels
    val nameField = FormField("Name", nameInput);
    addRequired(nameField, "Name is required");
    addMinLength(nameField, 2, "Name must be at least 2 characters");

    val emailField = FormField("Email", emailInput);
    addRequired(emailField, "Email is required");
    addEmailValidation(emailField, "Please enter a valid email");

    // Create form
    val form = FormWithNames(["name", "email"], [nameField, emailField]);

    val statusText = Text("Fill in the form and click Submit");

    val submitBtn = Button("Submit");
    onClick(submitBtn, fn() void {
        val valid = validateForm(form);
        if valid {
            setText(statusText, "Form submitted successfully!");
        } else {
            setText(statusText, "Please fix the errors above.");
        }
    });

    val resetBtn = Button("Reset");
    onClick(resetBtn, fn() void {
        resetForm(form);
        setText(statusText, "Form reset.");
    });

    val layout = Padding(24.0, [
        Column(16.0, [
            BoldText("Registration Form", 24.0),
            Divider(),
            form,
            Row(12.0, [submitBtn, resetBtn]),
            statusText
        ])
    ]);

    app.root(layout);
    app.run();
}
