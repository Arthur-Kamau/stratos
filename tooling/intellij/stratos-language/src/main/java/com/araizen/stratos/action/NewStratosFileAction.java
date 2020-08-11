package com.araizen.stratos.action;

import com.araizen.stratos.files.StratosFileIcons;
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
import com.intellij.openapi.util.text.StringUtil;
import com.intellij.psi.PsiDirectory;
import com.intellij.psi.PsiElement;
import com.intellij.psi.PsiFile;
import com.intellij.util.IncorrectOperationException;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.Properties;
public class NewStratosFileAction extends CreateFromTemplateAction<PsiFile> {


    public NewStratosFileAction(){
        super("Stratos File", null , StratosFileIcons.STRATOSICON);
    }

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
    protected void buildDialog(@NotNull Project project, @NotNull PsiDirectory directory, CreateFileFromTemplateDialog.@NotNull Builder builder) {

//        builder.setTitle(IdeBundle.message("action.create.new.class"));
        builder.setTitle("Create Stratos File");
        for (FileTemplate fileTemplate : RsFileTemplateUtil.getApplicableTemplates()) {
            final String templateName = fileTemplate.getName();
            final String shortName = RsFileTemplateUtil.getTemplateShortName(templateName);
            final Icon icon = StratosFileIcons.STRATOSICON;
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
    protected  String getActionName(PsiDirectory directory, @NotNull String newName, String templateName) {
        return "Creating File "+newName;
    }


    private static PsiElement createFile(String className, @NotNull PsiDirectory directory, final String templateName)
            throws Exception {
        final Properties props = new Properties(FileTemplateManager.getInstance().getDefaultProperties(directory.getProject()));
        props.setProperty(FileTemplate.ATTRIBUTE_NAME, className);

        final FileTemplate template = FileTemplateManager.getInstance().getInternalTemplate(templateName);

        return FileTemplateUtil.createFromTemplate(template, className, props, directory, NewStratosFileAction.class.getClassLoader());
    }

}
