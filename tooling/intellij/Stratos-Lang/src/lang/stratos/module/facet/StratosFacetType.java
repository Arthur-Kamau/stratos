package lang.stratos.module.facet;

import com.intellij.facet.Facet;
import com.intellij.facet.FacetType;
import com.intellij.facet.FacetTypeId;
import com.intellij.openapi.module.JavaModuleType;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.ModuleType;
import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;

/**
 * @author Arthur Kamau
 */
public class StratosFacetType extends FacetType<StratosFacet, StratosFacetConfiguration> {
  public final static FacetTypeId<StratosFacet> FACET_TYPE_ID = new FacetTypeId<StratosFacet>("RedlineSmalltalk");

  public StratosFacetType() {
    super(FACET_TYPE_ID, StratosModuleSettings.FACET_ID, StratosModuleSettings.FACET_NAME);
  }

  @Override
  public StratosFacetConfiguration createDefaultConfiguration() {
    return new StratosFacetConfiguration();
  }

  @Override
  public StratosFacet createFacet(@NotNull final Module module,
                             final String name,
                             @NotNull final StratosFacetConfiguration configuration,
                             @Nullable final Facet underlyingFacet) {
    return new StratosFacet(this, module, name, configuration, underlyingFacet);
  }

  @Override
  public boolean isSuitableModuleType(final ModuleType moduleType) {
    return moduleType instanceof JavaModuleType;
  }

  @Override
  public Icon getIcon() {
    return StratosIcons.StratosIcon;
  }

  public static StratosFacetType getInstance() {
    return findInstance(StratosFacetType.class);
  }

}
