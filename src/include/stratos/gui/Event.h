#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <variant>
#include <vector>

namespace stratos {
namespace gui {

// ============================================================
// Input Events
// ============================================================

enum class MouseButton {
    Left,
    Middle,
    Right
};

enum class KeyCode {
    Unknown = 0,
    // Letters
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    // Numbers
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    // Function keys
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    // Navigation
    Up, Down, Left, Right, Home, End, PageUp, PageDown,
    // Editing
    Backspace, Delete, Tab, Enter, Escape, Space,
    Insert,
    // Modifiers
    LeftShift, RightShift, LeftCtrl, RightCtrl,
    LeftAlt, RightAlt, LeftSuper, RightSuper,
    // Punctuation
    Comma, Period, Slash, Semicolon, Quote,
    LeftBracket, RightBracket, Backslash, Grave,
    Minus, Equals,
    CapsLock, NumLock, ScrollLock, PrintScreen, Pause
};

struct KeyModifiers {
    bool shift = false;
    bool ctrl = false;
    bool alt = false;
    bool super = false;
};

// Individual event types
struct MouseMoveEvent {
    float x, y;
    float deltaX, deltaY;
};

struct MouseButtonEvent {
    float x, y;
    MouseButton button;
    bool pressed;  // true = press, false = release
    int clicks;    // 1 = single, 2 = double
};

struct MouseScrollEvent {
    float x, y;
    float scrollX, scrollY;
};

struct KeyEvent {
    KeyCode key;
    KeyModifiers modifiers;
    bool pressed;  // true = press, false = release
    bool repeat;   // true if this is a key repeat
};

struct TextInputEvent {
    std::string text;  // UTF-8 text input
};

struct WindowResizeEvent {
    int width, height;
};

struct WindowEvent {
    enum Type {
        Close,
        Minimize,
        Maximize,
        Restore,
        FocusGained,
        FocusLost
    } type;
};

// Unified event type
struct Event {
    enum Type {
        None,
        MouseMove,
        MouseButton,
        MouseScroll,
        Key,
        TextInput,
        WindowResize,
        Window,
        Quit
    } type = None;

    std::variant<
        std::monostate,
        MouseMoveEvent,
        MouseButtonEvent,
        MouseScrollEvent,
        KeyEvent,
        TextInputEvent,
        WindowResizeEvent,
        WindowEvent
    > data;

    // Convenience accessors
    const MouseMoveEvent& mouseMove() const { return std::get<MouseMoveEvent>(data); }
    const MouseButtonEvent& mouseButton() const { return std::get<MouseButtonEvent>(data); }
    const MouseScrollEvent& mouseScroll() const { return std::get<MouseScrollEvent>(data); }
    const KeyEvent& key() const { return std::get<KeyEvent>(data); }
    const TextInputEvent& textInput() const { return std::get<TextInputEvent>(data); }
    const WindowResizeEvent& windowResize() const { return std::get<WindowResizeEvent>(data); }
    const WindowEvent& window() const { return std::get<WindowEvent>(data); }
};

// ============================================================
// Event Handler Types
// ============================================================

using OnClickHandler = std::function<void()>;
using OnMouseHandler = std::function<void(float x, float y)>;
using OnKeyHandler = std::function<void(KeyCode key, KeyModifiers mods)>;
using OnTextHandler = std::function<void(const std::string& text)>;
using OnChangeHandler = std::function<void(const std::string& value)>;
using OnBoolChangeHandler = std::function<void(bool value)>;
using OnFloatChangeHandler = std::function<void(float value)>;
using OnScrollHandler = std::function<void(float dx, float dy)>;

} // namespace gui
} // namespace stratos
