package org.stratos.lang.action;

import com.intellij.ide.IdeBundle;
import com.intellij.ide.actions.CreateFileFromTemplateDialog;
import com.intellij.ide.actions.CreateFromTemplateAction;
import com.intellij.ide.fileTemplates.FileTemplate;
import com.intellij.ide.fileTemplates.FileTemplateManager;
import com.intellij.ide.fileTemplates.FileTemplateUtil;
import com.intellij.openapi.actionSystem.DataContext;
import com.intellij.openapi.actionSystem.LangDataKeys;
//import com.intellij.openapi.module.JavaModuleType;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.ModuleType;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.InputValidatorEx;
import com.intellij.openapi.util.text.StringUtil;
import com.intellij.psi.PsiDirectory;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import org.stratos.lang.StratosIcons;
//import st.redline.smalltalk.SmalltalkIcons;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.Properties;

public class NewFileAction extends CreateFromTemplateAction<PsiFile> {
  public NewFileAction() {
    super("Stratos File", null, StratosIcons.SAMPLE_ICON);
  }

  @Override
  protected boolean isAvailable(DataContext dataContext) {
    final Module module = LangDataKeys.MODULE.getData(dataContext);
    final ModuleType moduleType = module == null ? null : ModuleType.get(module);
//    final boolean isJavaModule = moduleType instanceof JavaModuleType;
//    return super.isAvailable(dataContext) && isJavaModule;
    return super.isAvailable(dataContext) ;
  }

  @Override
  protected String getActionName(PsiDirectory directory, String newName, String templateName) {
    return "Creating File " + newName;
  }

  @Override
  protected void buildDialog(Project project, PsiDirectory directory, CreateFileFromTemplateDialog.Builder builder) {
    builder.setTitle("Stratos File");
    for (FileTemplate fileTemplate : StratosFileTemplateUtil.getApplicableTemplates()) {
      final String templateName = fileTemplate.getName();
      final String shortName = StratosFileTemplateUtil.getTemplateShortName(templateName);
      final Icon icon = StratosIcons.SAMPLE_ICON;  //SmalltalkIcons.Smalltalk;
      builder.addKind(shortName, icon, templateName);
    }
    builder.setValidator(new InputValidatorEx() {
      @Override
      public String getErrorText(String inputString) {
        if (inputString.length() > 0 && !StringUtil.isJavaIdentifier(inputString)) {
          return "This is not a valid Stratos name";
        }
        return null;
      }

      @Override
      public boolean checkInput(String inputString) {
        return true;
      }

      @Override
      public boolean canClose(String inputString) {
        return !StringUtil.isEmptyOrSpaces(inputString) && getErrorText(inputString) == null;
      }
    });
  }

  @Nullable
  @Override
  protected PsiFile createFile(String className, String templateName, PsiDirectory dir) {
    try {
      System.out.println("====>>> createFile  ");
      return createFile(className, dir, templateName).getContainingFile();
    }
    catch (Exception e) {
      System.out.println("\n\n FIx ME  cause "+e.getCause() +" trace"+e.getStackTrace() );
      //  throw new Exception(e);//IncorrectOperationException(e.getMessage(), e);
    }
    return null;
  }

  private static PsiElement createFile(String className, @NotNull PsiDirectory directory, final String templateName)
      throws Exception {
    final Properties props = new Properties(FileTemplateManager.getInstance().getDefaultProperties(directory.getProject()));
    props.setProperty(FileTemplate.ATTRIBUTE_NAME, className);

    System.out.println("%%%%%%%%   fixes  &&&&&&&&");
    final FileTemplate template = FileTemplateManager.getInstance().getInternalTemplate(templateName);
    System.out.println("%%%%%%%%   fixes  finally &&&&&&&&");
    return FileTemplateUtil.createFromTemplate(template, className, props, directory, NewFileAction.class.getClassLoader());
  }
}
