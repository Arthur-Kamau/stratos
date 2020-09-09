package lang.stratos.module;

//public class  extends ModuleType<StratosModuleBuilder> {
//    private static final StratosModuleType INSTANCE = new StratosModuleType();
//
//    public StratosModuleType() {
//        super("REDLINE_MODULE");
//    }
//}


import com.intellij.openapi.module.ModuleType;
import lang.stratos.icons.StratosIcons;
//import st.redline.smalltalk.SmalltalkIcons;

import javax.swing.*;

/**
 * @author Konstantin Bulenkov
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
        return "Redline Smalltalk";
    }

    @Override
    public String getDescription() {
        return "Add support for Redline Smalltalk";
    }

//    @Override
//    public Icon getBigIcon() {
//        return null;
//    }

    @Override
    public Icon getNodeIcon(@Deprecated boolean isOpened) {
        return StratosIcons.StratosIcon; //SmalltalkIcons.Redline;
    }

    public static StratosModuleType getInstance() {
        return INSTANCE;
    }
}

