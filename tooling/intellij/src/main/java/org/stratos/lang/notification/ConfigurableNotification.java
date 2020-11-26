package org.stratos.lang.notification;

import com.intellij.notification.Notification;
        import com.intellij.notification.NotificationListener;
        import com.intellij.openapi.actionSystem.AnAction;
        import org.jetbrains.annotations.NotNull;

/**
 * {@link ConfigurableNotification} applies the settings defined in {@link NotificationConfig}.
 * <p>
 * Please don't use this in production code. It's a notification to demonstrate all properties. Therefore
 * {@link NotificationConfig} has been introduced. Usually you can modify a notification directly, without
 * using an intermediate state container.
 */
class ConfigurableNotification extends Notification {
    @NotNull
    final NotificationConfig config;

    ConfigurableNotification(@NotNull NotificationConfig config) {
        super(config.group.getDisplayId(), config.title, config.content, config.notificationType, NotificationListener.URL_OPENING_LISTENER);
        this.config = config;

        setTitle(config.title, config.subtitle);
        setContent(config.content);
        setIcon(config.icon);
        setImportant(config.isImportant);

        if (config.dropdownText != null) {
            setDropDownText(config.dropdownText);
        }

        if (config.collapseDirection != null) {
            setCollapseActionsDirection(config.collapseDirection);
        }

        setContextHelpAction(config.contextHelpAction);

        for (AnAction action : config.actions) {
            addAction(action);
        }
    }
}
