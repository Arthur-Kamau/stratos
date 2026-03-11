package main;

use gui;

fn HomePage(router: any) int {
    val title = Text("Home Page");
    setFontSize(title, 24.0);

    val desc = Text("Welcome to the routing demo!");

    val aboutBtn = Button("Go to About");
    onClick(aboutBtn, fn() void {
        router.navigate("/about");
    });

    val contactBtn = Button("Go to Contact");
    onClick(contactBtn, fn() void {
        router.navigate("/contact");
    });

    val buttons = Row(8.0, [aboutBtn, contactBtn]);
    return Column(16.0, [title, desc, buttons]);
}

fn AboutPage(router: any) int {
    val title = Text("About Page");
    setFontSize(title, 24.0);

    val desc = Text("This is the about page.");

    val homeBtn = Button("Back to Home");
    onClick(homeBtn, fn() void {
        router.navigate("/");
    });

    return Column(16.0, [title, desc, homeBtn]);
}

fn ContactPage(router: any) int {
    val title = Text("Contact Page");
    setFontSize(title, 24.0);

    val desc = Text("Email: hello@stratos.dev");

    val backBtn = Button("Go Back");
    onClick(backBtn, fn() void {
        router.back();
    });

    return Column(16.0, [title, desc, backBtn]);
}

fn NotFoundPage(router: any) int {
    val title = Text("404 - Not Found");
    setFontSize(title, 24.0);

    val homeBtn = Button("Go Home");
    onClick(homeBtn, fn() void {
        router.navigate("/");
    });

    return Column(16.0, [title, homeBtn]);
}

fn main() {
    val app = App("Routing Demo", 800, 600);
    val router = Router();

    // Register routes
    router.route("/", fn() int => HomePage(router));
    router.route("/about", fn() int => AboutPage(router));
    router.route("/contact", fn() int => ContactPage(router));
    router.notFound(fn() int => NotFoundPage(router));

    // Navigate to home
    router.navigate("/");

    println("Router demo ready. Current path: ${router.currentPath()}");
    app.run();
}
