package com.araizen.stratos.language;


public enum LanguageLevel {
    CP_0_1(new StratosVersion(0, 1, 0, false));

    private final StratosVersion myVersion;

    LanguageLevel(StratosVersion v) {
        this.myVersion = v;
    }

    public boolean isPreview() {
        return myVersion.ea;
    }
    public static final LanguageLevel HIGHEST = CP_0_1;
    public boolean isAtLeast(int major, int minor) {
        if (major >= myVersion.major && minor >= myVersion.minor) {
            return true;
        } else {
            return false;
        }
    }

    public boolean isAtLeast(LanguageLevel cp01) {
        return isAtLeast(cp01.myVersion.major , cp01.myVersion.minor);
    }
}
