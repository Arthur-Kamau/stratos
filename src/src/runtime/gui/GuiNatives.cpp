#include "stratos/NativeRegistry.h"
#include "stratos/Interpreter.h"
#include "stratos/gui/App.h"
#include "stratos/gui/Widget.h"
#include "stratos/gui/Renderer.h"
#include "stratos/gui/Signals.h"
#include <iostream>
#include <unordered_map>
#include <memory>
#include <functional>

namespace stratos {

// Global storage for GUI objects (referenced by ID from Stratos code)
static int nextWidgetId = 1;
static std::unordered_map<int, gui::WidgetPtr> widgetRegistry;
static std::unique_ptr<gui::App> currentApp;

// Callback storage: widget ID -> callback function (set by interpreter)
static int nextCallbackId = 1;
static std::unordered_map<int, std::function<void()>> clickCallbacks;
static std::unordered_map<int, std::function<void(std::string)>> changeCallbacks;
static std::unordered_map<int, std::function<void(bool)>> boolChangeCallbacks;
static std::unordered_map<int, std::function<void(float)>> floatChangeCallbacks;
static std::unordered_map<int, std::function<void(float, float)>> mouseCallbacks;
static std::unordered_map<int, std::function<void(int, int)>> keyCallbacks;

// State storage: state ID -> value
static int nextStateId = 1;
static std::unordered_map<int, std::any> stateValues;
static std::unordered_map<int, std::vector<std::function<void()>>> stateListeners;

// Interpreter bridge for callback dispatch
static Interpreter* guiInterpreter_ = nullptr;

// Signal effects: signalId -> list of {widgetId, closure} pairs
static std::unordered_map<int, std::vector<std::pair<int, RuntimeValue>>> signalEffects;

// Free function to set interpreter reference from outside
void setGuiInterpreter(void* interp) {
    guiInterpreter_ = static_cast<Interpreter*>(interp);
}

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

            // Clean up all static state after the event loop exits.
            // Order matters: clear callbacks/signals first (they hold closures
            // that reference the interpreter), then widgets (before SDL is
            // fully torn down by currentApp destruction), then the app itself.
            guiInterpreter_ = nullptr;
            clickCallbacks.clear();
            changeCallbacks.clear();
            boolChangeCallbacks.clear();
            floatChangeCallbacks.clear();
            mouseCallbacks.clear();
            keyCallbacks.clear();
            stateValues.clear();
            stateListeners.clear();
            signalEffects.clear();
            gui::SignalRegistry::instance().clear();
            widgetRegistry.clear();
            currentApp.reset();

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

