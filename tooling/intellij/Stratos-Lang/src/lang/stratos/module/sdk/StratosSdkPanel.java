package lang.stratos.module.sdk;

import com.intellij.ide.util.PropertiesComponent;
import com.intellij.openapi.actionSystem.AnAction;
import com.intellij.openapi.actionSystem.AnActionEvent;
import com.intellij.openapi.application.AccessToken;
import com.intellij.openapi.application.ApplicationManager;
import com.intellij.openapi.fileChooser.*;
import com.intellij.openapi.projectRoots.Sdk;
import com.intellij.openapi.roots.ui.configuration.SdkComboBox;
import com.intellij.openapi.ui.TextComponentAccessor;
import com.intellij.openapi.vfs.VfsUtil;
import com.intellij.openapi.vfs.VirtualFile;
import com.intellij.ui.ComboboxWithBrowseButton;
import com.intellij.ui.components.labels.ActionLink;
import com.intellij.util.download.DownloadableFileDescription;
import com.intellij.util.download.DownloadableFileService;
import com.intellij.util.download.FileDownloader;
import com.intellij.util.io.ZipUtil;

import javax.swing.*;
import java.awt.*;
import java.io.IOException;
import java.util.Arrays;
import java.util.List;

/**
 * @author Arthur Kamau
 */
public class StratosSdkPanel extends JPanel {

  public static final String LAST_USED_REDLINE_HOME = "LAST_USED_REDLINE_HOME";
  private ActionLink myDownloadLink;
  private JPanel myRoot;
  private JLabel myHeading;
  private StratosSdkComboBox mySdkComboBox;
  private ComboboxWithBrowseButton myFlutterSdkPath;

  public StratosSdkPanel() {
    super(new BorderLayout());



//    myFlutterSdkPath.getComboBox().setEditable(true);
//    myFlutterSdkPath.getButton().addActionListener(
//            (e) -> myFlutterSdkPath.getComboBox().setSelectedItem(myFlutterSdkPath.getComboBox().getEditor().getItem()));
//
//    myFlutterSdkPath.addBrowseFolderListener("Browser sdk", null, null,
//            FileChooserDescriptorFactory.createSingleFolderDescriptor(),
//            TextComponentAccessor.STRING_COMBOBOX_WHOLE_TEXT);
//
//    myHeading.setText("Stratos SDK ");
//  }
//  private void createUIComponents() {
//    myDownloadLink = new ActionLink("Download and Install  Stratos", new AnAction() {
//      @Override
//      public void actionPerformed(AnActionEvent anActionEvent) {
//        FileChooserDescriptor descriptor = new FileChooserDescriptor(false, true, false, false, false, false);
//        PathChooserDialog pathChooser = FileChooserFactory.getInstance()
//            .createPathChooser(descriptor, null, StratosSdkPanel.this);
//        pathChooser.choose(VfsUtil.getUserHomeDir(), new FileChooser.FileChooserConsumer() {
//          @Override
//          public void cancelled() {
//          }
//
//          @Override
//          public void consume(List<VirtualFile> virtualFiles) {
//            if (virtualFiles.size() == 1) {
//              VirtualFile dir = virtualFiles.get(0);
//              String dirName = dir.getName();
//              AccessToken accessToken = ApplicationManager.getApplication().acquireWriteActionLock(StratosSdkPanel.class);
//              try {
//                if (!dirName.toLowerCase().contains("stratos") ) {// && !dirName.toLowerCase().contains("redline")) {
//                try {
//                  dir = dir.createChildDirectory(this, "src");
//                } catch (IOException e) {//
//                }
//              }
//              DownloadableFileService fileService = DownloadableFileService.getInstance();
//              DownloadableFileDescription fileDescription = fileService.createFileDescription("https://github.com/redline-smalltalk/redline-smalltalk.github.com/raw/master/assets/redline-deploy.zip", "redline-deploy.zip");
//              FileDownloader downloader = fileService.createDownloader(Arrays.asList(fileDescription), null, StratosSdkPanel.this, "redline-deploy.zip");
//              VirtualFile[] files = downloader.toDirectory(dir.getPath()).download();
//              if (files != null && files.length == 1) {
//                try {
//                  ZipUtil.extract(VfsUtil.virtualToIoFile(files[0]), VfsUtil.virtualToIoFile(dir), null);
//                  PropertiesComponent.getInstance().setValue(LAST_USED_REDLINE_HOME, dir.getPath());
//                } catch (IOException e) {
//                  e.printStackTrace();
//                } finally {
//                  dir.refresh(false, true);
//                }
//              }
//              } finally {
//                accessToken.finish();
//              }
//            }
//          }
//        });
//      }
//    });

//    <component id="9e532" class="com.intellij.ui.ComboboxWithBrowseButton" binding="myFlutterSdkPath">
//        <constraints>
//          <grid row="2" column="0" row-span="2" col-span="1" vsize-policy="0" hsize-policy="0" anchor="0" fill="1" indent="0" use-parent-layout="false"/>
//        </constraints>
//        <properties/>
//      </component>
//
//
//       <component id="15a3a" class="com.intellij.ui.components.labels.ActionLink" binding="myDownloadLink">
//        <constraints>
//          <grid row="5" column="0" row-span="1" col-span="1" vsize-policy="0" hsize-policy="0" anchor="0" fill="1" indent="0" use-parent-layout="false"/>
//        </constraints>
//        <properties>
//          <text value="Install SDK"/>
//        </properties>
//      </component>

    myRoot = new JPanel();
    add(myRoot, BorderLayout.CENTER);
  }

  public String getSdkName() {
    final Sdk selectedSdk = mySdkComboBox.getSelectedSdk();
    return selectedSdk == null ? null : selectedSdk.getName();
  }

  public Sdk getSdk() {
    return mySdkComboBox.getSelectedSdk();
  }

  public void setSdk(Sdk sdk) {
    mySdkComboBox.getComboBox().setSelectedItem(sdk);
  }
}
