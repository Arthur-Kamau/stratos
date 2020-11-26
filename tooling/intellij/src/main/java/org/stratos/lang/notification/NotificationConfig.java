package org.stratos.lang.notification;

import com.google.common.collect.Lists;
        import com.intellij.notification.Notification;
        import com.intellij.notification.NotificationGroup;
        import com.intellij.notification.NotificationType;
        import com.intellij.openapi.actionSystem.AnAction;
        import org.jetbrains.annotations.NotNull;
        import org.jetbrains.annotations.Nullable;

        import javax.swing.*;
        import java.util.List;

/**
 * Container to store the configuration of a notification.
 */
class NotificationConfig {
    @NotNull
    final List<AnAction> actions = Lists.newArrayList();
    @NotNull
    final String title;
    final String subtitle;
    @NotNull
    final String content;
    @Nullable
    final String dropdownText;
    @Nullable
    final Notification.CollapseActionsDirection collapseDirection;
    @Nullable
    final Icon icon;
    final boolean actionIcons;
    @NotNull
    final NotificationGroup group;
    final boolean isFullContent;
    final boolean isImportant;
    @NotNull
    final NotificationType notificationType;
    @Nullable
    public AnAction contextHelpAction;

    NotificationConfig(@NotNull String title, String subtitle, @NotNull String content, @Nullable String dropdownText,
                       @Nullable Notification.CollapseActionsDirection collapseDirection, @Nullable Icon icon,
                       @NotNull NotificationGroup group, boolean isFullContent, boolean isImportant,
                       @NotNull NotificationType notificationType, @NotNull List<AnAction> actions,
                       boolean actionIcons, @Nullable AnAction contextHelpAction) {
        this.title = title;
        this.subtitle = subtitle;
        this.content = content;
        this.dropdownText = dropdownText;
        this.collapseDirection = collapseDirection;
        this.icon = icon;
        this.actionIcons = actionIcons;
        this.group = group;
        this.isFullContent = isFullContent;
        this.isImportant = isImportant;
        this.notificationType = notificationType;
        this.contextHelpAction = contextHelpAction;
        this.actions.addAll(actions);
    }
}
