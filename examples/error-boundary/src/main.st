package main;
use gui;

fn main() {
    val app = App("Error Boundary", 600, 500);

    // Create normal content
    val content = Column(12.0, [
        Text("This is normal content inside an error boundary."),
        Text("Click the button below to simulate an error.")
    ]);

    // Create fallback widget for when error occurs
    val fallback = Padding(16.0, [
        Column(12.0, [
            BoldText("Something went wrong!", 18.0),
            Text("The component encountered an error."),
            Text("Click 'Reset' to try again.")
        ])
    ]);
    setBackground(fallback, 255, 235, 238);
    setBorderRadius(fallback, 8.0);

    // Create error boundary with fallback
    val boundary = ErrorBoundaryWithFallback(fallback, [content]);

    val errorStatusText = Text("Status: OK");

    // Set error callback
    onError(boundary, fn(message: string) void {
        setText(errorStatusText, "Error caught: ${message}");
    });

    // Button to trigger error
    val triggerBtn = Button("Trigger Error");
    setBackground(triggerBtn, 211, 47, 47);
    onClick(triggerBtn, fn() void {
        triggerError(boundary, "Simulated rendering failure");
    });

    // Button to reset
    val resetBtn = Button("Reset");
    setBackground(resetBtn, 76, 175, 80);
    onClick(resetBtn, fn() void {
        resetError(boundary);
        setText(errorStatusText, "Status: OK (reset)");
    });

    // Error boundary without custom fallback (uses default error UI)
    val boundary2 = ErrorBoundary([
        Text("This boundary uses the default error display.")
    ]);

    val triggerBtn2 = Button("Trigger Default Error");
    onClick(triggerBtn2, fn() void {
        triggerError(boundary2, "Widget failed to render: null reference");
    });

    val resetBtn2 = Button("Reset Default");
    onClick(resetBtn2, fn() void {
        resetError(boundary2);
    });

    val layout = Padding(24.0, [
        Column(16.0, [
            BoldText("Error Boundary Demo", 24.0),
            Divider(),
            BoldText("Custom Fallback:", 16.0),
            boundary,
            Row(12.0, [triggerBtn, resetBtn]),
            errorStatusText,
            Divider(),
            BoldText("Default Error Display:", 16.0),
            boundary2,
            Row(12.0, [triggerBtn2, resetBtn2])
        ])
    ]);

    app.root(layout);
    app.run();
}
