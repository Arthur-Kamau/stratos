#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <vector>
#include <functional>

namespace stratos {
namespace gui {

// Forward declarations
struct Color;
struct Rect;
struct Font;

// ============================================================
// Core Types
// ============================================================

struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;

    static Color rgb(uint8_t r, uint8_t g, uint8_t b) { return {r, g, b, 255}; }
    static Color rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { return {r, g, b, a}; }
    static Color hex(uint32_t hex) {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            255
        };
    }

    // Common colors
    static Color white() { return {255, 255, 255, 255}; }
    static Color black() { return {0, 0, 0, 255}; }
    static Color transparent() { return {0, 0, 0, 0}; }
    static Color red() { return {255, 0, 0, 255}; }
    static Color green() { return {0, 255, 0, 255}; }
    static Color blue() { return {0, 0, 255, 255}; }
    static Color gray() { return {128, 128, 128, 255}; }
};

struct Point {
    float x = 0, y = 0;
};

struct Size {
    float width = 0, height = 0;
};

struct Rect {
    float x = 0, y = 0;
    float width = 0, height = 0;

    bool contains(float px, float py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

struct EdgeInsets {
    float top = 0, right = 0, bottom = 0, left = 0;

    static EdgeInsets all(float v) { return {v, v, v, v}; }
    static EdgeInsets symmetric(float h, float v) { return {v, h, v, h}; }
};

struct BorderRadius {
    float topLeft = 0, topRight = 0, bottomRight = 0, bottomLeft = 0;

    static BorderRadius all(float v) { return {v, v, v, v}; }
    static BorderRadius circular(float v) { return {v, v, v, v}; }
};

struct Shadow {
    float offsetX = 0, offsetY = 2;
    float blur = 4;
    Color color = Color::rgba(0, 0, 0, 64);
};

enum class FontWeight {
    Thin = 100,
    Light = 300,
    Regular = 400,
    Medium = 500,
    SemiBold = 600,
    Bold = 700,
    ExtraBold = 800,
    Black = 900
};

enum class FontStyle {
    Normal,
    Italic
};

enum class TextAlign {
    Left,
    Center,
    Right
};

struct FontSpec {
    std::string family = "sans-serif";
    float size = 14.0f;
    FontWeight weight = FontWeight::Regular;
    FontStyle style = FontStyle::Normal;
};

struct TextMetrics {
    float width = 0;
    float height = 0;
    float ascent = 0;
    float descent = 0;
};

// ============================================================
// Renderer Backend Interface
// ============================================================

enum class RenderBackend {
    SDL2,
    Skia
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Lifecycle
    virtual bool initialize(int width, int height, const std::string& title) = 0;
    virtual void shutdown() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void present() = 0;
    virtual void clear(const Color& color) = 0;

    // State
    virtual void save() = 0;
    virtual void restore() = 0;
    virtual void translate(float x, float y) = 0;
    virtual void scale(float sx, float sy) = 0;
    virtual void rotate(float radians) = 0;
    virtual void clipRect(const Rect& rect) = 0;

    // Shapes
    virtual void drawRect(const Rect& rect, const Color& color) = 0;
    virtual void fillRect(const Rect& rect, const Color& color) = 0;
    virtual void drawRoundedRect(const Rect& rect, const BorderRadius& radius, const Color& color) = 0;
    virtual void fillRoundedRect(const Rect& rect, const BorderRadius& radius, const Color& color) = 0;
    virtual void drawCircle(float cx, float cy, float radius, const Color& color) = 0;
    virtual void fillCircle(float cx, float cy, float radius, const Color& color) = 0;
    virtual void drawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness = 1.0f) = 0;

    // Text
    virtual void drawText(const std::string& text, float x, float y, const FontSpec& font, const Color& color) = 0;
    virtual TextMetrics measureText(const std::string& text, const FontSpec& font) = 0;

    // Images
    virtual int loadImage(const std::string& path) = 0;
    virtual void drawImage(int imageId, const Rect& dest) = 0;
    virtual void drawImage(int imageId, const Rect& src, const Rect& dest) = 0;
    virtual void unloadImage(int imageId) = 0;

    // Shadows
    virtual void drawShadow(const Rect& rect, const BorderRadius& radius, const Shadow& shadow) = 0;

    // Window
    virtual Size getWindowSize() const = 0;
    virtual void setWindowSize(int width, int height) = 0;
    virtual void setWindowTitle(const std::string& title) = 0;
    virtual float getScaleFactor() const = 0;

    // Backend info
    virtual RenderBackend getBackend() const = 0;
    virtual std::string getBackendName() const = 0;
};

// Factory function — creates appropriate backend
std::unique_ptr<IRenderer> createRenderer(RenderBackend backend = RenderBackend::SDL2);

} // namespace gui
} // namespace stratos