    registerFunction("gui", "__gui_text_set_content",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string text = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                widget->setText(text);
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
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

    // ========================================
    // Typography: font weight, style, family, generic text color
    // ========================================

    // Set font weight by name: "thin","light","regular","medium","semibold","bold","extrabold","black"
    registerFunction("gui", "__gui_text_set_weight",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string w = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                gui::FontSpec font = widget->getFont();
                if (w == "thin") font.weight = gui::FontWeight::Thin;
                else if (w == "light") font.weight = gui::FontWeight::Light;
                else if (w == "regular") font.weight = gui::FontWeight::Regular;
                else if (w == "medium") font.weight = gui::FontWeight::Medium;
                else if (w == "semibold") font.weight = gui::FontWeight::SemiBold;
                else if (w == "bold") font.weight = gui::FontWeight::Bold;
                else if (w == "extrabold") font.weight = gui::FontWeight::ExtraBold;
                else if (w == "black") font.weight = gui::FontWeight::Black;
                widget->setFont(font);
            }
            return true;
        });

    // Set italic
    registerFunction("gui", "__gui_text_set_italic",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool italic = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                gui::FontSpec font = widget->getFont();
                font.style = italic ? gui::FontStyle::Italic : gui::FontStyle::Normal;
                widget->setFont(font);
            }
            return true;
        });

    // Set font family
    registerFunction("gui", "__gui_text_set_font_family",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string family = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Text>(getWidget(id));
            if (widget) {
                gui::FontSpec font = widget->getFont();
                font.family = family;
                widget->setFont(font);
            }
            return true;
        });

    // Generic widget font size (works on any widget that has a font_)
    registerFunction("gui", "__gui_widget_set_font_size",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float size = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = getWidget(id);
            if (!widget) return true;
            // Try each widget type that has a font
            if (auto t = std::dynamic_pointer_cast<gui::Text>(widget)) {
                gui::FontSpec f = t->getFont(); f.size = size; t->setFont(f);
            } else if (auto b = std::dynamic_pointer_cast<gui::Button>(widget)) {
                gui::FontSpec f; f.size = size; b->setFont(f);
            }
            return true;
        });

    // Generic widget text color (works on Text, Button, Icon)
    registerFunction("gui", "__gui_widget_set_text_color",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int r = std::any_cast<int>(args[1]);
            int g = std::any_cast<int>(args[2]);
            int b = std::any_cast<int>(args[3]);
            auto widget = getWidget(id);
            if (!widget) return true;
            if (auto t = std::dynamic_pointer_cast<gui::Text>(widget)) {
                t->setColor(gui::Color::rgb(r, g, b));
            } else if (auto btn = std::dynamic_pointer_cast<gui::Button>(widget)) {
                btn->setTextColor(gui::Color::rgb(r, g, b));
            } else if (auto icon = std::dynamic_pointer_cast<gui::Icon>(widget)) {
                icon->setColor(gui::Color::rgb(r, g, b));
            }
            return true;
        });

    // Generic widget font weight
    registerFunction("gui", "__gui_widget_set_font_weight",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string w = std::any_cast<std::string>(args[1]);
            auto widget = getWidget(id);
            if (!widget) return true;
            gui::FontWeight weight = gui::FontWeight::Regular;
            if (w == "thin") weight = gui::FontWeight::Thin;
            else if (w == "light") weight = gui::FontWeight::Light;
            else if (w == "medium") weight = gui::FontWeight::Medium;
            else if (w == "semibold") weight = gui::FontWeight::SemiBold;
            else if (w == "bold") weight = gui::FontWeight::Bold;
            else if (w == "extrabold") weight = gui::FontWeight::ExtraBold;
            else if (w == "black") weight = gui::FontWeight::Black;
            if (auto t = std::dynamic_pointer_cast<gui::Text>(widget)) {
                gui::FontSpec f = t->getFont(); f.weight = weight; t->setFont(f);
            } else if (auto btn = std::dynamic_pointer_cast<gui::Button>(widget)) {
                gui::FontSpec f; f.weight = weight; btn->setFont(f);
            }
            return true;
        });

    // Generic widget font family
    registerFunction("gui", "__gui_widget_set_font_family",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string family = std::any_cast<std::string>(args[1]);
            auto widget = getWidget(id);
            if (!widget) return true;
            if (auto t = std::dynamic_pointer_cast<gui::Text>(widget)) {
                gui::FontSpec f = t->getFont(); f.family = family; t->setFont(f);
            } else if (auto btn = std::dynamic_pointer_cast<gui::Button>(widget)) {
                gui::FontSpec f; f.family = family; btn->setFont(f);
            }
            return true;
        });

    // Generic widget italic
    registerFunction("gui", "__gui_widget_set_italic",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool italic = std::any_cast<bool>(args[1]);
            auto widget = getWidget(id);
            if (!widget) return true;
            gui::FontStyle style = italic ? gui::FontStyle::Italic : gui::FontStyle::Normal;
            if (auto t = std::dynamic_pointer_cast<gui::Text>(widget)) {
                gui::FontSpec f = t->getFont(); f.style = style; t->setFont(f);
            } else if (auto btn = std::dynamic_pointer_cast<gui::Button>(widget)) {
                gui::FontSpec f; f.style = style; btn->setFont(f);
            }
            return true;
        });

    // Font management
    registerFunction("gui", "__gui_font_register_file",
        [](const std::vector<std::any>& args) -> std::any {
            std::string family = std::any_cast<std::string>(args[0]);
            std::string path = std::any_cast<std::string>(args[1]);
            std::string variant = args.size() > 2 ? std::any_cast<std::string>(args[2]) : "regular";
            if (currentApp) {
                currentApp->getRenderer().registerFontFile(family, path, variant);
            }
            return true;
        });

    registerFunction("gui", "__gui_font_register_url",
        [](const std::vector<std::any>& args) -> std::any {
            std::string family = std::any_cast<std::string>(args[0]);
            std::string url = std::any_cast<std::string>(args[1]);
            std::string variant = args.size() > 2 ? std::any_cast<std::string>(args[2]) : "regular";
            if (currentApp) {
                return currentApp->getRenderer().registerFontURL(family, url, variant);
            }
            return false;
        });

    registerFunction("gui", "__gui_font_set_default",
        [](const std::vector<std::any>& args) -> std::any {
            std::string family = std::any_cast<std::string>(args[0]);
            if (currentApp) {
                currentApp->getRenderer().setDefaultFontFamily(family);
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

    registerFunction("gui", "__gui_padding_create",
        [](const std::vector<std::any>& args) -> std::any {
            float all = args.empty() ? 0.0f :
                        static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::Padding>(gui::EdgeInsets::all(all));
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
    // New widgets: TextArea, RadioButton, Dropdown, Icon, Drawer, TabBar, Menu
    // ========================================

    // TextArea
    registerFunction("gui", "__gui_textarea_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::TextArea>();
            if (!args.empty()) {
                widget->setPlaceholder(std::any_cast<std::string>(args[0]));
            }
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_textarea_get_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::TextArea>(getWidget(id));
            if (widget) return widget->getValue();
            return std::string("");
        });

    registerFunction("gui", "__gui_textarea_set_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string value = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::TextArea>(getWidget(id));
            if (widget) widget->setValue(value);
            return true;
        });

    // RadioButton
    registerFunction("gui", "__gui_radiobutton_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = args.size() > 0 ? std::any_cast<std::string>(args[0]) : "";
            std::string group = args.size() > 1 ? std::any_cast<std::string>(args[1]) : "default";
            auto widget = std::make_shared<gui::RadioButton>(label, group);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_radiobutton_is_selected",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::RadioButton>(getWidget(id));
            return widget ? widget->isSelected() : false;
        });

    // Dropdown
    registerFunction("gui", "__gui_dropdown_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Dropdown>();
            if (!args.empty()) {
                widget->setPlaceholder(std::any_cast<std::string>(args[0]));
            }
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_dropdown_add_item",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string item = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Dropdown>(getWidget(id));
            if (widget) widget->addItem(item);
            return true;
        });

    registerFunction("gui", "__gui_dropdown_get_selected",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Dropdown>(getWidget(id));
            return widget ? widget->getSelectedItem() : std::string("");
        });

    registerFunction("gui", "__gui_dropdown_get_selected_index",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Dropdown>(getWidget(id));
            return widget ? widget->getSelectedIndex() : -1;
        });

    registerFunction("gui", "__gui_dropdown_set_selected",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int idx = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Dropdown>(getWidget(id));
            if (widget) widget->setSelectedIndex(idx);
            return true;
        });

    // Icon
    registerFunction("gui", "__gui_icon_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string icon = std::any_cast<std::string>(args[0]);
            float size = args.size() > 1 ? static_cast<float>(std::any_cast<double>(args[1])) : 24.0f;
            auto widget = std::make_shared<gui::Icon>(icon, size);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_icon_set_color",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int r = std::any_cast<int>(args[1]);
            int g = std::any_cast<int>(args[2]);
            int b = std::any_cast<int>(args[3]);
            auto widget = std::dynamic_pointer_cast<gui::Icon>(getWidget(id));
            if (widget) widget->setColor(gui::Color::rgb(r, g, b));
            return true;
        });

    // Drawer
    registerFunction("gui", "__gui_drawer_create",
        [](const std::vector<std::any>& args) -> std::any {
            float width = args.empty() ? 280.0f : static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::Drawer>(width);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_drawer_open",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool open = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Drawer>(getWidget(id));
            if (widget) widget->setOpen(open);
            return true;
        });

    registerFunction("gui", "__gui_drawer_is_open",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Drawer>(getWidget(id));
            return widget ? widget->isOpen() : false;
        });

    // TabBar
    registerFunction("gui", "__gui_tabbar_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::TabBar>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_tabbar_add_tab",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string label = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::TabBar>(getWidget(id));
            if (widget) widget->addTab(label);
            return true;
        });

    registerFunction("gui", "__gui_tabbar_get_active",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::TabBar>(getWidget(id));
            return widget ? widget->getActiveTab() : 0;
        });

    registerFunction("gui", "__gui_tabbar_set_active",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int idx = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::TabBar>(getWidget(id));
            if (widget) widget->setActiveTab(idx);
            return true;
        });

    // Menu
    registerFunction("gui", "__gui_menu_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Menu>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_menu_add_item",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string label = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Menu>(getWidget(id));
            if (widget) {
                gui::MenuItem item;
                item.label = label;
                widget->addItem(item);
            }
            return true;
        });

    registerFunction("gui", "__gui_menu_add_separator",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Menu>(getWidget(id));
            if (widget) widget->addSeparator();
            return true;
        });

    registerFunction("gui", "__gui_menu_show",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float x = static_cast<float>(std::any_cast<double>(args[1]));
            float y = static_cast<float>(std::any_cast<double>(args[2]));
            auto widget = std::dynamic_pointer_cast<gui::Menu>(getWidget(id));
            if (widget) widget->show(x, y);
            return true;
        });

    registerFunction("gui", "__gui_menu_hide",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Menu>(getWidget(id));
            if (widget) widget->hide();
            return true;
        });

    // ========================================
    // Event handler registration (callback bridge)
    // ========================================

    // onClick — accepts a closure and executes it via the interpreter on click
    registerFunction("gui", "__gui_widget_set_onclick",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnClick([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return widgetId;
        });

    // onChange (string value) — for TextField, TextArea, Dropdown
    registerFunction("gui", "__gui_widget_set_onchange",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (!widget || !guiInterpreter_) return widgetId;
            auto interpPtr = guiInterpreter_;

            auto invokeCallback = [closure, interpPtr](const std::string& val) {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> cbArgs;
                cbArgs.push_back(RuntimeValue(val));
                interpPtr->executeCallback(rv, cbArgs);
            };

            auto textField = std::dynamic_pointer_cast<gui::TextField>(widget);
            if (textField) {
                textField->setOnChange(invokeCallback);
            }

            auto textArea = std::dynamic_pointer_cast<gui::TextArea>(widget);
            if (textArea) {
                textArea->setOnChange(invokeCallback);
            }

            auto dropdown = std::dynamic_pointer_cast<gui::Dropdown>(widget);
            if (dropdown) {
                dropdown->setOnChange(invokeCallback);
            }

            return widgetId;
        });

    // onSubmit — for TextField
    registerFunction("gui", "__gui_widget_set_onsubmit",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                auto textField = std::dynamic_pointer_cast<gui::TextField>(widget);
                if (textField) {
                    textField->setOnSubmit([closure, interpPtr]() {
                        RuntimeValue rv(closure, "function");
                        std::vector<RuntimeValue> noArgs;
                        interpPtr->executeCallback(rv, noArgs);
                    });
                }
            }
            return widgetId;
        });

    // onMouseEnter
    registerFunction("gui", "__gui_widget_set_onmouseenter",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnMouseEnter([closure, interpPtr](float x, float y) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return widgetId;
        });

    // onMouseLeave
    registerFunction("gui", "__gui_widget_set_onmouseleave",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnMouseLeave([closure, interpPtr](float x, float y) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return widgetId;
        });

    // onKeyPress
    registerFunction("gui", "__gui_widget_set_onkeypress",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnKeyPress([closure, interpPtr](gui::KeyCode key, gui::KeyModifiers mods) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> cbArgs;
                    cbArgs.push_back(RuntimeValue(static_cast<int>(key)));
                    interpPtr->executeCallback(rv, cbArgs);
                });
            }
            return widgetId;
        });

    // ========================================
    // State management
    // ========================================

    registerFunction("gui", "__gui_state_create",
        [](const std::vector<std::any>& args) -> std::any {
            int id = nextStateId++;
            stateValues[id] = args.empty() ? std::any() : args[0];
            stateListeners[id] = {};
            return id;
        });

    registerFunction("gui", "__gui_state_get",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto it = stateValues.find(id);
            if (it != stateValues.end()) return it->second;
            return std::any();
        });

    registerFunction("gui", "__gui_state_set",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            stateValues[id] = args[1];
            // Notify listeners
            auto lit = stateListeners.find(id);
            if (lit != stateListeners.end()) {
                for (auto& listener : lit->second) {
                    listener();
                }
            }
            // Trigger signal text effects
            auto eit = signalEffects.find(id);
            if (eit != signalEffects.end() && guiInterpreter_) {
                for (auto& [widgetId, effectClosure] : eit->second) {
                    std::vector<RuntimeValue> noArgs;
                    RuntimeValue newText = guiInterpreter_->executeCallback(effectClosure, noArgs);
                    // Update widget text
                    auto widget = getWidget(widgetId);
                    if (widget) {
                        auto textWidget = std::dynamic_pointer_cast<gui::Text>(widget);
                        if (textWidget) {
                            textWidget->setText(newText.asString());
                        }
                    }
                }
            }
            // Request redraw
            if (currentApp) currentApp->requestRedraw();
            return true;
        });

    registerFunction("gui", "__gui_state_watch",
        [](const std::vector<std::any>& args) -> std::any {
            int stateId = std::any_cast<int>(args[0]);
            int callbackId = std::any_cast<int>(args[1]);
            auto lit = stateListeners.find(stateId);
            if (lit != stateListeners.end()) {
                lit->second.push_back([]() {
                    // Interpreter will dispatch this callback
                });
            }
            return true;
        });

    // ========================================
    // Signal-aware event handlers
    // ========================================

    // Store and invoke Stratos closures for click events
    registerFunction("gui", "__gui_set_click_closure",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1]; // Keep as std::any (shared_ptr<Closure>)
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto closureCopy = closure;
                auto interpPtr = guiInterpreter_;
                widget->setOnClick([closureCopy, interpPtr]() {
                    RuntimeValue rv(closureCopy, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return true;
        });

    // Store and invoke Stratos closures for change events
    registerFunction("gui", "__gui_set_change_closure",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = getWidget(widgetId);
            if (widget && guiInterpreter_) {
                auto closureCopy = closure;
                auto interpPtr = guiInterpreter_;
                auto tf = std::dynamic_pointer_cast<gui::TextField>(widget);
                if (tf) {
                    tf->setOnChange([closureCopy, interpPtr](const std::string& val) {
                        RuntimeValue rv(closureCopy, "function");
                        std::vector<RuntimeValue> callArgs = { RuntimeValue(val) };
                        interpPtr->executeCallback(rv, callArgs);
                    });
                }
            }
            return true;
        });

    // Bind a text-update effect to a signal
    registerFunction("gui", "__gui_signal_bind_text",
        [](const std::vector<std::any>& args) -> std::any {
            int signalId = std::any_cast<int>(args[0]);
            int widgetId = std::any_cast<int>(args[1]);
            auto closure = args[2]; // format closure
            signalEffects[signalId].push_back({widgetId, RuntimeValue(closure, "function")});
            return true;
        });

    // ========================================
    // Signal system (Phase 5 — advanced reactivity)
    // ========================================

    registerFunction("gui", "__gui_signal_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto& reg = gui::SignalRegistry::instance();
            return reg.createSignal(args[0]);
        });

    registerFunction("gui", "__gui_signal_get",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto& reg = gui::SignalRegistry::instance();
            return reg.getSignal(id);
        });

    registerFunction("gui", "__gui_signal_set",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto& reg = gui::SignalRegistry::instance();
            reg.setSignal(id, args[1]);
            return true;
        });

    registerFunction("gui", "__gui_signal_peek",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto& reg = gui::SignalRegistry::instance();
            return reg.peekSignal(id);
        });

    registerFunction("gui", "__gui_effect_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto closure = args[0];
            if (!guiInterpreter_) return -1;
            auto interpPtr = guiInterpreter_;
            auto& reg = gui::SignalRegistry::instance();
            int id = reg.createEffect([closure, interpPtr]() {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> noArgs;
                interpPtr->executeCallback(rv, noArgs);
            });
            return id;
        });

    registerFunction("gui", "__gui_effect_dispose",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto& reg = gui::SignalRegistry::instance();
            reg.disposeEffect(id);
            return true;
        });

    registerFunction("gui", "__gui_computed_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto closure = args[0];
            if (!guiInterpreter_) return -1;
            auto interpPtr = guiInterpreter_;
            auto& reg = gui::SignalRegistry::instance();
            int id = reg.createComputed([closure, interpPtr]() -> std::any {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> noArgs;
                RuntimeValue result = interpPtr->executeCallback(rv, noArgs);
                // Convert RuntimeValue back to std::any for signal storage
                if (result.type == "int") return std::any(std::get<int>(result.value));
                if (result.type == "double") return std::any(std::get<double>(result.value));
                if (result.type == "string") return std::any(std::get<std::string>(result.value));
                if (result.type == "bool") return std::any(std::get<bool>(result.value));
                return std::any{};
            });
            return id;
        });

    registerFunction("gui", "__gui_computed_get",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto& reg = gui::SignalRegistry::instance();
            return reg.getComputed(id);
        });

    registerFunction("gui", "__gui_batch",
        [](const std::vector<std::any>& args) -> std::any {
            auto closure = args[0];
            if (!guiInterpreter_) return false;
            auto interpPtr = guiInterpreter_;
            auto& ctx = gui::TrackingContext::instance();
            ctx.beginBatch();
            RuntimeValue rv(closure, "function");
            std::vector<RuntimeValue> noArgs;
            interpPtr->executeCallback(rv, noArgs);
            ctx.endBatch();
            return true;
        });

    registerFunction("gui", "__gui_untrack",
        [](const std::vector<std::any>& args) -> std::any {
            auto closure = args[0];
            if (!guiInterpreter_) return std::any{};
            auto interpPtr = guiInterpreter_;
            auto& ctx = gui::TrackingContext::instance();
            ctx.pushUntrack();
            RuntimeValue rv(closure, "function");
            std::vector<RuntimeValue> noArgs;
            RuntimeValue result = interpPtr->executeCallback(rv, noArgs);
            ctx.popUntrack();
            // Convert result
            if (result.type == "int") return std::any(std::get<int>(result.value));
            if (result.type == "double") return std::any(std::get<double>(result.value));
            if (result.type == "string") return std::any(std::get<std::string>(result.value));
            if (result.type == "bool") return std::any(std::get<bool>(result.value));
            return std::any{};
        });

    registerFunction("gui", "__gui_on_cleanup",
        [](const std::vector<std::any>& args) -> std::any {
            auto closure = args[0];
            if (!guiInterpreter_) return false;
            auto interpPtr = guiInterpreter_;
            auto& ctx = gui::TrackingContext::instance();
            gui::Effect* current = ctx.getCurrentEffect();
            if (current) {
                current->addCleanup([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
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
            clickCallbacks.clear();
            changeCallbacks.clear();
            boolChangeCallbacks.clear();
            floatChangeCallbacks.clear();
            mouseCallbacks.clear();
            keyCallbacks.clear();
            stateValues.clear();
            stateListeners.clear();
            signalEffects.clear();
            gui::SignalRegistry::instance().clear();
            guiInterpreter_ = nullptr;
            nextStateId = 1;
            nextCallbackId = 1;
            currentApp.reset();
            return true;
        });
}

} // namespace stratos
