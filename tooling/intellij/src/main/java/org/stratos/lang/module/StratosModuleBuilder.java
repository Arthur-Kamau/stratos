package org.stratos.lang.module;

import com.intellij.ide.util.projectWizard.ModuleBuilder;
import com.intellij.ide.util.projectWizard.ModuleWizardStep;
import com.intellij.ide.util.projectWizard.WizardContext;
import com.intellij.openapi.Disposable;
import com.intellij.openapi.roots.ModifiableRootModel;
import com.intellij.openapi.util.Disposer;
import com.intellij.openapi.vfs.LocalFileSystem;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.utils.StratosSdk;

import javax.swing.*;

public class StratosModuleBuilder extends ModuleBuilder {

    private StratosModuleWizardStep myStep;

    @Override
    public void setupRootModel(@NotNull ModifiableRootModel model) {
    }

    @Override
    public StratosModuleType getModuleType() {
        return StratosModuleType.getInstance();
    }

    @Nullable
    @Override
    public ModuleWizardStep getCustomOptionsStep(WizardContext context, Disposable parentDisposable) {
//        return new StratosModuleWizardStep();
        myStep = new StratosModuleWizardStep(context);
        Disposer.register(parentDisposable, myStep);
        return myStep;
    }


    public class StratosModuleWizardStep extends ModuleWizardStep implements Disposable {
        private final StratosGeneratorPeer myPeer;

        public StratosModuleWizardStep(@NotNull WizardContext context) {
            //TODO(pq): find a way to listen to wizard cancelation and propagate to peer.
            myPeer = new StratosGeneratorPeer(context);
        }

        @Override
        public JComponent getComponent() {
            return myPeer.getComponent();
        }

        @Override
        public void updateDataModel() {
        }

        @Override
        public boolean validate() {
            final boolean valid = myPeer.validate();
            if (valid) {
                myPeer.apply();
            }
            return valid;
        }

        @Override
        public void dispose() {
        }

        @Nullable
        StratosSdk getFlutterSdk() {
            final String sdkPath = myPeer.getSdkComboPath();

            // Ensure the local filesystem has caught up to external processes (e.g., git clone).
            if (!sdkPath.isEmpty()) {
                try {
                    LocalFileSystem
                            .getInstance().refreshAndFindFileByPath(sdkPath);
                }
                catch (Throwable e) {
                    // It's possible that the refresh will fail in which case we just want to trap and ignore.
                }
            }
            return StratosSdk.forPath(sdkPath);
        }
    }

}
