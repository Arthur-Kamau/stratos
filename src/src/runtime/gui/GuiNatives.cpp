#include "stratos/NativeRegistry.h"
#include "stratos/gui/App.h"
#include "stratos/gui/Widget.h"
#include "stratos/gui/Renderer.h"
#include <iostream>
#include <unordered_map>
#include <memory>

namespace stratos {

// Global storage for GUI objects (referenced by ID from Stratos code)
static int nextWidgetId = 1;
static std::unordered_map<int, gui::WidgetPtr> widgetRegistry;
static std::unique_ptr<gui::App> currentApp;

static int storeWidget(gui::WidgetPtr widget) {
    int id = nextWidgetId++;
    widgetRegistry[id] = widget;
    return id;
}

static gui::WidgetPtr getWidget(int id) {
    auto it = widgetRegistry.find(id);
    return (it != widgetRegistry.end()) ? it->second : nullptr;
}

void NativeRegistry::initGui() {
    // ========================================
    // App lifecycle
    // ========================================

    registerFunction("gui", "__gui_app_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string title = std::any_cast<std::string>(args[0]);
            int width = std::any_cast<int>(args[1]);
            int height = std::any_cast<int>(args[2]);
            currentApp = std::make_unique<gui::App>(title, width, height);
            if (!currentApp->init()) {
                std::cerr << "Failed to initialize GUI app" << std::endl;
                return false;
            }
            return true;
        });

    registerFunction("gui", "__gui_app_run",
        [](const std::vector<std::any>& args) -> std::any {
            if (!currentApp) return false;
            currentApp->run();
            return true;
        });

    registerFunction("gui", "__gui_app_quit",
        [](const std::vector<std::any>& args) -> std::any {
            if (currentApp) currentApp->quit();
            return true;
        });

