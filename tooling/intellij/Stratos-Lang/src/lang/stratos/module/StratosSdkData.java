package lang.stratos.module;

import com.intellij.openapi.components.PersistentStateComponent;
import com.intellij.openapi.projectRoots.SdkAdditionalData;
import com.intellij.util.xmlb.XmlSerializerUtil;

/**
 * @author Konstantin Bulenkov
 */
@SuppressWarnings("UnusedDeclaration")
public class StratosSdkData implements SdkAdditionalData, PersistentStateComponent<StratosSdkData> {
  private String homePath = "";
  private String version = "";

  public StratosSdkData() {
  }

  public StratosSdkData(String homePath, String version) {
    this.homePath = homePath;
    this.version = version;
  }

  public String getHomePath() {
    return homePath;
  }

  public String getVersion() {
    return version;
  }

  @SuppressWarnings({"CloneDoesntCallSuperClone"})
  @Override
  public Object clone() throws CloneNotSupportedException {
    return super.clone();
  }

  public StratosSdkData getState() {
    return this;
  }

  public void loadState(StratosSdkData state) {
    XmlSerializerUtil.copyBean(state, this);
  }
}
