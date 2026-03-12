package main;
use gui;

fn main() {
    val app = App("Transitions", 700, 600);

    // --- Fade Transition ---
    val fadeContent = Card([
        Padding(16.0, [
            Column(8.0, [
                BoldText("Fade Content", 16.0),
                Text("This card fades in and out.")
            ])
        ])
    ]);
    val fade = FadeTransition(500.0, [fadeContent]);

    val fadeInBtn = Button("Fade In");
    setBackground(fadeInBtn, 76, 175, 80);
    onClick(fadeInBtn, fn() void {
        fadeIn(fade);
    });

    val fadeOutBtn = Button("Fade Out");
    setBackground(fadeOutBtn, 211, 47, 47);
    onClick(fadeOutBtn, fn() void {
        fadeOut(fade);
    });

    // --- Slide Transition ---
    val slideContent = Card([
        Padding(16.0, [
            Column(8.0, [
                BoldText("Slide Content", 16.0),
                Text("This card slides left/right.")
            ])
        ])
    ]);
    val slide = SlideTransition("left", 400.0, [slideContent]);

    val slideInBtn = Button("Slide In");
    setBackground(slideInBtn, 33, 150, 243);
    onClick(slideInBtn, fn() void {
        slideIn(slide);
    });

    val slideOutBtn = Button("Slide Out");
    setBackground(slideOutBtn, 255, 152, 0);
    onClick(slideOutBtn, fn() void {
        slideOut(slide);
    });

    // --- Scale Transition ---
    val scaleContent = Card([
        Padding(16.0, [
            Column(8.0, [
                BoldText("Scale Content", 16.0),
                Text("This card scales in and out.")
            ])
        ])
    ]);
    val scale = ScaleTransition(400.0, [scaleContent]);

    val scaleInBtn = Button("Scale In");
    setBackground(scaleInBtn, 156, 39, 176);
    onClick(scaleInBtn, fn() void {
        scaleIn(scale);
    });

    val scaleOutBtn = Button("Scale Out");
    setBackground(scaleOutBtn, 121, 85, 72);
    onClick(scaleOutBtn, fn() void {
        scaleOut(scale);
    });

    // --- Animated Container ---
    val animContainer = AnimatedContainer(600.0, [
        Padding(12.0, [
            Text("Animated Container")
        ])
    ]);
    setSize(animContainer, 200.0, 80.0);
    setBackground(animContainer, 100, 149, 237);
    setBorderRadius(animContainer, 8.0);

    val growBtn = Button("Grow");
    onClick(growBtn, fn() void {
        animateWidth(animContainer, 400.0);
        animateHeight(animContainer, 120.0);
    });

    val shrinkBtn = Button("Shrink");
    onClick(shrinkBtn, fn() void {
        animateWidth(animContainer, 150.0);
        animateHeight(animContainer, 60.0);
    });

    val layout = Padding(24.0, [
        ScrollView([
            Column(16.0, [
                BoldText("Transition Animations Demo", 24.0),
                Divider(),
                BoldText("Fade Transition", 16.0),
                fade,
                Row(8.0, [fadeInBtn, fadeOutBtn]),
                Divider(),
                BoldText("Slide Transition", 16.0),
                slide,
                Row(8.0, [slideInBtn, slideOutBtn]),
                Divider(),
                BoldText("Scale Transition", 16.0),
                scale,
                Row(8.0, [scaleInBtn, scaleOutBtn]),
                Divider(),
                BoldText("Animated Container", 16.0),
                animContainer,
                Row(8.0, [growBtn, shrinkBtn])
            ])
        ])
    ]);

    app.root(layout);
    app.run();
}
