package org.stratos.lang;

import com.intellij.openapi.fileTypes.LanguageFileType;
import org.jetbrains.annotations.NotNull;
import org.jetbrains.annotations.Nullable;

import javax.swing.*;

public class StratosFileType extends LanguageFileType {
	public static final String FILE_EXTENSION = "sts";
		public static final StratosFileType INSTANCE = new StratosFileType();

	protected StratosFileType() {
		super(StratosLanguage.INSTANCE);
	}

	@NotNull
	@Override
	public String getName() {
		return "Stratos Language";
	}

	@NotNull
	@Override
	public String getDescription() {
		return "Stratos language";
	}

	@NotNull
	@Override
	public String getDefaultExtension() {
		return FILE_EXTENSION;
	}

	@Nullable
	@Override
	public Icon getIcon() {
		return StratosIcons.SAMPLE_ICON;
	}
}
