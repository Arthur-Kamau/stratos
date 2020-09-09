package lang.stratos.action;

import com.intellij.ide.fileTemplates.FileTemplate;
import com.intellij.ide.fileTemplates.FileTemplateManager;
import com.intellij.openapi.util.Condition;
import com.intellij.util.SmartList;
import com.intellij.util.containers.ContainerUtil;
import lang.stratos.language.StratosFileType;

import java.util.List;

public class RsFileTemplateUtil {
    private final static String STRATOS_TEMPLATE_PREFIX = "Stratos";

    public static List<FileTemplate> getApplicableTemplates() {
        return getApplicableTemplates(new Condition<FileTemplate>() {
            @Override
            public boolean value(FileTemplate fileTemplate) {
                return StratosFileType.INSTANCE.getDefaultExtension().equals(fileTemplate.getExtension());
            }
        });
    }

    public static List<FileTemplate> getApplicableTemplates(Condition<FileTemplate> filter) {
        final List<FileTemplate> applicableTemplates = new SmartList<FileTemplate>();
        applicableTemplates.addAll(ContainerUtil.findAll(FileTemplateManager.getInstance().getInternalTemplates(), filter));
        applicableTemplates.addAll(ContainerUtil.findAll(FileTemplateManager.getInstance().getAllTemplates(), filter));
        return applicableTemplates;
    }

    public static String getTemplateShortName(String templateName) {
        if (templateName.startsWith(STRATOS_TEMPLATE_PREFIX)) {
            return templateName.substring(STRATOS_TEMPLATE_PREFIX.length());
        }
        return templateName;
    }
}
