package main;

use gui;

fn main() {
    val app = App("Form Demo", 800, 600);

    val title = BoldText("Registration Form", 24.0);

    val nameField = TextField("Full Name");
    val emailField = TextField("Email Address");
    val bio = TextArea("Tell us about yourself...");
    setSize(bio, 400.0, 100.0);

    val genderLabel = Text("Gender:");
    val male = RadioButton("Male", "gender");
    val female = RadioButton("Female", "gender");
    val other = RadioButton("Other", "gender");
    val genderRow = Row(16.0, [genderLabel, male, female, other]);

    val newsletter = Checkbox("Subscribe to newsletter");
    val terms = Checkbox("I agree to terms and conditions");

    val country = Dropdown("Select country");
    addDropdownItem(country, "United States");
    addDropdownItem(country, "United Kingdom");
    addDropdownItem(country, "Canada");
    addDropdownItem(country, "Germany");
    addDropdownItem(country, "Japan");

    val submitBtn = ColoredButton("Submit", 46, 125, 50);
    val resetBtn = Button("Reset");
    val buttons = Row(8.0, [submitBtn, resetBtn]);

    val form = Column(12.0, [title, nameField, emailField, bio, genderRow, newsletter, terms, country, buttons]);
    setPadding(form, 32.0);
    val root = Center([form]);

    app.root(root);
    app.run();
}
