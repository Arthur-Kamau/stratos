package lang.stratos.framework;

import com.intellij.framework.FrameworkTypeEx;
import com.intellij.framework.addSupport.FrameworkSupportInModuleProvider;
import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;

/**
 * @author Arthur_Kamau
 */
public class StratosFrameworkType extends FrameworkTypeEx {
  public static final String ID = "Stratos ";

  protected StratosFrameworkType() {
    super(ID);
  }

  @NotNull
  @Override
  public FrameworkSupportInModuleProvider createProvider() {
    return new StratosModuleProvider();
  }

  @NotNull
  @Override
  public String getPresentableName() {
    return "Stratos Module";
  }

  @NotNull
  @Override
  public Icon getIcon() {
    return StratosIcons.StratosIcon;
  }
}