    registerFunction("gui", "__gui_app_set_root",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto widget = getWidget(widgetId);
            if (currentApp && widget) {
                currentApp->setRoot(widget);
            }
            return true;
        });

    registerFunction("gui", "__gui_app_set_theme",
        [](const std::vector<std::any>& args) -> std::any {
            std::string themeName = std::any_cast<std::string>(args[0]);
            if (!currentApp) return false;
            if (themeName == "dark") currentApp->setTheme(gui::Theme::dark());
            else currentApp->setTheme(gui::Theme::light());
            return true;
        });

    registerFunction("gui", "__gui_app_set_fps",
        [](const std::vector<std::any>& args) -> std::any {
            int fps = std::any_cast<int>(args[0]);
            if (currentApp) currentApp->setTargetFPS(fps);
            return true;
        });

    // ========================================
    // Widget creation
    // ========================================

    // Text
    registerFunction("gui", "__gui_text_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string text = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Text>(text);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_text_set_font_size",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float size = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                gui::FontSpec font = widget->getFont();
                font.size = size;
                widget->setFont(font);
            }
            return true;
        });

    registerFunction("gui", "__gui_text_set_color",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int r = std::any_cast<int>(args[1]);
            int g = std::any_cast<int>(args[2]);
            int b = std::any_cast<int>(args[3]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) widget->setColor(gui::Color::rgb(r, g, b));
            return true;
        });

    registerFunction("gui", "__gui_text_set_bold",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                gui::FontSpec font = widget->getFont();
                font.weight = gui::FontWeight::Bold;
                widget->setFont(font);
            }
            return true;
        });

    registerFunction("gui", "__gui_text_set_align",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string align = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                if (align == "center") widget->setTextAlign(gui::TextAlign::Center);
                else if (align == "right") widget->setTextAlign(gui::TextAlign::Right);
                else widget->setTextAlign(gui::TextAlign::Left);
            }
            return true;
        });

    // Button
    registerFunction("gui", "__gui_button_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Button>(label);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_button_set_color",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int r = std::any_cast<int>(args[1]);
            int g = std::any_cast<int>(args[2]);
            int b = std::any_cast<int>(args[3]);
            auto widget = getWidget(id);
            if (widget) widget->style().backgroundColor = gui::Color::rgb(r, g, b);
            return true;
        });

    // Box / Container
    registerFunction("gui", "__gui_box_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Box>();
            return storeWidget(widget);
        });

    // Image
    registerFunction("gui", "__gui_image_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string path = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Image>(path);
            return storeWidget(widget);
        });

    // TextField
    registerFunction("gui", "__gui_textfield_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::TextField>();
            if (!args.empty()) {
                widget->setPlaceholder(std::any_cast<std::string>(args[0]));
            }
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_textfield_get_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::TextField>(getWidget(id));
            if (widget) return widget->getValue();
            return std::string("");
        });

    registerFunction("gui", "__gui_textfield_set_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string value = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::TextField>(getWidget(id));
            if (widget) widget->setValue(value);
            return true;
        });

    // Checkbox
    registerFunction("gui", "__gui_checkbox_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = args.empty() ? "" : std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Checkbox>(label);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_checkbox_is_checked",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Checkbox>(getWidget(id));
            return widget ? widget->isChecked() : false;
        });

    // Slider
    registerFunction("gui", "__gui_slider_create",
        [](const std::vector<std::any>& args) -> std::any {
            float min = static_cast<float>(std::any_cast<double>(args[0]));
            float max = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::make_shared<gui::Slider>(min, max);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_slider_get_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Slider>(getWidget(id));
            return widget ? static_cast<double>(widget->getValue()) : 0.0;
        });

    // Switch
    registerFunction("gui", "__gui_switch_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Switch>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_switch_is_on",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Switch>(getWidget(id));
            return widget ? widget->isOn() : false;
        });

    // AppBar
    registerFunction("gui", "__gui_appbar_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string title = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::AppBar>(title);
            return storeWidget(widget);
        });

    // Dialog
    registerFunction("gui", "__gui_dialog_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string title = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Dialog>(title);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_dialog_open",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool open = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Dialog>(getWidget(id));
            if (widget) widget->setOpen(open);
            return true;
        });

    // ========================================
    // Layout widgets
    // ========================================

    registerFunction("gui", "__gui_row_create",
        [](const std::vector<std::any>& args) -> std::any {
            float spacing = args.empty() ? 0.0f :
                            static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::Row>(spacing);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_row_set_alignment",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string main = std::any_cast<std::string>(args[1]);
            std::string cross = std::any_cast<std::string>(args[2]);
            auto widget = std::dynamic_pointer_cast<gui::Row>(getWidget(id));
            if (widget) {
                if (main == "center") widget->setMainAxisAlignment(gui::MainAxisAlignment::Center);
                else if (main == "end") widget->setMainAxisAlignment(gui::MainAxisAlignment::End);
                else if (main == "spaceBetween") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceBetween);
                else if (main == "spaceAround") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceAround);
                else if (main == "spaceEvenly") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceEvenly);

                if (cross == "center") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::Center);
                else if (cross == "end") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::End);
                else if (cross == "stretch") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::Stretch);
            }
            return true;
        });

    registerFunction("gui", "__gui_column_create",
        [](const std::vector<std::any>& args) -> std::any {
            float spacing = args.empty() ? 0.0f :
                            static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::Column>(spacing);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_column_set_alignment",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string main = std::any_cast<std::string>(args[1]);
            std::string cross = std::any_cast<std::string>(args[2]);
            auto widget = std::dynamic_pointer_cast<gui::Column>(getWidget(id));
            if (widget) {
                if (main == "center") widget->setMainAxisAlignment(gui::MainAxisAlignment::Center);
                else if (main == "end") widget->setMainAxisAlignment(gui::MainAxisAlignment::End);
                else if (main == "spaceBetween") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceBetween);
                else if (main == "spaceAround") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceAround);
                else if (main == "spaceEvenly") widget->setMainAxisAlignment(gui::MainAxisAlignment::SpaceEvenly);

                if (cross == "center") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::Center);
                else if (cross == "end") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::End);
                else if (cross == "stretch") widget->setCrossAxisAlignment(gui::CrossAxisAlignment::Stretch);
            }
            return true;
        });

    registerFunction("gui", "__gui_stack_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Stack>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_center_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Center>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_spacer_create",
        [](const std::vector<std::any>& args) -> std::any {
            float size = args.empty() ? 0.0f :
                         static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::Spacer>(size);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_scrollview_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::ScrollView>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_grid_create",
        [](const std::vector<std::any>& args) -> std::any {
            int columns = std::any_cast<int>(args[0]);
            float spacing = args.size() > 1 ?
                            static_cast<float>(std::any_cast<double>(args[1])) : 8.0f;
            auto widget = std::make_shared<gui::Grid>(columns, spacing);
            return storeWidget(widget);
        });

    // ========================================
    // Generic widget operations
    // ========================================

    registerFunction("gui", "__gui_widget_add_child",
        [](const std::vector<std::any>& args) -> std::any {
            int parentId = std::any_cast<int>(args[0]);
            int childId = std::any_cast<int>(args[1]);
            auto parent = getWidget(parentId);
            auto child = getWidget(childId);
            if (parent && child) parent->addChild(child);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_size",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float w = static_cast<float>(std::any_cast<double>(args[1]));
            float h = static_cast<float>(std::any_cast<double>(args[2]));
            auto widget = getWidget(id);
            if (widget) {
                widget->style().width = w;
                widget->style().height = h;
            }
            return true;
        });

    registerFunction("gui", "__gui_widget_set_padding",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float all = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (widget) widget->style().padding = gui::EdgeInsets::all(all);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_margin",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float all = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (widget) widget->style().margin = gui::EdgeInsets::all(all);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_background",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int r = std::any_cast<int>(args[1]);
            int g = std::any_cast<int>(args[2]);
            int b = std::any_cast<int>(args[3]);
            auto widget = getWidget(id);
            if (widget) widget->style().backgroundColor = gui::Color::rgb(r, g, b);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_border",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float width = static_cast<float>(std::any_cast<double>(args[1]));
            int r = std::any_cast<int>(args[2]);
            int g = std::any_cast<int>(args[3]);
            int b = std::any_cast<int>(args[4]);
            auto widget = getWidget(id);
            if (widget) {
                widget->style().borderWidth = width;
                widget->style().borderColor = gui::Color::rgb(r, g, b);
            }
            return true;
        });

    registerFunction("gui", "__gui_widget_set_border_radius",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float radius = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (widget) widget->style().borderRadius = gui::BorderRadius::all(radius);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_shadow",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = getWidget(id);
            if (widget) {
                widget->style().hasShadow = true;
                widget->style().shadow = {0, 2, 6, gui::Color::rgba(0, 0, 0, 40)};
            }
            return true;
        });

    registerFunction("gui", "__gui_widget_set_visible",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool visible = std::any_cast<bool>(args[1]);
            auto widget = getWidget(id);
            if (widget) widget->setVisible(visible);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_enabled",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool enabled = std::any_cast<bool>(args[1]);
            auto widget = getWidget(id);
            if (widget) widget->setEnabled(enabled);
            return true;
        });

    registerFunction("gui", "__gui_widget_set_flex",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float flex = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (widget) widget->style().flex = flex;
            return true;
        });

    registerFunction("gui", "__gui_widget_set_opacity",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float opacity = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (widget) widget->style().opacity = opacity;
            return true;
        });

    // ========================================
    // Event handler registration
    // ========================================

    // Note: Event handlers from Stratos use a callback mechanism.
    // The interpreter stores the Stratos function reference and
    // calls it when the C++ widget fires the event.
    // This is handled via the Interpreter's callback system.

    registerFunction("gui", "__gui_widget_set_onclick",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            // The callback function is stored as an opaque ID by the interpreter
            // and invoked through the interpreter's callback dispatch
            auto widget = getWidget(id);
            if (widget) {
                // The interpreter will set this up via its own mechanism
                // This is a placeholder for the callback bridge
            }
            return true;
        });

    // ========================================
    // Cleanup
    // ========================================

    registerFunction("gui", "__gui_destroy_widget",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            widgetRegistry.erase(id);
            return true;
        });

    registerFunction("gui", "__gui_destroy_all",
        [](const std::vector<std::any>& args) -> std::any {
            widgetRegistry.clear();
            nextWidgetId = 1;
            currentApp.reset();
            return true;
        });
}

} // namespace stratos
