package main;
use gui;

fn main() {
    val app = App("Gesture Detection", 600, 500);

    val statusText = Text("Try gestures on the colored boxes below");
    setFontSize(statusText, 16.0);

    // Double-tap target
    val doubleTapBox = Box([]);
    setSize(doubleTapBox, 200.0, 120.0);
    setBackground(doubleTapBox, 100, 149, 237);
    setBorderRadius(doubleTapBox, 12.0);

    val doubleTapLabel = Text("Double-tap me!");
    setTextColor(doubleTapLabel, 255, 255, 255);

    val doubleTapResult = Text("Waiting...");

    val doubleTapDetector = GestureDetector(
        Column(8.0, [doubleTapBox, doubleTapLabel])
    );
    onDoubleTap(doubleTapDetector, fn() void {
        setText(doubleTapResult, "Double-tap detected!");
    });

    // Swipe target
    val swipeBox = Box([]);
    setSize(swipeBox, 200.0, 120.0);
    setBackground(swipeBox, 76, 175, 80);
    setBorderRadius(swipeBox, 12.0);

    val swipeLabel = Text("Swipe me!");
    setTextColor(swipeLabel, 255, 255, 255);

    val swipeResult = Text("Waiting...");

    val swipeDetector = GestureDetector(
        Column(8.0, [swipeBox, swipeLabel])
    );
    onSwipe(swipeDetector, fn(direction: string) void {
        setText(swipeResult, "Swiped: ${direction}");
    });

    // Drag target
    val dragBox = Box([]);
    setSize(dragBox, 200.0, 120.0);
    setBackground(dragBox, 255, 152, 0);
    setBorderRadius(dragBox, 12.0);

    val dragLabel = Text("Drag me!");
    setTextColor(dragLabel, 255, 255, 255);

    val dragResult = Text("Waiting...");

    val dragDetector = GestureDetector(
        Column(8.0, [dragBox, dragLabel])
    );
    onDragStart(dragDetector, fn(x: float, y: float) void {
        setText(dragResult, "Drag started");
    });
    onDrag(dragDetector, fn(x: float, y: float, dx: float, dy: float) void {
        setText(dragResult, "Dragging: dx=${dx}, dy=${dy}");
    });
    onDragEnd(dragDetector, fn() void {
        setText(dragResult, "Drag ended");
    });

    val layout = Padding(24.0, [
        Column(16.0, [
            BoldText("Gesture Detection Demo", 24.0),
            Divider(),
            statusText,
            Row(24.0, [
                Column(8.0, [doubleTapDetector, doubleTapResult]),
                Column(8.0, [swipeDetector, swipeResult]),
                Column(8.0, [dragDetector, dragResult])
            ])
        ])
    ]);

    app.root(layout);
    app.run();
}
