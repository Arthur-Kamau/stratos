package org.stratos.lang.sdk;

import com.google.common.base.Charsets;
import com.intellij.openapi.projectRoots.*;
import com.intellij.openapi.vfs.LocalFileSystem;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.util.xmlb.XmlSerializer;
import org.jdom.Element;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.StratosIcons;

import javax.swing.*;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Properties;

/**
 * @author Arthur Kamau
 */
public class StratosSdkType extends SdkType {
    public StratosSdkType() {
        super("stratos");
    }

    public static SdkTypeId getInstance() {
        return SdkType.findInstance(StratosSdkType.class);
    }

    @Nullable
    @Override
    public String suggestHomePath() {
        return System.getenv().get("STRATOS_HOME");
    }

    @Override
    public boolean isValidSdkHome(String s) {
        VirtualFile home = LocalFileSystem.getInstance().findFileByIoFile(new File(s));
        if (home != null && home.exists() && home.isDirectory()) {
            VirtualFile lib = home.findChild("lib");
            VirtualFile rt = home.findChild("src");
            if (lib != null && lib.isDirectory() && rt != null && rt.isDirectory()) {
                return true;
            }
        }

        return false;
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
        return StratosIcons.SAMPLE_ICON;
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

    @Nullable
    @Override
    public String getVersionString(Sdk sdk) {
//    return "Read .conf file";
        String path = sdk.getHomePath();
        if (path == null) return null;

        File file = new File(path);
        VirtualFile home = LocalFileSystem.getInstance().findFileByIoFile(file);
        if (home != null) {
            VirtualFile conf = home.findChild("stratos.conf");
            if (conf.exists() && !conf.isDirectory()) {
                try {
                    InputStream stream = conf.getInputStream();
                    final Properties properties = new Properties();
                    properties.load(new InputStreamReader(stream, Charsets.UTF_8));

                    final String value = properties.getProperty("version");
                    if (value == null) {
                        return null;
                    }

                    return value;

                } catch (Exception e) {

                }
            } else {
                return null;
            }
        }
        return null;
    }
}
