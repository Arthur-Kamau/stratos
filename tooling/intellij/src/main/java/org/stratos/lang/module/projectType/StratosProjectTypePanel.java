package org.stratos.lang.module.projectType;

import javax.swing.*;
import javax.swing.border.Border;
import javax.swing.border.CompoundBorder;
import javax.swing.border.EmptyBorder;
import java.awt.*;

public class StratosProjectTypePanel extends JPanel {
    private JPanel myRoot;

    private JList list1;
    private JLabel label1;


    public StratosProjectTypePanel() {
        super(new BorderLayout());
        myRoot = new JPanel(new BorderLayout());


        final DefaultListModel<String> l1 = new DefaultListModel<>();
        l1.addElement("Application ");
        l1.addElement("Library");
        l1.addElement("Plugin");


label1.setText("Choose project type");


        add(myRoot, BorderLayout.CENTER);


        Border border1 = myRoot.getBorder();
        Border margin1 = new EmptyBorder(10,50,10,10);
        myRoot.setBorder(new CompoundBorder(border1, margin1));
        
        myRoot.add(list1, BorderLayout.CENTER);

        Border border = label1.getBorder();
        Border margin = new EmptyBorder(10,10,40,10);
        label1.setBorder(new CompoundBorder(border, margin));
        myRoot.add(label1, BorderLayout.NORTH);




    }
}