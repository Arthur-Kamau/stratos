package main;
use gui;

fn main() {
    val app = App("Async Widgets", 600, 500);

    // --- FutureBuilder example ---

    // Loading widget
    val loadingWidget = Center([
        Column(8.0, [
            CircularProgress(),
            Text("Loading data...")
        ])
    ]);

    // Success widget
    val successWidget = Card([
        Padding(16.0, [
            Column(8.0, [
                BoldText("Data Loaded!", 18.0),
                Text("User: Arthur Kamau"),
                Text("Email: arthur@example.com"),
                Text("Role: Developer")
            ])
        ])
    ]);

    // Error widget
    val errorWidget = Padding(12.0, [
        Column(8.0, [
            BoldText("Failed to load data", 16.0),
            Text("Network error: connection timed out")
        ])
    ]);
    setBackground(errorWidget, 255, 235, 238);
    setBorderRadius(errorWidget, 8.0);

    val futureBuilder = FutureBuilder(loadingWidget, successWidget, errorWidget);

    // Buttons to simulate async state changes
    val loadBtn = Button("Start Loading");
    onClick(loadBtn, fn() void {
        setAsyncState(futureBuilder, "loading");
    });

    val successBtn = Button("Simulate Success");
    setBackground(successBtn, 76, 175, 80);
    onClick(successBtn, fn() void {
        setAsyncState(futureBuilder, "success");
    });

    val errorBtn = Button("Simulate Error");
    setBackground(errorBtn, 211, 47, 47);
    onClick(errorBtn, fn() void {
        setAsyncState(futureBuilder, "error");
    });

    // --- Suspense example ---

    val suspenseFallback = Center([
        Column(8.0, [
            CircularProgress(),
            Text("Loading component...")
        ])
    ]);

    val suspenseContent = Card([
        Padding(16.0, [
            Column(8.0, [
                BoldText("Component Loaded", 18.0),
                Text("This content was behind a Suspense boundary."),
                Text("It appeared once the data was ready.")
            ])
        ])
    ]);

    val suspense = Suspense(suspenseFallback, [suspenseContent]);

    val readyBtn = Button("Mark Ready");
    setBackground(readyBtn, 33, 150, 243);
    onClick(readyBtn, fn() void {
        setSuspenseReady(suspense, true);
    });

    val unreadyBtn = Button("Reset to Loading");
    onClick(unreadyBtn, fn() void {
        setSuspenseReady(suspense, false);
    });

    val layout = Padding(24.0, [
        ScrollView([
            Column(16.0, [
                BoldText("Async Widget Demo", 24.0),
                Divider(),
                BoldText("FutureBuilder", 18.0),
                Text("Shows loading/success/error based on async state:"),
                futureBuilder,
                Row(8.0, [loadBtn, successBtn, errorBtn]),
                Divider(),
                BoldText("Suspense", 18.0),
                Text("Shows fallback until content is ready:"),
                suspense,
                Row(8.0, [readyBtn, unreadyBtn])
            ])
        ])
    ]);

    app.root(layout);
    app.run();
}
