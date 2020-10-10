
package org.stratos.sdk.action;
//import com.araizen.stratos.files.StratosFileIcons;
import com.intellij.ide.IdeBundle;
import com.intellij.ide.actions.CreateFileFromTemplateDialog;
import com.intellij.ide.actions.CreateFromTemplateAction;
import com.intellij.ide.fileTemplates.FileTemplate;
import com.intellij.ide.fileTemplates.FileTemplateManager;
import com.intellij.ide.fileTemplates.FileTemplateUtil;
import com.intellij.openapi.actionSystem.DataContext;
import com.intellij.openapi.actionSystem.LangDataKeys;
import com.intellij.openapi.module.Module;
import com.intellij.openapi.module.ModuleType;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.ui.InputValidatorEx;
import com.intellij.openapi.util.NlsContexts;
import com.intellij.openapi.util.text.StringUtil;
import com.intellij.psi.PsiDirectory;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.util.IncorrectOperationException;
//import lang.stratos.bundle.StratosBundle;
//import lang.stratos.icons.StratosIcons;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.sdk.bundle.StratosBundle;
import org.stratos.sdk.icon.StratosIcons;

import javax.swing.*;
import java.util.Properties;

public class NewStratosFileAction extends CreateFromTemplateAction {

    public NewStratosFileAction() {
        super(StratosBundle.messagePointer("stratos.file"), StratosBundle.messagePointer("new.stratos.file.title"), StratosIcons.FILE);
    }

    
//    @Nullable
//    @Override
//    protected PsiElement createFile(String name, String templateName, PsiDirectory dir) {
//        return null;
//    }
@Nullable
@Override
protected PsiFile createFile(String name, String templateName, PsiDirectory dir) {

    try {
        return createFile(name, dir, templateName).getContainingFile();
    }
    catch (Exception e) {
        System.out.println("\n\n Error  message" +e.getMessage() + " cause " +e.getCause() + " stack "+ e.getStackTrace());
        throw new IncorrectOperationException(e.getMessage());
    }
}

    @Override
    protected void buildDialog(@NotNull Project project, @NotNull PsiDirectory directory, CreateFileFromTemplateDialog.Builder builder) {
        builder.setTitle(StratosBundle.message("new.stratos.file.title"));
        builder.addKind(StratosBundle.message("stratos.file"), StratosIcons.FILE, "Stratos File");

        for (FileTemplate fileTemplate : RsFileTemplateUtil.getApplicableTemplates()) {
            final String templateName = fileTemplate.getName();
            final String shortName = RsFileTemplateUtil.getTemplateShortName(templateName);
            final Icon icon = StratosIcons.FILE;
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

    @Override
    protected @NlsContexts.Command String getActionName(PsiDirectory directory, @NotNull String newName, String templateName) {

        return StratosBundle.message("create.stratos.file.0", newName);
    }

    private static PsiElement createFile(String className, @NotNull PsiDirectory directory, final String templateName)
            throws Exception {
        final Properties props = new Properties(FileTemplateManager.getInstance().getDefaultProperties(directory.getProject()));
        props.setProperty(FileTemplate.ATTRIBUTE_NAME, className);

         FileTemplate template = FileTemplateManager.getInstance().getInternalTemplate(templateName);

        return FileTemplateUtil.createFromTemplate(template, className, props, directory, NewStratosFileAction.class.getClassLoader());
    }
}
