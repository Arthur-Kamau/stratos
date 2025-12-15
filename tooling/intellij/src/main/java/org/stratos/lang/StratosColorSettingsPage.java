package org.stratos.lang;

import com.intellij.openapi.editor.colors.TextAttributesKey;
import com.intellij.openapi.fileTypes.SyntaxHighlighter;
import com.intellij.openapi.options.colors.AttributesDescriptor;
import com.intellij.openapi.options.colors.ColorDescriptor;
import com.intellij.openapi.options.colors.ColorSettingsPage;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;
import java.util.Map;

public class StratosColorSettingsPage implements ColorSettingsPage {
	private static final AttributesDescriptor[] DESCRIPTORS = new AttributesDescriptor[]{
		new AttributesDescriptor("Identifier", StratosSyntaxHighlighter.ID),
		new AttributesDescriptor("Keyword", StratosSyntaxHighlighter.KEYWORD),
		new AttributesDescriptor("String", StratosSyntaxHighlighter.STRING),
		new AttributesDescriptor("Number", StratosSyntaxHighlighter.NUMBER),
		new AttributesDescriptor("Comment", StratosSyntaxHighlighter.COMMENT),
		new AttributesDescriptor("Block Comment", StratosSyntaxHighlighter.BLOCK_COMMENT),
	};

	@Nullable
	@Override
	public Map<String, TextAttributesKey> getAdditionalHighlightingTagToDescriptorMap() {
		return null;
	}

	@Nullable
	@Override
	public Icon getIcon() {
		return Icons.SAMPLE_ICON;
	}

	@NotNull
	@Override
	public SyntaxHighlighter getHighlighter() {
		return new StratosSyntaxHighlighter();
	}

	@NotNull
	@Override
	public String getDemoText() {
		return
			"// Stratos Demo Code\n" +
			"/* This is a block comment */\n" +
			"package main;\n\n" +
			"use std:io;\n\n" +
			"val pi: double = 3.14159;\n\n" +
			"class Circle {\n" +
			"    var radius: double;\n" +
			"    constructor(r: double) {\n" +
			"        this.radius = r;\n" +
			"    }\n" +
			"}\n\n" +
			"fn calculateArea(c: Circle) double {\n" +
			"    return pi * c.radius * c.radius;\n" +
			"}\n\n" +
			"fn main() {\n" +
			"    val myCircle = Circle(10.0);\n" +
			"    if (myCircle.radius > 5.0) {\n" +
			"        print(\"Big Circle\");\n" +
			"    } else {\n" +
			"        print(\"Small Circle\");\n" +
			"    }\n" +
			"}";
	}

	@NotNull
	@Override
	public AttributesDescriptor[] getAttributeDescriptors() {
		return DESCRIPTORS;
	}

	@NotNull
	@Override
	public ColorDescriptor[] getColorDescriptors() {
		return ColorDescriptor.EMPTY_ARRAY;
	}

	@NotNull
	@Override
	public String getDisplayName() {
		return "Stratos";
	}
}