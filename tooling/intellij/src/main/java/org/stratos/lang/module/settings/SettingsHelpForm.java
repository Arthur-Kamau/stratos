/*
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
package org.stratos.lang.module.settings;

import com.intellij.ide.browsers.BrowserLauncher;
import com.intellij.ui.components.labels.LinkLabel;
//import io.flutter.StratosBundle;
//import io.flutter.FlutterConstants;
//import org.stratos.lang.bundle.StratosBundle;
import org.jetbrains.annotations.NotNull;

import javax.swing.*;
import java.awt.*;

/**
 * The settings panel that list helps.
 */
public class SettingsHelpForm {
  private JPanel mainPanel;
  private JPanel helpPanel;

  private JLabel helpLabel;

  private JLabel projectTypeLabel;
  private JLabel projectTypeDescriptionForApp;
  private JLabel projectTypeDescriptionForPlugin;
  private JLabel projectTypeDescriptionForPackage;

  private LinkLabel gettingStartedUrl;

  public SettingsHelpForm() {
    projectTypeLabel.setText("settings");//(StratosBundle.message("flutter.module.create.settings.help.project_type.label"));
    projectTypeDescriptionForApp.setText("desk");//(StratosBundle.message("flutter.module.create.settings.help.project_type.description.app"));
    projectTypeDescriptionForPlugin.setText("099999");
            //StratosBundle.message("flutter.module.create.settings.help.project_type.description.plugin"));
    projectTypeDescriptionForPackage.setText("Settings package");
            //(StratosBundle.message("flutter.module.create.settings.help.project_type.description.package"));

    gettingStartedUrl.setText("boom");//StratosBundle.message("flutter.module.create.settings.help.getting_started_link_text"));
    gettingStartedUrl.setCursor(Cursor.getPredefinedCursor(Cursor.HAND_CURSOR));
    gettingStartedUrl.setIcon(null);
    //noinspection unchecked
    gettingStartedUrl
      .setListener((label, linkUrl) -> BrowserLauncher.getInstance().browse("get started url", null), null);
  }


  @NotNull
  public JComponent getComponent() {
    return mainPanel;
  }
}
