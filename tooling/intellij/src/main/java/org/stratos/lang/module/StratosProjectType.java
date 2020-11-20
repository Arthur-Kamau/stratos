package org.stratos.lang.module;


public enum StratosProjectType {
    APP("flutter.module.create.settings.type.application", "app"),
    PLUGIN("flutter.module.create.settings.type.plugin" , "plugin"),
    PACKAGE("flutter.module.create.settings.type.package" , "package"),
    MODULE("flutter.module.create.settings.type.module" , "module"),
    IMPORT("flutter.module.create.settings.type.import_module" , "module");

    final public String title;
    final public String arg;

    StratosProjectType(String title, String arg) {
        this.title = title;
        this.arg = arg;
    }

    public String toString() {
        return title;
    }
}
