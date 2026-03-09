#pragma once

#include "stratos/gui/Renderer.h"
#include "stratos/gui/Widget.h"
#include "stratos/gui/Event.h"
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

namespace stratos {
namespace gui {

// ============================================================
// Theme
// ============================================================

struct Theme {
    // Primary colors
    Color primary = Color::rgb(33, 150, 243);       // Blue
    Color primaryDark = Color::rgb(25, 118, 210);
    Color primaryLight = Color::rgb(100, 181, 246);

    // Secondary colors
    Color secondary = Color::rgb(255, 64, 129);     // Pink
    Color secondaryDark = Color::rgb(197, 17, 98);

    // Surface colors
    Color background = Color::rgb(250, 250, 250);
    Color surface = Color::white();
    Color error = Color::rgb(211, 47, 47);
    Color success = Color::rgb(46, 125, 50);
    Color warning = Color::rgb(237, 108, 2);

    // Text colors
    Color onPrimary = Color::white();
    Color onSecondary = Color::white();
    Color onBackground = Color::rgb(33, 33, 33);
    Color onSurface = Color::rgb(33, 33, 33);
    Color onError = Color::white();

    // Text styles
    Color textPrimary = Color::rgb(33, 33, 33);
    Color textSecondary = Color::rgb(117, 117, 117);
    Color textDisabled = Color::rgb(189, 189, 189);
    Color textHint = Color::rgb(158, 158, 158);

    // Divider
    Color divider = Color::rgb(224, 224, 224);

    // Default fonts
    FontSpec bodyFont = {"sans-serif", 14.0f, FontWeight::Regular, FontStyle::Normal};
    FontSpec titleFont = {"sans-serif", 20.0f, FontWeight::Medium, FontStyle::Normal};
    FontSpec headingFont = {"sans-serif", 24.0f, FontWeight::Bold, FontStyle::Normal};
    FontSpec captionFont = {"sans-serif", 12.0f, FontWeight::Regular, FontStyle::Normal};

    // Spacing
    float spacingSmall = 4.0f;
    float spacingMedium = 8.0f;
    float spacingLarge = 16.0f;
    float spacingXLarge = 24.0f;

    // Border radius
    float borderRadiusSmall = 4.0f;
    float borderRadiusMedium = 8.0f;
    float borderRadiusLarge = 12.0f;

    // Elevation (shadow)
    Shadow elevation1 = {0, 1, 3, Color::rgba(0, 0, 0, 30)};
    Shadow elevation2 = {0, 2, 6, Color::rgba(0, 0, 0, 40)};
    Shadow elevation3 = {0, 4, 12, Color::rgba(0, 0, 0, 50)};

    // Presets
    static Theme light();
    static Theme dark();
};

// ============================================================
// Reactive State
// ============================================================

class StateBase {
public:
    virtual ~StateBase() = default;
    virtual void notifyListeners() = 0;

    void addListener(std::function<void()> listener) {
        listeners_.push_back(std::move(listener));
    }

protected:
    std::vector<std::function<void()>> listeners_;
};

template<typename T>
class State : public StateBase {
public:
    explicit State(T value) : value_(std::move(value)) {}

    const T& get() const { return value_; }

    void set(T value) {
        if (value_ != value) {
            value_ = std::move(value);
            notifyListeners();
        }
    }

    void update(std::function<void(T&)> updater) {
        updater(value_);
        notifyListeners();
    }

    void notifyListeners() override {
        for (auto& listener : listeners_) {
            listener();
        }
    }

    // Convenience operators
    operator const T&() const { return value_; }

private:
    T value_;
};

// ============================================================
// Application
// ============================================================

class App {
public:
    App(const std::string& title, int width, int height,
        RenderBackend backend = RenderBackend::SDL2);
    ~App();

    // Lifecycle
    bool init();
    void run();
    void quit();

    // Root widget
    void setRoot(WidgetPtr root);
    WidgetPtr getRoot() const { return root_; }

    // Theme
    void setTheme(const Theme& theme) { theme_ = theme; }
    const Theme& getTheme() const { return theme_; }

    // Window
    IRenderer& getRenderer() { return *renderer_; }
    Size getWindowSize() const;
    void setWindowSize(int width, int height);
    void setWindowTitle(const std::string& title);

    // Frame rate
    void setTargetFPS(int fps) { targetFPS_ = fps; }
    int getTargetFPS() const { return targetFPS_; }

    // State management
    template<typename T>
    std::shared_ptr<State<T>> createState(T initialValue) {
        auto state = std::make_shared<State<T>>(std::move(initialValue));
        state->addListener([this]() { requestRedraw(); });
        states_.push_back(state);
        return state;
    }

    void requestRedraw() { needsRedraw_ = true; }

    // Singleton access (for native registry)
    static App* current();

private:
    void processEvents();
    void update();
    void render();
    Event translateSDLEvent(void* sdlEvent);

    std::string title_;
    int width_, height_;
    RenderBackend backendType_;
    std::unique_ptr<IRenderer> renderer_;
    WidgetPtr root_;
    Theme theme_;
    bool running_ = false;
    bool needsRedraw_ = true;
    int targetFPS_ = 60;
    std::vector<std::shared_ptr<StateBase>> states_;

    // Focus management
    Widget* focusedWidget_ = nullptr;
    Widget* hoveredWidget_ = nullptr;

    static App* currentApp_;
};

} // namespace gui
} // namespace stratos
