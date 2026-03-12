package main;
use gui;

fn main() {
    val app = App("Responsive Layout", 800, 600);

    val infoText = Text("Resize the window to see the layout change");

    val breakpointText = Text("Current breakpoint: desktop");

    // Responsive widget that changes layout based on window size
    val responsiveContent = Responsive(
        // Mobile layout (< 600px): single column
        fn() int {
            setText(breakpointText, "Current breakpoint: mobile");
            return Column(8.0, [
                Card([Padding(12.0, [
                    Column(8.0, [
                        BoldText("Mobile View", 18.0),
                        Text("Single column layout"),
                        Text("Width: < 600px")
                    ])
                ])]),
                Card([Padding(12.0, [Text("Card A")])]),
                Card([Padding(12.0, [Text("Card B")])]),
                Card([Padding(12.0, [Text("Card C")])])
            ]);
        },
        // Tablet layout (600-1024px): two columns
        fn() int {
            setText(breakpointText, "Current breakpoint: tablet");
            return Column(12.0, [
                Card([Padding(12.0, [
                    Column(8.0, [
                        BoldText("Tablet View", 18.0),
                        Text("Two column layout"),
                        Text("Width: 600-1024px")
                    ])
                ])]),
                Row(12.0, [
                    Card([Padding(12.0, [
                        Column(8.0, [Text("Card A"), Text("Left column")])
                    ])]),
                    Card([Padding(12.0, [
                        Column(8.0, [Text("Card B"), Text("Right column")])
                    ])])
                ])
            ]);
        },
        // Desktop layout (> 1024px): three columns
        fn() int {
            setText(breakpointText, "Current breakpoint: desktop");
            return Column(12.0, [
                Card([Padding(12.0, [
                    Column(8.0, [
                        BoldText("Desktop View", 18.0),
                        Text("Three column layout"),
                        Text("Width: > 1024px")
                    ])
                ])]),
                Row(12.0, [
                    Card([Padding(12.0, [
                        Column(8.0, [Text("Card A"), Text("Column 1")])
                    ])]),
                    Card([Padding(12.0, [
                        Column(8.0, [Text("Card B"), Text("Column 2")])
                    ])]),
                    Card([Padding(12.0, [
                        Column(8.0, [Text("Card C"), Text("Column 3")])
                    ])])
                ])
            ]);
        }
    );

    val layout = Padding(24.0, [
        Column(16.0, [
            BoldText("Responsive Layout Demo", 24.0),
            Divider(),
            infoText,
            breakpointText,
            FixedSpacer(8.0),
            responsiveContent
        ])
    ]);

    app.root(layout);
    app.run();
}
