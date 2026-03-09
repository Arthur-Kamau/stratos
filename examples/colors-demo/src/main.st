package main;

use colors;

fn main() {
    println("=== Stratos Colors Demo ===\n");

    // Basic color creation
    println("1. Creating colors:");
    val red = colors.rgb(255, 0, 0);
    val green = colors.hex("#00FF00");
    val blue = colors.hsl(240.0, 100.0, 50.0);

    println("   Red:   " + red.toString() + " -> " + red.toHex());
    println("   Green: " + green.toString() + " -> " + green.toHex());
    println("   Blue:  " + blue.toRGB().toString() + " -> " + blue.toRGB().toHex());

    // Color manipulation
    println("\n2. Color manipulation:");
    val lightRed = red.lighten(30.0);
    val darkRed = red.darken(30.0);
    println("   Original: " + red.toHex());
    println("   Lightened: " + lightRed.toHex());
    println("   Darkened: " + darkRed.toHex());

    val complement = red.complement();
    println("   Complement of red: " + complement.toHex());

    // Color mixing
    println("\n3. Color mixing:");
    val purple = red.mix(blue.toRGB(), 0.5);
    println("   Red + Blue (50%): " + purple.toHex());

    val orange = red.mix(colors.YELLOW, 0.5);
    println("   Red + Yellow (50%): " + orange.toHex());

    // Terminal colors
    println("\n4. Terminal colors:");
    println(colors.fg("   This text is red!", colors.RED));
    println(colors.fg("   This text is green!", colors.GREEN));
    println(colors.fg("   This text is blue!", colors.BLUE));
    println(colors.bg("   White on purple background!", colors.MD_PURPLE));
    println(colors.colorize("   Yellow on blue!", colors.YELLOW, colors.BLUE));

    // Styles
    println("\n5. Text styles:");
    println(colors.style("   Bold text", colors.BOLD));
    println(colors.style("   Italic text", colors.ITALIC));
    println(colors.style("   Underlined text", colors.UNDERLINE));

    // Predefined Material Design colors
    println("\n6. Material Design colors:");
    println(colors.fg("   Material Red", colors.MD_RED));
    println(colors.fg("   Material Blue", colors.MD_BLUE));
    println(colors.fg("   Material Green", colors.MD_GREEN));
    println(colors.fg("   Material Teal", colors.MD_TEAL));

    // Rainbow gradient
    println("\n7. Rainbow gradient:");
    val rainbow = colors.rainbow();
    var gradientLine = "   ";
    for (i in 0..40) {
        val color = rainbow.at(i / 40.0);
        gradientLine = gradientLine + colors.fg("█", color);
    }
    println(gradientLine);

    // Custom gradient
    println("\n8. Custom gradient (fire):");
    val fire = colors.gradient()
        .addStop(colors.hex("#FF0000"), 0.0)
        .addStop(colors.hex("#FF6600"), 0.3)
        .addStop(colors.hex("#FFCC00"), 0.6)
        .addStop(colors.hex("#FFFF00"), 1.0);

    var fireLine = "   ";
    for (i in 0..40) {
        val color = fire.at(i / 40.0);
        fireLine = fireLine + colors.fg("█", color);
    }
    println(fireLine);

    // HSL color wheel
    println("\n9. HSL color wheel:");
    var wheelLine = "   ";
    for (i in 0..36) {
        val hue = i * 10.0;
        val color = colors.hsl(hue, 100.0, 50.0).toRGB();
        wheelLine = wheelLine + colors.fg("█", color);
    }
    println(wheelLine);

    println("\n" + colors.fg("Demo complete!", colors.MD_GREEN) + colors.RESET);
}
