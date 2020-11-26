package org.stratos.lang.notification;


import com.intellij.notification.impl.NotificationFullContent;

/**
 * Inherits all from {@link ConfigurableNotification} and implements the interface to mark
 * this notification as full content.
 */
class FullContentConfigurableNotification extends ConfigurableNotification implements NotificationFullContent {
    FullContentConfigurableNotification(NotificationConfig config) {
        super(config);
    }
}