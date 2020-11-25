package org.stratos.lang.action;

import com.intellij.ide.fileTemplates.FileTemplate;
import com.intellij.ide.fileTemplates.FileTemplateManager;
import com.intellij.openapi.util.Condition;
import com.intellij.util.SmartList;
import com.intellij.util.containers.ContainerUtil;
import org.stratos.lang.StratosFileType;

import java.util.List;


public class StratosFileTemplateUtil {
  private final static String SMALLTALK_TEMPLATE_PREFIX = "Smalltalk ";

  public static List<FileTemplate> getApplicableTemplates() {
    return getApplicableTemplates(new Condition<FileTemplate>() {
      @Override
      public boolean value(FileTemplate fileTemplate) {
        System.out.print(", ext "+fileTemplate.getExtension());
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
    if (templateName.startsWith(SMALLTALK_TEMPLATE_PREFIX)) {
      return templateName.substring(SMALLTALK_TEMPLATE_PREFIX.length());
    }
    return templateName;
  }
}
