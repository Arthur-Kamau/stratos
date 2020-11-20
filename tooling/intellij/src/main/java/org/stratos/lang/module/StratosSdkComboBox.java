package org.stratos.lang.module;

import com.intellij.openapi.fileChooser.FileChooser;
import com.intellij.openapi.fileChooser.FileChooserDescriptor;
import com.intellij.openapi.fileChooser.FileChooserDescriptorFactory;
import com.intellij.openapi.project.Project;
import com.intellij.openapi.project.ProjectManager;
import com.intellij.openapi.projectRoots.ProjectJdkTable;
import com.intellij.openapi.projectRoots.Sdk;
import com.intellij.openapi.vfs.VfsUtil;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.ui.ColoredListCellRenderer;
import com.intellij.ui.ComboboxWithBrowseButton;
import com.intellij.ui.SimpleTextAttributes;
import org.jetbrains.annotations.NotNull;
import org.stratos.lang.action.InstallSdkAction;
import org.stratos.lang.sdk.StratosSdkType;

import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.List;

/**
 * @author Konstantin Bulenkov
 */
public class StratosSdkComboBox extends ComboboxWithBrowseButton { // implements InstallSdkAction.Model {

  public StratosSdkComboBox() {
    getComboBox().setRenderer(new ColoredListCellRenderer() {
      @Override
      protected void customizeCellRenderer(JList list, Object value, int index, boolean selected, boolean hasFocus) {
        append("SDK ONE");
//        if (value instanceof Sdk) {
//          append(((Sdk) value).getName());
//        } else {
//          append("Select Stratos SDK", SimpleTextAttributes.ERROR_ATTRIBUTES);
//        }
      }
    });
    addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
//        Sdk selectedSdk = getSelectedSdk();
//        final Project project = ProjectManager.getInstance().getDefaultProject();
        System.out.println("=>>>>>>> TODO <<<<<<<<<< =======");

        // Defaults to ~/flutter
        final FileChooserDescriptor descriptor =
                new FileChooserDescriptor(FileChooserDescriptorFactory.createSingleFolderDescriptor()) {
                  @Override
                  public void validateSelectedFiles(@NotNull VirtualFile[] files) {
                    for (VirtualFile file : files) {
                      // Eliminate some false positives, which occurs when an existing directory is deleted.
                      VfsUtil.markDirtyAndRefresh(false, true, true, file);
                      if (file.findChild("stratos") != null) {
                        throw new IllegalArgumentException("A file called 'stratos' already exists in this location.");
                      }
                    }
                  }
                }.withTitle("Flutter SDK Directory").withDescription("Choose a directory to install the Flutter SDK");

        final VirtualFile installTarget = FileChooser.chooseFile(descriptor, null, null);
        if (installTarget != null) {
//          FlutterInitializer.sendAnalyticsAction(ANALYTICS_KEY);
          installTo(installTarget);
        }
        else {
          // A valid SDK may have been deleted before the FileChooser was cancelled.
//          validatePeer();
        }
//        ProjectJdksEditor editor = new ProjectJdksEditor(selectedSdk, project, StratosSdkComboBox.this);
//        editor.show();
//        if (editor.isOK()) {
//          selectedSdk = editor.getSelectedJdk();
//          updateSdkList(selectedSdk, false);
//        }
      }
    });
    updateSdkList(null, true);
  }

  private void installTo(final @NotNull VirtualFile directory) {
    final String installPath = directory.getPath();
    final String sdkDir = new File(installPath, "Stratos").getPath();


  }

  public void updateSdkList(Sdk sdkToSelect, boolean selectAnySdk) {
    final List<Sdk> sdkList = ProjectJdkTable.getInstance().getSdksOfType(StratosSdkType.getInstance());
    if (selectAnySdk && sdkList.size() > 0) {
      sdkToSelect = sdkList.get(0);
    }
    sdkList.add(0, null);
    getComboBox().setModel(new DefaultComboBoxModel(sdkList.toArray(new Sdk[sdkList.size()])));
    getComboBox().setSelectedItem(sdkToSelect);
  }

  public Sdk getSelectedSdk() {
    return (Sdk) getComboBox().getSelectedItem();
  }
}
