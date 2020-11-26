package org.stratos.lang.notification;

//public class NotificationConfigBuilder {
//}

import com.google.common.collect.Lists;
        import com.intellij.icons.AllIcons;
        import com.intellij.notification.Notification;
        import com.intellij.notification.NotificationDisplayType;
        import com.intellij.notification.NotificationGroup;
        import com.intellij.notification.NotificationType;
        import com.intellij.openapi.actionSystem.AnAction;
        import com.intellij.openapi.actionSystem.AnActionEvent;
        import com.intellij.openapi.project.Project;
        import com.intellij.openapi.ui.Messages;
        import com.intellij.openapi.wm.ToolWindowManager;
        import com.intellij.openapi.wm.ex.ToolWindowManagerEx;
        import com.intellij.util.PlatformIcons;
        import org.jetbrains.annotations.NotNull;
        import org.jetbrains.annotations.Nullable;

        import javax.swing.*;
        import java.util.List;

public class NotificationConfigBuilder {
    private static final NotificationGroup STICKY_GROUP = new NotificationGroup("demo.notifications.balloon", NotificationDisplayType.STICKY_BALLOON, true);
    private static final NotificationGroup BALLOON_GROUP = new NotificationGroup("demo.notifications.stickyBalloon", NotificationDisplayType.BALLOON, true);

    private final Project project;

    @NotNull
    private String title = "";
    private String subtitle;
    @NotNull
    private String content = "";
    @Nullable
    private String dropdownText;
    @Nullable
    private Notification.CollapseActionsDirection collapseDirection;
    @Nullable
    private Icon icon;
    private boolean actionIcons;
    @NotNull
    private NotificationGroup group = BALLOON_GROUP;
    private boolean isFullContent;
    private boolean isImportant;
    @NotNull
    private NotificationType notificationType = NotificationType.INFORMATION;
    private List<AnAction> actions = Lists.newArrayList();
    @Nullable
    private AnAction contextHelpAction;

    private NotificationConfigBuilder(Project project) {
        this.project = project;
    }

    public static NotificationConfigBuilder create(Project project) {
        return new NotificationConfigBuilder(project);
    }

    static NotificationConfigBuilder create(Project project, NotificationConfig config) {
        NotificationConfigBuilder b = new NotificationConfigBuilder(project);
        b.title = config.title;
        b.subtitle = config.subtitle;
        b.content = config.content;
        b.dropdownText = config.dropdownText;
        b.collapseDirection = config.collapseDirection;
        b.icon = config.icon;
        b.actionIcons = config.actionIcons;
        b.group = config.group;
        b.isFullContent = config.isFullContent;
        b.isImportant = config.isImportant;
        b.notificationType = config.notificationType;
        b.contextHelpAction = config.contextHelpAction;

        b.actions.addAll(config.actions);

        return b;
    }

    public void setTitle(@NotNull String title) {
        this.title = title;
    }

    public void setSubtitle(String subtitle) {
        this.subtitle = subtitle;
    }

    public void setContent(@NotNull String content) {
        this.content = content;
    }

    public void setDropdownText(@Nullable String dropdownText) {
        this.dropdownText = dropdownText;
    }

    public void setCollapseDirection(@Nullable Notification.CollapseActionsDirection collapseDirection) {
        this.collapseDirection = collapseDirection;
    }

    public void setIcon(@Nullable Icon icon) {
        this.icon = icon;
    }

    public void setGroup(@NotNull NotificationGroup group) {
        this.group = group;
    }

    public void setFullContent(boolean fullContent) {
        isFullContent = fullContent;
    }

    public void setImportant(boolean important) {
        isImportant = important;
    }

    public void setNotificationType(@NotNull NotificationType notificationType) {
        this.notificationType = notificationType;
    }

    public void addAction(AnAction actions) {
        this.actions.add(actions);
    }

    public void setActionIcons(boolean actionIcons) {
        this.actionIcons = actionIcons;
    }

    public void setContextHelpAction(@Nullable AnAction contextHelpAction) {
        this.contextHelpAction = contextHelpAction;
    }

    public Notification build() {
        NotificationConfig config = new NotificationConfig(title,
                subtitle,
                content,
                dropdownText,
                collapseDirection,
                icon,
                group,
                isFullContent,
                isImportant,
                notificationType,
                actions,
                actionIcons,
                contextHelpAction);

        if (this.isFullContent) {
            return new FullContentConfigurableNotification(config);
        }
        return new ConfigurableNotification(config);
    }

    public void resetActions() {
        this.actions.clear();
    }

