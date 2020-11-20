package org.stratos.lang.module;



import com.intellij.openapi.module.ModuleType;
import org.stratos.lang.StratosIcons;

import javax.swing.*;

/**
 * @author Arthur Kamau
 */
public class StratosModuleType extends ModuleType<StratosModuleBuilder> {
    private static final StratosModuleType INSTANCE = new StratosModuleType();

    public StratosModuleType() {
        super("STRATOS_MODULE");
    }

    @Override
    public StratosModuleBuilder createModuleBuilder() {
        return new StratosModuleBuilder();
    }



    @Override
    public String getName() {
        return "Stratos";
    }

    @Override
    public String getDescription() {
        return "Add support for Stratos ";
    }

    @Override
    public Icon getNodeIcon(@Deprecated boolean isOpened) {
        return StratosIcons.SAMPLE_ICON;
    }

    public static StratosModuleType getInstance() {
        return INSTANCE;
    }
}

