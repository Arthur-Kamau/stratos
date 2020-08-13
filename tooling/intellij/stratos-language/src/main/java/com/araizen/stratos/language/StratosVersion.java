package com.araizen.stratos.language;

import com.intellij.util.lang.JavaVersion;
import org.jetbrains.annotations.NotNull;

public final class StratosVersion {



    /**
     * The major version.
     * Corresponds to the first number of 9+ format (<b>9</b>.0.1) / the second number of 1.x format (1.<b>8</b>.0_60).
     */
    public final int major;

    /**
     * The minor version.
     * Corresponds to the second number of 9+ format (9.<b>0</b>.1) / the third number of 1.x format (1.8.<b>0</b>_60).
     * Was used in version strings prior to 1.5, in newer strings is always {@code 0}.
     */
    public final int minor;


    /**
     * The build number.
     * Corresponds to a number prefixed by the "plus" sign in 9+ format (9.0.1+<b>7</b>) / by "-b" string in 1.x format (1.8.0_60-b<b>12</b>).
     */
    public final int build;

    /**
     * {@code true} if the platform is an early access release, {@code false} otherwise (or when not known).
     */
    public final boolean ea;

    StratosVersion(int major, int minor, int build, boolean ea) {
        this.major = major;
        this.minor = minor;
        this.build = build;
        this.ea = ea;
    }

    public static @NotNull StratosVersion compose(int feature) {
        return compose(feature, 0, 0, false);
    }

    public static @NotNull StratosVersion compose(int major, int minor, int build, boolean ea) throws IllegalArgumentException {
        if (major < 0) throw new IllegalArgumentException();
        if (minor < 0) throw new IllegalArgumentException();
        if (build < 0) throw new IllegalArgumentException();
        return new StratosVersion(major, minor, build, ea);
    }



}