    public void addDefaultActions() {
        if (this.group.getDisplayType() == NotificationDisplayType.TOOL_WINDOW) {
            // toolwindow notifications don't support actions
            return;
        }

        addAction(ConfigurableNotificationAction.create(isFullContent ? "Collapse content" : "Expand content", "Expand a notification to full height",
                actionIcons ? AllIcons.General.ExpandComponent : null,
                builder -> builder.setFullContent(!builder.isFullContent)));

        addAction(ConfigurableNotificationAction.create(isImportant ? "Set as not important" : "set as important", "Mark the notification as important ",
                actionIcons ? AllIcons.General.TodoImportant : null,
                builder -> builder.setImportant(!builder.isImportant)));

        addAction(ConfigurableNotificationAction.create("As information", "set notification type to INFORMATION",
                actionIcons ? AllIcons.General.Information : null,
                builder -> builder.setNotificationType(NotificationType.INFORMATION)));

        addAction(ConfigurableNotificationAction.create("As warnings", "set notification type to WARNING",
                actionIcons ? AllIcons.General.Warning : null,
                builder -> builder.setNotificationType(NotificationType.WARNING)));

        addAction(ConfigurableNotificationAction.create("As error", "set notification type to ERROR",
                actionIcons ? AllIcons.General.Error : null,
                builder -> builder.setNotificationType(NotificationType.ERROR)));

        if (collapseDirection == Notification.CollapseActionsDirection.KEEP_LEFTMOST) {
            addAction(ConfigurableNotificationAction.create("Collapse actions on the left", "collapse action on the left",
                    actionIcons ? AllIcons.General.CollapseComponent : null,
                    builder -> builder.setCollapseDirection(Notification.CollapseActionsDirection.KEEP_RIGHTMOST)));
        } else {
            addAction(ConfigurableNotificationAction.create("Collapse actions on the right", "collapse actions on the right",
                    actionIcons ? AllIcons.General.CollapseComponent : null,
                    builder -> builder.setCollapseDirection(Notification.CollapseActionsDirection.KEEP_LEFTMOST)));
        }

        addAction(ConfigurableNotificationAction.create("Set title", "modify the title",
                actionIcons ? AllIcons.General.Inline_edit : null,
                builder -> {
                    String value = Messages.showInputDialog(project, "Title:", "Notification Title", null, builder.title, null);
                    if (value != null) {
                        builder.setTitle(value);
                    }
                }));

        addAction(ConfigurableNotificationAction.create("Set subtitle", "modify the subtitle",
                actionIcons ? AllIcons.General.Inline_edit : null,
                builder -> {
                    String value = Messages.showInputDialog(project, "Subtitle:", "Notification Subtitle", null, builder.subtitle, null);
                    if (value != null) {
                        builder.setSubtitle(value);
                    }
                }));

        addAction(ConfigurableNotificationAction.create("Set content", "modify the content",
                actionIcons ? AllIcons.General.Inline_edit : null,
                builder -> {
                    String value = Messages.showMultilineInputDialog(project, "Content:", "Notification Content", builder.content, null, null);
                    if (value != null) {
                        builder.setContent(value);
                    }
                }));

        addAction(ConfigurableNotificationAction.create("Set dropdown text", "set the action dropdown text",
                actionIcons ? AllIcons.General.Dropdown : null,
                builder -> {
                    String value = Messages.showInputDialog(project, "Dropdown text:", "Notification Dropdown Text", null, builder.dropdownText, null);
                    if (value != null) {
                        builder.setDropdownText(value);
                    }
                }));

        if (group.getDisplayType() != NotificationDisplayType.STICKY_BALLOON) {
            addAction(ConfigurableNotificationAction.create("Set sticky", "make the notification sticky",
                    actionIcons ? AllIcons.General.Balloon : null,
                    builder -> builder.setGroup(STICKY_GROUP)));
        }

        if (group.getDisplayType() != NotificationDisplayType.BALLOON) {
            addAction(ConfigurableNotificationAction.create("Set non-sticky", "make the notification non-sticky",
                    actionIcons ? AllIcons.General.Balloon : null,
                    builder -> builder.setGroup(BALLOON_GROUP)));
        }

        if (group.getDisplayType() != NotificationDisplayType.TOOL_WINDOW) {
            addAction(ConfigurableNotificationAction.create("As tool window notification", "show the notification for a tool window",
                    actionIcons ? AllIcons.General.HideToolWindow : null,
                    builder -> {
                        String activeId = ToolWindowManager.getInstance(project).getActiveToolWindowId();
                        if (activeId == null) {
                            activeId = ToolWindowManagerEx.getInstance(project).getToolWindowIds()[0];
                        }
                        builder.setGroup(NotificationGroup.toolWindowGroup("demo.notifications.toolwindow", activeId));
                    }));
        }

        if (icon == null) {
            addAction(ConfigurableNotificationAction.create("With custom notification icon", "set the icon of the notification",
                    actionIcons ? AllIcons.General.Note : null,
                    builder -> builder.setIcon(PlatformIcons.PROJECT_ICON)));
        } else {
            addAction(ConfigurableNotificationAction.create("No custom notification icon", "use the default notification icons",
                    actionIcons ? AllIcons.General.Note : null,
                    builder -> builder.setIcon(null)));
        }

        addAction(ConfigurableNotificationAction.create(actionIcons ? "Hide action icons" : "Show action icons", "show/hide icons of actions",
                actionIcons ? AllIcons.Actions.Show : null,
                builder -> builder.setActionIcons(!actionIcons)));

        if (contextHelpAction == null) {
            addAction(ConfigurableNotificationAction.create("With context help action", "show/hide context help action",
                    actionIcons ? AllIcons.Actions.Help : null,
                    builder -> builder.setContextHelpAction(new AnAction("Help!", "<em>This</em> is the context help action of this notification!", null) {
                        @Override
                        public void actionPerformed(@NotNull AnActionEvent e) {
                        }
                    })));
        } else {
            addAction(ConfigurableNotificationAction.create("No context help action", "show/hide context help action",
                    actionIcons ? AllIcons.Actions.Help : null,
                    builder -> builder.setContextHelpAction(null)));
        }
    }
}
