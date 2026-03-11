package main;

use gui;

fn UserListPage(router: any) int {
    val title = Text("Users");
    setFontSize(title, 24.0);

    val btn1 = Button("User: Alice (id=1)");
    onClick(btn1, fn() void { router.navigate("/user/1"); });

    val btn2 = Button("User: Bob (id=2)");
    onClick(btn2, fn() void { router.navigate("/user/2"); });

    val btn3 = Button("Search: q=stratos");
    onClick(btn3, fn() void { router.navigate("/search?q=stratos"); });

    return Column(12.0, [title, btn1, btn2, btn3]);
}

fn UserPage(router: any) int {
    val userId = router.param("id");
    val title = Text("User Profile");
    setFontSize(title, 24.0);

    val info = Text("User ID: ${userId}");

    val backBtn = Button("Back to Users");
    onClick(backBtn, fn() void { router.navigate("/users"); });

    return Column(12.0, [title, info, backBtn]);
}

fn SearchPage(router: any) int {
    val query = router.query("q");
    val title = Text("Search Results");
    setFontSize(title, 24.0);

    val info = Text("Query: ${query}");

    val backBtn = Button("Back");
    onClick(backBtn, fn() void { router.back(); });

    return Column(12.0, [title, info, backBtn]);
}

fn main() {
    val app = App("Route Params Demo", 800, 600);
    val router = Router();

    router.route("/users", fn() int => UserListPage(router));
    router.route("/user/:id", fn() int => UserPage(router));
    router.route("/search", fn() int => SearchPage(router));

    router.navigate("/users");
    println("Route params demo ready.");
    app.run();
}
