package lang.stratos.module.facet;

import com.intellij.facet.Facet;
import com.intellij.facet.FacetManager;
import com.intellij.facet.FacetType;
import com.intellij.openapi.module.Module;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

/**
 * @author Arthur Kamau
 */
public class StratosFacet extends Facet<StratosFacetConfiguration> {
  public StratosFacet(@NotNull final FacetType facetType,
                      @NotNull final Module module,
                      final String name,
                      @NotNull final StratosFacetConfiguration configuration,
                      Facet underlyingFacet) {
    super(facetType, module, name, configuration, underlyingFacet);
  }

  @Nullable
  public static StratosFacet getInstance(Module module) {
    return FacetManager.getInstance(module).getFacetByType(StratosFacetType.FACET_TYPE_ID);
  }
}

