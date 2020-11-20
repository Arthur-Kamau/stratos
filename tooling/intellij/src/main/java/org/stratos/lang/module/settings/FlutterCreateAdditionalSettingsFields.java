/*
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
package org.stratos.lang.module.settings;

import com.intellij.ide.util.projectWizard.SettingsStep;
import com.intellij.ui.DocumentAdapter;
import com.intellij.util.PlatformUtils;
import com.intellij.util.ui.UIUtil;

import java.awt.event.ItemEvent;
import java.util.function.Supplier;
import javax.swing.JTextField;
import javax.swing.event.DocumentEvent;
import org.jetbrains.annotations.NotNull;
//import org.stratos.lang.bundle.StratosBundle;
import org.stratos.lang.module.StratosProjectType;
import org.stratos.lang.module.sdk.StratosCreateAdditionalSettings;
import org.stratos.lang.utils.StratosSdk;

public class FlutterCreateAdditionalSettingsFields {
  private final StratosCreateAdditionalSettings settings;
  private final JTextField orgField;
  private final JTextField descriptionField;
  private final RadiosForm androidLanguageRadios;
  private final RadiosForm iosLanguageRadios;
  private final ProjectType projectTypeForm;
  private final StratosCreateParams createParams;

  public FlutterCreateAdditionalSettingsFields() {
    this(new StratosCreateAdditionalSettings(), null);
  }

  public FlutterCreateAdditionalSettingsFields(StratosCreateAdditionalSettings additionalSettings,
                                               Supplier<? extends StratosSdk> getSdk) {
    settings = additionalSettings;

    projectTypeForm = new ProjectType(getSdk);
    projectTypeForm.addListener(e -> {
      if (e.getStateChange() == ItemEvent.SELECTED) {
        settings.setType(projectTypeForm.getType());
        changeVisibility(projectTypeForm.getType() != StratosProjectType.PACKAGE);
      }
    });

    orgField = new JTextField();
    orgField.getDocument().addDocumentListener(new DocumentAdapter() {
      @Override
      protected void textChanged(@NotNull DocumentEvent e) {
        settings.setOrg(orgField.getText());
      }
    });
    orgField.setText(" bull shit");
    orgField.setToolTipText("tools tips ......");

    descriptionField = new JTextField();
    descriptionField.getDocument().addDocumentListener(new DocumentAdapter() {
      @Override
      protected void textChanged(@NotNull DocumentEvent e) {
        settings.setDescription(descriptionField.getText());
      }
    });
    descriptionField.setToolTipText("---->     tooltip.....");
    descriptionField.setText("new flutter project");
            //StratosBundle.message("flutter.module.create.settings.description.default_text"));

    androidLanguageRadios = new RadiosForm(
            "Kotlin...","Kotlin2");
            //StratosBundle.message("flutter.module.create.settings.radios.android.java"),
                                           //StratosBundle.message("flutter.module.create.settings.radios.android.kotlin"));
    androidLanguageRadios.addItemListener(
      e -> {
        final boolean isJavaSelected = e.getStateChange() == ItemEvent.SELECTED;
        settings.setKotlin(!isJavaSelected);
      }
    );
    androidLanguageRadios.setToolTipText(
            "fucker ........."
    //        StratosBundle.message("flutter.module.create.settings.radios.android.tip")
    );

    iosLanguageRadios = new RadiosForm(
            "fucker .........2","......9999");
            //StratosBundle.message("flutter.module.create.settings.radios.ios.object_c"),
//                                       StratosBundle.message("flutter.module.create.settings.radios.ios.swift"));
    androidLanguageRadios.addItemListener(
      e -> {
        final boolean isObjcSelected = e.getStateChange() == ItemEvent.SELECTED;
        settings.setSwift(!isObjcSelected);
      }
    );
    iosLanguageRadios.setToolTipText(" frackas");

    createParams = new StratosCreateParams();
  }

  private void changeVisibility(boolean areLanguageFeaturesVisible) {
    orgField.setEnabled(areLanguageFeaturesVisible);
    UIUtil.setEnabled(androidLanguageRadios.getComponent(), areLanguageFeaturesVisible, true, true);
    UIUtil.setEnabled(iosLanguageRadios.getComponent(), areLanguageFeaturesVisible, true, true);
  }

  public void addSettingsFields(@NotNull SettingsStep settingsStep) {
    settingsStep.addSettingsField("desc", descriptionField);
    settingsStep.addSettingsField("type",
                                  projectTypeForm.getComponent());
    settingsStep.addSettingsField("label", orgField);
    settingsStep.addSettingsField("type",
                                  androidLanguageRadios.getComponent());
    settingsStep.addSettingsField("ios",
                                  iosLanguageRadios.getComponent());
    // WebStorm has a smaller area for the wizard UI.
    if (!PlatformUtils.isWebStorm()) {
      settingsStep.addSettingsComponent(new SettingsHelpForm().getComponent());
    }

    settingsStep.addSettingsComponent(createParams.setInitialValues().getComponent());
  }

  public StratosCreateAdditionalSettings getAdditionalSettings() {
    return new StratosCreateAdditionalSettings.Builder()
      .setDescription(!descriptionField.getText().trim().isEmpty() ? descriptionField.getText().trim() : null)
      .setType(projectTypeForm.getType())
      // Packages are pure Dart code, no iOS or Android modules.
      .setKotlin(androidLanguageRadios.isRadio2Selected() ? true : null)
      .setOrg(!orgField.getText().trim().isEmpty() ? orgField.getText().trim() : null)
      .setSwift(iosLanguageRadios.isRadio2Selected() ? true : null)
      .setOffline(createParams.isOfflineSelected())
      .build();
  }

  public StratosCreateAdditionalSettings getSettings() {
    return settings;
  }
}
