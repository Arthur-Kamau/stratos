package lang.stratos.module.sdk;

import com.intellij.openapi.projectRoots.*;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.util.xmlb.XmlSerializer;
import lang.stratos.icons.StratosIcons;
import org.jdom.Element;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.io.File;

/**
 * @author Arthur Kamau
 */
public class StratosSdkType extends SdkType {
  public StratosSdkType() {
    super("stratos");
  }

  @Nullable
  @Override
  public String suggestHomePath() {
    return System.getenv().get("STRATOS_HOME");
  }

  @Override
  public boolean isValidSdkHome(String s) {
//    VirtualFile home = LocalFileSystem.getInstance().findFileByIoFile(new File(s));
//    if (home != null && home.exists() && home.isDirectory()) {
//      VirtualFile lib = home.findChild("lib");
//      VirtualFile rt = home.findChild("rt");
//      if (lib != null && lib.isDirectory() && rt != null && rt.isDirectory()) {
//        return true;
//      }
//    }
//
//    return false;
    return true;
  }

  @Override
  public String suggestSdkName(String s, String s2) {
    return "Stratos SDK";
  }

  @Nullable
  @Override
  public AdditionalDataConfigurable createAdditionalDataConfigurable(SdkModel sdkModel, SdkModificator sdkModificator) {
    return null;
  }

  @Override
  public String getPresentableName() {
    return "Stratos Compiler SDK";
  }

  @Override
  public SdkAdditionalData loadAdditionalData(Element additional) {
    return XmlSerializer.deserialize(additional, StratosSdkData.class);
  }

  @Override
  public Icon getIcon() {
    return StratosIcons.StratosIcon;
  }

  @Override
  public Icon getIconForAddAction() {
    return getIcon();
  }

  @Override
  public void saveAdditionalData(SdkAdditionalData additionalData, Element additional) {
    if (additionalData instanceof StratosSdkData) {
      XmlSerializer.serializeInto(additionalData, additional);
    }
  }

  public static SdkTypeId getInstance() {
    return SdkType.findInstance(StratosSdkType.class);
  }

  @Nullable
  @Override
  public String getVersionString(Sdk sdk) {
    return "Read .conf file";
//    String path = sdk.getHomePath();
//    if (path == null) return null;
//
//    File file = new File(path);
//    VirtualFile home = LocalFileSystem.getInstance().findFileByIoFile(file);
//    if (home != null) {
//      VirtualFile lib = home.findChild("lib");
//      if (lib != null) {
//        for (VirtualFile jar : lib.getChildren()) {
//          String name = jar.getName();
//          if (name.startsWith("redline-") && "jar".equalsIgnoreCase(jar.getExtension())) {
//            name = name.substring(8);
//            name = name.replace("-SNAPSHOT", "");
//            name = name.replace(".jar", "");
//            return name;
//          }
//        }
//      }
//    }
//    return null;
  }
}
