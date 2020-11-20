/*
 * Copyright 2017 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
package org.stratos.lang.module.settings;

import com.intellij.openapi.ui.ComboBox;

//import org.stratos.lang.bundle.StratosBundle;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
import java.util.function.Supplier;
import javax.swing.AbstractListModel;
import javax.swing.ComboBoxModel;
import javax.swing.JComponent;
import javax.swing.JPanel;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;
import org.stratos.lang.module.StratosProjectType;
import org.stratos.lang.utils.StratosSdk;

public class ProjectType {
  private static final class ProjectTypeComboBoxModel extends AbstractListModel<StratosProjectType>
    implements ComboBoxModel<StratosProjectType> {
    private final List<StratosProjectType> myList = new ArrayList<>(EnumSet.allOf(StratosProjectType.class));
    private StratosProjectType mySelected;

    private ProjectTypeComboBoxModel() {
      if (System.getProperty("flutter.experimental.modules", null) == null) {
        myList.remove(StratosProjectType.MODULE);
        myList.remove(StratosProjectType.IMPORT);
      }
      mySelected = myList.get(0);
    }

    @Override
    public int getSize() {
      return myList.size();
    }

    @Override
    public StratosProjectType getElementAt(int index) {
      return myList.get(index);
    }

    @Override
    public void setSelectedItem(Object item) {
      setSelectedItem((StratosProjectType)item);
    }

    @Override
    public StratosProjectType getSelectedItem() {
      return mySelected;
    }

    public void setSelectedItem(StratosProjectType item) {
      mySelected = item;
      fireContentsChanged(this, 0, getSize());
    }
  }

  private Supplier<? extends StratosSdk> getSdk;

  private JPanel projectTypePanel;
  private ComboBox projectTypeCombo;

  public ProjectType(@Nullable Supplier<? extends StratosSdk> getSdk) {
    this.getSdk = getSdk;
  }

  @SuppressWarnings("unused")
  public ProjectType() {
    // Required by AS NPW
  }

  private void createUIComponents() {
    projectTypeCombo = new ComboBox<>();
    //noinspection unchecked
    projectTypeCombo.setModel(new ProjectTypeComboBoxModel());
    projectTypeCombo.setToolTipText("project type hints");//(StratosBundle.message("flutter.module.create.settings.type.tip"));
  }

  @NotNull
  public JComponent getComponent() {
    return projectTypePanel;
  }

  public StratosProjectType getType() {
    return (StratosProjectType)projectTypeCombo.getSelectedItem();
  }

  public ComboBox getProjectTypeCombo() {
    return projectTypeCombo;
  }

  public void setSdk(@NotNull Supplier<? extends StratosSdk> sdk) {
    this.getSdk = sdk;
  }

  public void addListener(ItemListener listener) {
    projectTypeCombo.addItemListener(listener);
  }
}
