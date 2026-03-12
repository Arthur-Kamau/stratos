#include "stratos/NativeRegistry.h"
#include "stratos/Interpreter.h"
#include "stratos/gui/App.h"
#include "stratos/gui/Widget.h"
#include "stratos/gui/Renderer.h"
#include "stratos/gui/Signals.h"
#include "stratos/gui/Router.h"
#include "stratos/gui/Store.h"
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
            gui::LifecycleRegistry::instance().clear();
            gui::MediaQueryRegistry::instance().clear();
            gui::AnimationManager::instance().clear();
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
    // Router
    // ========================================

    registerFunction("gui", "__gui_router_create",
        [](const std::vector<std::any>& args) -> std::any {
            return gui::RouterRegistry::instance().createRouter();
        });

    registerFunction("gui", "__gui_router_add_route",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto path = std::any_cast<std::string>(args[1]);
            auto builder = args[2]; // closure
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            router->addRoute(path, builder);
            return true;
        });

    registerFunction("gui", "__gui_router_add_guard",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto path = std::any_cast<std::string>(args[1]);
            auto guardFn = args[2]; // closure
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            router->addGuard(path, guardFn);
            return true;
        });

    registerFunction("gui", "__gui_router_set_not_found",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto builder = args[1]; // closure
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            router->setNotFound(builder);
            return true;
        });

    registerFunction("gui", "__gui_router_navigate",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto path = std::any_cast<std::string>(args[1]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;

            bool matched = router->navigate(path);
            if (!matched) return false;

            // Execute the matched route's builder to get the widget tree
            if (!guiInterpreter_) return false;
            auto& builder = router->getCurrentBuilder();
            if (!builder.has_value()) {
                // Check notFound
                if (router->hasNotFoundBuilder()) {
                    auto& nfBuilder = router->getNotFoundBuilder();
                    RuntimeValue rv(nfBuilder, "function");
                    std::vector<RuntimeValue> noArgs;
                    RuntimeValue result = guiInterpreter_->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = std::get<int>(result.value);
                        router->setCurrentWidgetId(widgetId);

                        // If there's an app, swap the root widget
                        if (currentApp) {
                            auto it = widgetRegistry.find(widgetId);
                            if (it != widgetRegistry.end()) {
                                currentApp->setRoot(it->second);
                            }
                        }
                    }
                }
                return true;
            }

            RuntimeValue rv(builder, "function");
            std::vector<RuntimeValue> noArgs;
            RuntimeValue result = guiInterpreter_->executeCallback(rv, noArgs);
            if (result.type == "int") {
                int widgetId = std::get<int>(result.value);
                router->setCurrentWidgetId(widgetId);

                // If there's an app, swap the root widget
                if (currentApp) {
                    auto it = widgetRegistry.find(widgetId);
                    if (it != widgetRegistry.end()) {
                        currentApp->setRoot(it->second);
                    }
                }
            }
            return true;
        });

    registerFunction("gui", "__gui_router_back",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            bool ok = router->back();
            if (ok && guiInterpreter_) {
                auto& builder = router->getCurrentBuilder();
                if (builder.has_value()) {
                    RuntimeValue rv(builder, "function");
                    std::vector<RuntimeValue> noArgs;
                    RuntimeValue result = guiInterpreter_->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = std::get<int>(result.value);
                        router->setCurrentWidgetId(widgetId);
                        if (currentApp) {
                            auto it = widgetRegistry.find(widgetId);
                            if (it != widgetRegistry.end()) {
                                currentApp->setRoot(it->second);
                            }
                        }
                    }
                }
            }
            return ok;
        });

    registerFunction("gui", "__gui_router_forward",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            bool ok = router->forward();
            if (ok && guiInterpreter_) {
                auto& builder = router->getCurrentBuilder();
                if (builder.has_value()) {
                    RuntimeValue rv(builder, "function");
                    std::vector<RuntimeValue> noArgs;
                    RuntimeValue result = guiInterpreter_->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = std::get<int>(result.value);
                        router->setCurrentWidgetId(widgetId);
                        if (currentApp) {
                            auto it = widgetRegistry.find(widgetId);
                            if (it != widgetRegistry.end()) {
                                currentApp->setRoot(it->second);
                            }
                        }
                    }
                }
            }
            return ok;
        });

    registerFunction("gui", "__gui_router_replace",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto path = std::any_cast<std::string>(args[1]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return false;
            return router->replace(path);
        });

    registerFunction("gui", "__gui_router_get_param",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto name = std::any_cast<std::string>(args[1]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return std::string("");
            return router->getParam(name);
        });

    registerFunction("gui", "__gui_router_get_query",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto name = std::any_cast<std::string>(args[1]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return std::string("");
            return router->getQuery(name);
        });

    registerFunction("gui", "__gui_router_get_path",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return std::string("");
            return router->currentPath();
        });

    registerFunction("gui", "__gui_router_get_widget",
        [](const std::vector<std::any>& args) -> std::any {
            int routerId = std::any_cast<int>(args[0]);
            auto* router = gui::RouterRegistry::instance().getRouter(routerId);
            if (!router) return -1;
            return router->getCurrentWidgetId();
        });

    // ========================================
    // ========================================
    // Store & Context
    // ========================================

    registerFunction("gui", "__gui_store_create",
        [](const std::vector<std::any>& args) -> std::any {
            return gui::StoreRegistry::instance().createStore();
        });

    registerFunction("gui", "__gui_store_get",
        [](const std::vector<std::any>& args) -> std::any {
            int storeId = std::any_cast<int>(args[0]);
            std::string key = std::any_cast<std::string>(args[1]);
            auto* store = gui::StoreRegistry::instance().getStore(storeId);
            if (!store) return std::any{};
            return store->get(key);
        });

    registerFunction("gui", "__gui_store_set",
        [](const std::vector<std::any>& args) -> std::any {
            int storeId = std::any_cast<int>(args[0]);
            std::string key = std::any_cast<std::string>(args[1]);
            std::any value = args[2];
            auto* store = gui::StoreRegistry::instance().getStore(storeId);
            if (!store) return false;
            store->set(key, std::move(value));
            return true;
        });

    registerFunction("gui", "__gui_store_subscribe",
        [](const std::vector<std::any>& args) -> std::any {
            int storeId = std::any_cast<int>(args[0]);
            std::string key = std::any_cast<std::string>(args[1]);
            std::any closure = args[2];
            auto* store = gui::StoreRegistry::instance().getStore(storeId);
            if (!store) return -1;

            auto* interp = guiInterpreter_;
            int subId = store->subscribe(key, [closure, interp](std::any newValue) {
                if (interp) {
                    RuntimeValue rv(closure, "function");
                    RuntimeValue argVal(newValue, "any");
                    std::vector<RuntimeValue> cbArgs = {argVal};
                    interp->executeCallback(rv, cbArgs);
                }
            });
            return subId;
        });

    registerFunction("gui", "__gui_store_unsubscribe",
        [](const std::vector<std::any>& args) -> std::any {
            int subscriptionId = std::any_cast<int>(args[0]);
            // Search all stores for this subscription
            // Since subscription IDs are globally unique per store, iterate stores
            // For simplicity, we store a global mapping
            // Actually, subscription IDs are per-store, so we need to search
            // We'll iterate all stores
            auto& registry = gui::StoreRegistry::instance();
            // Try to unsubscribe from each store until found
            // This is O(n) but fine for typical usage
            for (int i = 0; ; i++) {
                auto* store = registry.getStore(i);
                if (!store) break;
                if (store->unsubscribe(subscriptionId)) return true;
            }
            return false;
        });

    registerFunction("gui", "__gui_context_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string name = std::any_cast<std::string>(args[0]);
            std::any defaultValue = args[1];
            return gui::ContextRegistry::instance().createContext(name, std::move(defaultValue));
        });

    registerFunction("gui", "__gui_context_provide",
        [](const std::vector<std::any>& args) -> std::any {
            int contextId = std::any_cast<int>(args[0]);
            std::any value = args[1];
            return gui::ContextRegistry::instance().provide(contextId, std::move(value));
        });

    registerFunction("gui", "__gui_context_consume",
        [](const std::vector<std::any>& args) -> std::any {
            int contextId = std::any_cast<int>(args[0]);
            return gui::ContextRegistry::instance().consume(contextId);
        });

    // ========================================
    // Phase 8 — Lifecycle Hooks
    // ========================================

    registerFunction("gui", "__gui_lifecycle_on_mount",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            if (!guiInterpreter_) return false;
            auto interpPtr = guiInterpreter_;
            auto widget = getWidget(widgetId);
            if (!widget) return false;
            gui::LifecycleRegistry::instance().onMount(widget->getId(), [closure, interpPtr]() {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> noArgs;
                interpPtr->executeCallback(rv, noArgs);
            });
            return true;
        });

    registerFunction("gui", "__gui_lifecycle_on_destroy",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            if (!guiInterpreter_) return false;
            auto interpPtr = guiInterpreter_;
            auto widget = getWidget(widgetId);
            if (!widget) return false;
            gui::LifecycleRegistry::instance().onDestroy(widget->getId(), [closure, interpPtr]() {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> noArgs;
                interpPtr->executeCallback(rv, noArgs);
            });
            return true;
        });

    registerFunction("gui", "__gui_lifecycle_on_update",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto closure = args[1];
            if (!guiInterpreter_) return false;
            auto interpPtr = guiInterpreter_;
            auto widget = getWidget(widgetId);
            if (!widget) return false;
            gui::LifecycleRegistry::instance().onUpdate(widget->getId(), [closure, interpPtr]() {
                RuntimeValue rv(closure, "function");
                std::vector<RuntimeValue> noArgs;
                interpPtr->executeCallback(rv, noArgs);
            });
            return true;
        });

    registerFunction("gui", "__gui_lifecycle_trigger_mount",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto widget = getWidget(widgetId);
            if (widget) gui::LifecycleRegistry::instance().triggerMount(widget->getId());
            return true;
        });

    registerFunction("gui", "__gui_lifecycle_trigger_destroy",
        [](const std::vector<std::any>& args) -> std::any {
            int widgetId = std::any_cast<int>(args[0]);
            auto widget = getWidget(widgetId);
            if (widget) gui::LifecycleRegistry::instance().triggerDestroy(widget->getId());
            return true;
        });

    // ========================================
    // Phase 8 — New Widgets
    // ========================================

    // Card
    registerFunction("gui", "__gui_card_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Card>();
            if (!args.empty()) {
                float elevation = static_cast<float>(std::any_cast<double>(args[0]));
                widget->setElevation(elevation);
            }
            return storeWidget(widget);
        });

    // Divider
    registerFunction("gui", "__gui_divider_create",
        [](const std::vector<std::any>& args) -> std::any {
            bool vertical = !args.empty() && std::any_cast<bool>(args[0]);
            auto widget = std::make_shared<gui::Divider>(vertical);
            if (args.size() > 1) {
                widget->setThickness(static_cast<float>(std::any_cast<double>(args[1])));
            }
            return storeWidget(widget);
        });

    // Badge
    registerFunction("gui", "__gui_badge_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = args.empty() ? "" : std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Badge>(label);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_badge_set_label",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string label = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Badge>(getWidget(id));
            if (widget) widget->setLabel(label);
            return true;
        });

    // Tooltip
    registerFunction("gui", "__gui_tooltip_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string message = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Tooltip>(message);
            return storeWidget(widget);
        });

    // Chip
    registerFunction("gui", "__gui_chip_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::Chip>(label);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_chip_set_selected",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool selected = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Chip>(getWidget(id));
            if (widget) widget->setSelected(selected);
            return true;
        });

    registerFunction("gui", "__gui_chip_is_selected",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::Chip>(getWidget(id));
            return widget ? widget->isSelected() : false;
        });

    // FAB
    registerFunction("gui", "__gui_fab_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string icon = args.empty() ? "+" : std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::FAB>(icon);
            return storeWidget(widget);
        });

    // ProgressBar
    registerFunction("gui", "__gui_progressbar_create",
        [](const std::vector<std::any>& args) -> std::any {
            float value = args.empty() ? 0.0f : static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::ProgressBar>();
            widget->setValue(value);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_progressbar_set_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float value = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::dynamic_pointer_cast<gui::ProgressBar>(getWidget(id));
            if (widget) {
                widget->setValue(value);
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
        });

    registerFunction("gui", "__gui_progressbar_set_indeterminate",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool indeterminate = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::ProgressBar>(getWidget(id));
            if (widget) widget->setIndeterminate(indeterminate);
            return true;
        });

    // CircularProgress
    registerFunction("gui", "__gui_circular_progress_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::CircularProgress>();
            if (!args.empty()) {
                float value = static_cast<float>(std::any_cast<double>(args[0]));
                widget->setValue(value);
                widget->setIndeterminate(false);
            }
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_circular_progress_set_value",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float value = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::dynamic_pointer_cast<gui::CircularProgress>(getWidget(id));
            if (widget) {
                widget->setValue(value);
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
        });

    // SnackBar
    registerFunction("gui", "__gui_snackbar_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string message = args.empty() ? "" : std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::SnackBar>(message);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_snackbar_show",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::SnackBar>(getWidget(id));
            if (widget) {
                widget->show();
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
        });

    registerFunction("gui", "__gui_snackbar_hide",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::SnackBar>(getWidget(id));
            if (widget) widget->hide();
            return true;
        });

    registerFunction("gui", "__gui_snackbar_set_message",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string message = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::SnackBar>(getWidget(id));
            if (widget) widget->setMessage(message);
            return true;
        });

    // Wrap
    registerFunction("gui", "__gui_wrap_create",
        [](const std::vector<std::any>& args) -> std::any {
            float spacing = args.empty() ? 8.0f : static_cast<float>(std::any_cast<double>(args[0]));
            float runSpacing = args.size() > 1 ? static_cast<float>(std::any_cast<double>(args[1])) : 8.0f;
            auto widget = std::make_shared<gui::Wrap>(spacing, runSpacing);
            return storeWidget(widget);
        });

    // ExpansionPanel
    registerFunction("gui", "__gui_expansion_panel_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string title = std::any_cast<std::string>(args[0]);
            auto widget = std::make_shared<gui::ExpansionPanel>(title);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_expansion_panel_set_expanded",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool expanded = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::ExpansionPanel>(getWidget(id));
            if (widget) widget->setExpanded(expanded);
            return true;
        });

    registerFunction("gui", "__gui_expansion_panel_is_expanded",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::ExpansionPanel>(getWidget(id));
            return widget ? widget->isExpanded() : false;
        });

    // Scaffold
    registerFunction("gui", "__gui_scaffold_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Scaffold>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_scaffold_set_appbar",
        [](const std::vector<std::any>& args) -> std::any {
            int scaffoldId = std::any_cast<int>(args[0]);
            int appbarId = std::any_cast<int>(args[1]);
            auto scaffold = std::dynamic_pointer_cast<gui::Scaffold>(getWidget(scaffoldId));
            auto appbar = getWidget(appbarId);
            if (scaffold && appbar) scaffold->setAppBar(appbar);
            return true;
        });

    registerFunction("gui", "__gui_scaffold_set_body",
        [](const std::vector<std::any>& args) -> std::any {
            int scaffoldId = std::any_cast<int>(args[0]);
            int bodyId = std::any_cast<int>(args[1]);
            auto scaffold = std::dynamic_pointer_cast<gui::Scaffold>(getWidget(scaffoldId));
            auto body = getWidget(bodyId);
            if (scaffold && body) scaffold->setBody(body);
            return true;
        });

    registerFunction("gui", "__gui_scaffold_set_fab",
        [](const std::vector<std::any>& args) -> std::any {
            int scaffoldId = std::any_cast<int>(args[0]);
            int fabId = std::any_cast<int>(args[1]);
            auto scaffold = std::dynamic_pointer_cast<gui::Scaffold>(getWidget(scaffoldId));
            auto fab = getWidget(fabId);
            if (scaffold && fab) scaffold->setFab(fab);
            return true;
        });

    // BottomNavBar
    registerFunction("gui", "__gui_bottomnav_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::BottomNavBar>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_bottomnav_add_item",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string icon = std::any_cast<std::string>(args[1]);
            std::string label = std::any_cast<std::string>(args[2]);
            auto widget = std::dynamic_pointer_cast<gui::BottomNavBar>(getWidget(id));
            if (widget) widget->addItem(icon, label);
            return true;
        });

    registerFunction("gui", "__gui_bottomnav_get_active",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::BottomNavBar>(getWidget(id));
            return widget ? widget->getActiveIndex() : 0;
        });

    registerFunction("gui", "__gui_bottomnav_set_active",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int idx = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::BottomNavBar>(getWidget(id));
            if (widget) widget->setActiveIndex(idx);
            return true;
        });

    // ========================================
    // Form system
    // ========================================

    registerFunction("gui", "__gui_form_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Form>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_form_add_field",
        [](const std::vector<std::any>& args) -> std::any {
            int formId = std::any_cast<int>(args[0]);
            std::string name = std::any_cast<std::string>(args[1]);
            int fieldId = std::any_cast<int>(args[2]);
            auto form = std::dynamic_pointer_cast<gui::Form>(getWidget(formId));
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(fieldId));
            if (form && field) {
                form->addField(name, field);
            }
            return true;
        });

    registerFunction("gui", "__gui_form_validate",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto form = std::dynamic_pointer_cast<gui::Form>(getWidget(id));
            if (form) return form->validate();
            return false;
        });

    registerFunction("gui", "__gui_form_reset",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto form = std::dynamic_pointer_cast<gui::Form>(getWidget(id));
            if (form) { form->reset(); return true; }
            return false;
        });

    registerFunction("gui", "__gui_form_submit",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto form = std::dynamic_pointer_cast<gui::Form>(getWidget(id));
            if (form) { form->submit(); return true; }
            return false;
        });

    registerFunction("gui", "__gui_form_set_onsubmit",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto form = std::dynamic_pointer_cast<gui::Form>(getWidget(id));
            if (form && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                form->setOnSubmit([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_formfield_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string label = std::any_cast<std::string>(args[0]);
            int inputId = std::any_cast<int>(args[1]);
            auto input = getWidget(inputId);
            auto widget = std::make_shared<gui::FormField>(label, input);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_formfield_add_required",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string msg = std::any_cast<std::string>(args[1]);
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(id));
            if (field) {
                gui::ValidationEntry entry;
                entry.rule = gui::ValidationRule::Required;
                entry.errorMessage = msg;
                field->addValidation(entry);
            }
            return true;
        });

    registerFunction("gui", "__gui_formfield_add_min_length",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int minLen = std::any_cast<int>(args[1]);
            std::string msg = std::any_cast<std::string>(args[2]);
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(id));
            if (field) {
                gui::ValidationEntry entry;
                entry.rule = gui::ValidationRule::MinLength;
                entry.intParam = minLen;
                entry.errorMessage = msg;
                field->addValidation(entry);
            }
            return true;
        });

    registerFunction("gui", "__gui_formfield_add_max_length",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int maxLen = std::any_cast<int>(args[1]);
            std::string msg = std::any_cast<std::string>(args[2]);
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(id));
            if (field) {
                gui::ValidationEntry entry;
                entry.rule = gui::ValidationRule::MaxLength;
                entry.intParam = maxLen;
                entry.errorMessage = msg;
                field->addValidation(entry);
            }
            return true;
        });

    registerFunction("gui", "__gui_formfield_add_email",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string msg = std::any_cast<std::string>(args[1]);
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(id));
            if (field) {
                gui::ValidationEntry entry;
                entry.rule = gui::ValidationRule::Email;
                entry.errorMessage = msg;
                field->addValidation(entry);
            }
            return true;
        });

    registerFunction("gui", "__gui_formfield_get_error",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto field = std::dynamic_pointer_cast<gui::FormField>(getWidget(id));
            if (field) return field->getError();
            return std::string("");
        });

    // ========================================
    // Phase 8.2 — Gesture Detection
    // ========================================

    registerFunction("gui", "__gui_gesture_detector_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::GestureDetector>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_gesture_set_on_double_tap",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnDoubleTap([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_gesture_set_on_long_press",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnLongPress([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_gesture_set_on_drag_start",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnDragStart([closure, interpPtr](float x, float y) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> cbArgs;
                    cbArgs.push_back(RuntimeValue(static_cast<double>(x), "float"));
                    cbArgs.push_back(RuntimeValue(static_cast<double>(y), "float"));
                    interpPtr->executeCallback(rv, cbArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_gesture_set_on_drag",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnDrag([closure, interpPtr](float x, float y, float dx, float dy) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> cbArgs;
                    cbArgs.push_back(RuntimeValue(static_cast<double>(x), "float"));
                    cbArgs.push_back(RuntimeValue(static_cast<double>(y), "float"));
                    cbArgs.push_back(RuntimeValue(static_cast<double>(dx), "float"));
                    cbArgs.push_back(RuntimeValue(static_cast<double>(dy), "float"));
                    interpPtr->executeCallback(rv, cbArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_gesture_set_on_drag_end",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnDragEnd([closure, interpPtr]() {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    interpPtr->executeCallback(rv, noArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_gesture_set_on_swipe",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::GestureDetector>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnSwipe([closure, interpPtr](gui::SwipeDirection dir) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> cbArgs;
                    std::string dirStr;
                    switch (dir) {
                        case gui::SwipeDirection::Left: dirStr = "left"; break;
                        case gui::SwipeDirection::Right: dirStr = "right"; break;
                        case gui::SwipeDirection::Up: dirStr = "up"; break;
                        case gui::SwipeDirection::Down: dirStr = "down"; break;
                    }
                    cbArgs.push_back(RuntimeValue(dirStr, "string"));
                    interpPtr->executeCallback(rv, cbArgs);
                });
            }
            return true;
        });

    // ========================================
    // Phase 8.4 — Error Boundary
    // ========================================

    registerFunction("gui", "__gui_error_boundary_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::ErrorBoundary>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_error_boundary_set_fallback",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int fallbackId = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::ErrorBoundary>(getWidget(id));
            auto fallback = getWidget(fallbackId);
            if (widget && fallback) widget->setFallback(fallback);
            return true;
        });

    registerFunction("gui", "__gui_error_boundary_set_on_error",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::ErrorBoundary>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setOnError([closure, interpPtr](const std::string& msg) {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> cbArgs;
                    cbArgs.push_back(RuntimeValue(msg, "string"));
                    interpPtr->executeCallback(rv, cbArgs);
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_error_boundary_set_error",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string msg = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::ErrorBoundary>(getWidget(id));
            if (widget) widget->setError(msg);
            return true;
        });

    registerFunction("gui", "__gui_error_boundary_reset",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::ErrorBoundary>(getWidget(id));
            if (widget) widget->reset();
            return true;
        });

    registerFunction("gui", "__gui_error_boundary_has_error",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::ErrorBoundary>(getWidget(id));
            return widget ? widget->hasError() : false;
        });

    // ========================================
    // Phase 8.5 — MediaQuery / Responsive
    // ========================================

    registerFunction("gui", "__gui_media_query_width",
        [](const std::vector<std::any>& args) -> std::any {
            return static_cast<double>(gui::MediaQueryRegistry::instance().get().width);
        });

    registerFunction("gui", "__gui_media_query_height",
        [](const std::vector<std::any>& args) -> std::any {
            return static_cast<double>(gui::MediaQueryRegistry::instance().get().height);
        });

    registerFunction("gui", "__gui_media_query_orientation",
        [](const std::vector<std::any>& args) -> std::any {
            auto o = gui::MediaQueryRegistry::instance().get().orientation;
            return std::string(o == gui::Orientation::Portrait ? "portrait" : "landscape");
        });

    registerFunction("gui", "__gui_media_query_breakpoint",
        [](const std::vector<std::any>& args) -> std::any {
            auto bp = gui::MediaQueryRegistry::instance().get().breakpoint;
            switch (bp) {
                case gui::Breakpoint::Mobile: return std::string("mobile");
                case gui::Breakpoint::Tablet: return std::string("tablet");
                case gui::Breakpoint::Desktop: return std::string("desktop");
            }
            return std::string("desktop");
        });

    registerFunction("gui", "__gui_responsive_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Responsive>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_responsive_set_mobile",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::Responsive>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setMobileBuilder([closure, interpPtr]() -> gui::WidgetPtr {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    auto result = interpPtr->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = result.asInt();
                        return getWidget(widgetId);
                    }
                    return nullptr;
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_responsive_set_tablet",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::Responsive>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setTabletBuilder([closure, interpPtr]() -> gui::WidgetPtr {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    auto result = interpPtr->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = result.asInt();
                        return getWidget(widgetId);
                    }
                    return nullptr;
                });
            }
            return true;
        });

    registerFunction("gui", "__gui_responsive_set_desktop",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto closure = args[1];
            auto widget = std::dynamic_pointer_cast<gui::Responsive>(getWidget(id));
            if (widget && guiInterpreter_) {
                auto interpPtr = guiInterpreter_;
                widget->setDesktopBuilder([closure, interpPtr]() -> gui::WidgetPtr {
                    RuntimeValue rv(closure, "function");
                    std::vector<RuntimeValue> noArgs;
                    auto result = interpPtr->executeCallback(rv, noArgs);
                    if (result.type == "int") {
                        int widgetId = result.asInt();
                        return getWidget(widgetId);
                    }
                    return nullptr;
                });
            }
            return true;
        });

    // ========================================
    // Phase 9.2 — Async Widget Support
    // ========================================

    registerFunction("gui", "__gui_future_builder_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::FutureBuilder>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_future_builder_set_loading",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int loadingId = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::FutureBuilder>(getWidget(id));
            auto loading = getWidget(loadingId);
            if (widget && loading) widget->setLoadingWidget(loading);
            return true;
        });

    registerFunction("gui", "__gui_future_builder_set_success",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int successId = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::FutureBuilder>(getWidget(id));
            auto success = getWidget(successId);
            if (widget && success) widget->setSuccessWidget(success);
            return true;
        });

    registerFunction("gui", "__gui_future_builder_set_error",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int errorId = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::FutureBuilder>(getWidget(id));
            auto errorW = getWidget(errorId);
            if (widget && errorW) widget->setErrorWidget(errorW);
            return true;
        });

    registerFunction("gui", "__gui_future_builder_set_state",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            std::string state = std::any_cast<std::string>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::FutureBuilder>(getWidget(id));
            if (widget) {
                if (state == "loading") widget->setState(gui::AsyncState::Loading);
                else if (state == "success") widget->setState(gui::AsyncState::Success);
                else if (state == "error") widget->setState(gui::AsyncState::Error);
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
        });

    registerFunction("gui", "__gui_future_builder_get_state",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::FutureBuilder>(getWidget(id));
            if (widget) {
                switch (widget->getState()) {
                    case gui::AsyncState::Loading: return std::string("loading");
                    case gui::AsyncState::Success: return std::string("success");
                    case gui::AsyncState::Error: return std::string("error");
                }
            }
            return std::string("loading");
        });

    registerFunction("gui", "__gui_suspense_create",
        [](const std::vector<std::any>& args) -> std::any {
            auto widget = std::make_shared<gui::Suspense>();
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_suspense_set_fallback",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            int fallbackId = std::any_cast<int>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Suspense>(getWidget(id));
            auto fallback = getWidget(fallbackId);
            if (widget && fallback) widget->setFallback(fallback);
            return true;
        });

    registerFunction("gui", "__gui_suspense_set_ready",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            bool ready = std::any_cast<bool>(args[1]);
            auto widget = std::dynamic_pointer_cast<gui::Suspense>(getWidget(id));
            if (widget) {
                widget->setReady(ready);
                if (currentApp) currentApp->requestRedraw();
            }
            return true;
        });

    // ========================================
    // Phase 9.3 — Transition Animations
    // ========================================

    registerFunction("gui", "__gui_animated_container_create",
        [](const std::vector<std::any>& args) -> std::any {
            float duration = args.empty() ? 300.0f : static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::AnimatedContainer>(duration);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_animated_container_set_target_width",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float w = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::dynamic_pointer_cast<gui::AnimatedContainer>(getWidget(id));
            if (widget) widget->setTargetWidth(w);
            return true;
        });

    registerFunction("gui", "__gui_animated_container_set_target_height",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            float h = static_cast<float>(std::any_cast<double>(args[1]));
            auto widget = std::dynamic_pointer_cast<gui::AnimatedContainer>(getWidget(id));
            if (widget) widget->setTargetHeight(h);
            return true;
        });

    registerFunction("gui", "__gui_fade_transition_create",
        [](const std::vector<std::any>& args) -> std::any {
            float duration = args.empty() ? 300.0f : static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::FadeTransition>(duration);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_fade_transition_fade_in",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::FadeTransition>(getWidget(id));
            if (widget) widget->fadeIn();
            return true;
        });

    registerFunction("gui", "__gui_fade_transition_fade_out",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::FadeTransition>(getWidget(id));
            if (widget) widget->fadeOut();
            return true;
        });

    registerFunction("gui", "__gui_slide_transition_create",
        [](const std::vector<std::any>& args) -> std::any {
            std::string dir = args.empty() ? "left" : std::any_cast<std::string>(args[0]);
            float duration = args.size() > 1 ? static_cast<float>(std::any_cast<double>(args[1])) : 300.0f;
            gui::SlideTransition::Direction d = gui::SlideTransition::Direction::Left;
            if (dir == "right") d = gui::SlideTransition::Direction::Right;
            else if (dir == "up") d = gui::SlideTransition::Direction::Up;
            else if (dir == "down") d = gui::SlideTransition::Direction::Down;
            auto widget = std::make_shared<gui::SlideTransition>(d, duration);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_slide_transition_slide_in",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::SlideTransition>(getWidget(id));
            if (widget) widget->slideIn();
            return true;
        });

    registerFunction("gui", "__gui_slide_transition_slide_out",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::SlideTransition>(getWidget(id));
            if (widget) widget->slideOut();
            return true;
        });

    registerFunction("gui", "__gui_scale_transition_create",
        [](const std::vector<std::any>& args) -> std::any {
            float duration = args.empty() ? 300.0f : static_cast<float>(std::any_cast<double>(args[0]));
            auto widget = std::make_shared<gui::ScaleTransition>(duration);
            return storeWidget(widget);
        });

    registerFunction("gui", "__gui_scale_transition_scale_in",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::ScaleTransition>(getWidget(id));
            if (widget) widget->scaleIn();
            return true;
        });

    registerFunction("gui", "__gui_scale_transition_scale_out",
        [](const std::vector<std::any>& args) -> std::any {
            int id = std::any_cast<int>(args[0]);
            auto widget = std::dynamic_pointer_cast<gui::ScaleTransition>(getWidget(id));
            if (widget) widget->scaleOut();
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
            gui::RouterRegistry::instance().clear();
            gui::StoreRegistry::instance().clear();
            gui::ContextRegistry::instance().clear();
            gui::LifecycleRegistry::instance().clear();
            gui::MediaQueryRegistry::instance().clear();
            gui::AnimationManager::instance().clear();
            guiInterpreter_ = nullptr;
            nextStateId = 1;
            nextCallbackId = 1;
            currentApp.reset();
            return true;
        });
}

} // namespace stratos
