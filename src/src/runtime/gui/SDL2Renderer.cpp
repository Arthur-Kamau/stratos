#include "stratos/gui/Renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <sstream>

namespace {

// Global font registry: family name → file path(s) by weight
struct FontFamilyEntry {
    std::string regular;     // path to regular weight file
    std::string bold;        // path to bold weight file (optional)
    std::string italic;      // path to italic file (optional)
    std::string boldItalic;  // path to bold-italic file (optional)
    std::string variableFont; // path to variable font file (covers all weights)
};

static std::unordered_map<std::string, FontFamilyEntry> fontRegistry_;
static std::string defaultFontFamily_ = "Inter";

// Get cache directory for downloaded fonts
std::string getFontCacheDir() {
    const char* home = std::getenv("HOME");
    if (!home) home = "/tmp";
    std::string dir = std::string(home) + "/.cache/stratos/fonts";
    std::filesystem::create_directories(dir);
    return dir;
}

// Download a file via curl (blocking)
bool downloadFile(const std::string& url, const std::string& outputPath) {
    if (std::filesystem::exists(outputPath)) return true; // Already cached
    std::string cmd = "curl -sL -o \"" + outputPath + "\" \"" + url + "\" 2>/dev/null";
    int result = std::system(cmd.c_str());
    return result == 0 && std::filesystem::exists(outputPath) &&
           std::filesystem::file_size(outputPath) > 100;
}

// Download Inter font from Google Fonts API on first use
std::string ensureInterFont() {
    std::string cacheDir = getFontCacheDir();
    std::string fontPath = cacheDir + "/Inter-VariableFont.ttf";

    if (std::filesystem::exists(fontPath)) return fontPath;

    // Google Fonts direct download URL for Inter variable font
    std::string url = "https://github.com/google/fonts/raw/main/ofl/inter/Inter%5Bopsz%2Cwght%5D.ttf";
    std::cout << "Downloading Inter font..." << std::endl;
    if (downloadFile(url, fontPath)) {
        std::cout << "  Font cached at: " << fontPath << std::endl;
        return fontPath;
    }

    // Fallback: try alternate URL pattern
    url = "https://github.com/rsms/inter/releases/download/v4.1/Inter-4.1.zip";
    // If the variable font URL fails, we'll fall back to system fonts
    std::cerr << "  Warning: Could not download Inter font. Using system font." << std::endl;
    return "";
}

} // anonymous namespace

namespace stratos {
namespace gui {

// ============================================================
// SDL2 Renderer Implementation
// ============================================================

class SDL2Renderer : public IRenderer {
public:
    SDL2Renderer() = default;
    ~SDL2Renderer() override { shutdown(); }

    bool initialize(int width, int height, const std::string& title) override {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        if (TTF_Init() < 0) {
            std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
            return false;
        }

        int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
            // Non-fatal: continue without image support
        }

        window_ = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
        );
        if (!window_) {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            return false;
        }

        sdlRenderer_ = SDL_CreateRenderer(window_, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!sdlRenderer_) {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_SetRenderDrawBlendMode(sdlRenderer_, SDL_BLENDMODE_BLEND);

        width_ = width;
        height_ = height;

        // Calculate scale factor for HiDPI
        int drawW, drawH;
        SDL_GL_GetDrawableSize(window_, &drawW, &drawH);
        scaleFactor_ = static_cast<float>(drawW) / width;

        // Load a default font
        loadDefaultFont();

        initialized_ = true;
        return true;
    }

    void shutdown() override {
        if (!initialized_) return;

        // Clean up fonts
        for (auto& [key, font] : fontCache_) {
            if (font) TTF_CloseFont(font);
        }
        fontCache_.clear();

        // Clean up images
        for (auto& [id, tex] : imageCache_) {
            if (tex) SDL_DestroyTexture(tex);
        }
        imageCache_.clear();

        if (sdlRenderer_) { SDL_DestroyRenderer(sdlRenderer_); sdlRenderer_ = nullptr; }
        if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }

        IMG_Quit();
        TTF_Quit();
        SDL_Quit();
        initialized_ = false;
    }

    void beginFrame() override {
        // Save initial state
        stateStack_.clear();
        transformX_ = 0;
        transformY_ = 0;
        opacity_ = 1.0f;
    }

    void endFrame() override {
        // Nothing needed
    }

    void present() override {
        SDL_RenderPresent(sdlRenderer_);
    }

    void clear(const Color& color) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        SDL_RenderClear(sdlRenderer_);
    }

    // State management
    void save() override {
        stateStack_.push_back({transformX_, transformY_, clipRect_, opacity_});
    }

    void restore() override {
        if (!stateStack_.empty()) {
            auto& state = stateStack_.back();
            transformX_ = state.tx;
            transformY_ = state.ty;
            clipRect_ = state.clip;
            opacity_ = state.opacity;
            if (clipRect_.width > 0 && clipRect_.height > 0) {
                SDL_Rect clip = toSDLRect(clipRect_);
                SDL_RenderSetClipRect(sdlRenderer_, &clip);
            } else {
                SDL_RenderSetClipRect(sdlRenderer_, nullptr);
            }
            stateStack_.pop_back();
        }
    }

    void translate(float x, float y) override {
        transformX_ += x;
        transformY_ += y;
    }

    void scale(float sx, float sy) override {
        SDL_RenderSetScale(sdlRenderer_, sx, sy);
    }

    void setOpacity(float opacity) override {
        opacity_ = std::clamp(opacity, 0.0f, 1.0f);
    }

    void rotate(float /*radians*/) override {
        // SDL2 renderer doesn't support arbitrary rotation of the coordinate system.
        // Individual draw calls would need SDL_RenderCopyEx. Skia backend handles this.
    }

    void clipRect(const Rect& rect) override {
        clipRect_ = {rect.x + transformX_, rect.y + transformY_, rect.width, rect.height};
        SDL_Rect clip = toSDLRect(clipRect_);
        SDL_RenderSetClipRect(sdlRenderer_, &clip);
    }

    // Shapes
    void drawRect(const Rect& rect, const Color& color) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        SDL_FRect r = toSDLFRect(rect);
        SDL_RenderDrawRectF(sdlRenderer_, &r);
    }

    void fillRect(const Rect& rect, const Color& color) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        SDL_FRect r = toSDLFRect(rect);
        SDL_RenderFillRectF(sdlRenderer_, &r);
    }

    void drawRoundedRect(const Rect& rect, const BorderRadius& radius, const Color& color) override {
        // SDL2 doesn't have native rounded rect. Draw with line segments.
        float r = radius.topLeft; // Simplified: use topLeft for all corners
        if (r <= 0) { drawRect(rect, color); return; }
        drawRoundedRectImpl(rect, r, color, false);
    }

    void fillRoundedRect(const Rect& rect, const BorderRadius& radius, const Color& color) override {
        float r = radius.topLeft;
        if (r <= 0) { fillRect(rect, color); return; }
        drawRoundedRectImpl(rect, r, color, true);
    }

    void drawCircle(float cx, float cy, float radius, const Color& color) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        drawCircleImpl(cx + transformX_, cy + transformY_, radius, false);
    }

    void fillCircle(float cx, float cy, float radius, const Color& color) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        drawCircleImpl(cx + transformX_, cy + transformY_, radius, true);
    }

    void drawLine(float x1, float y1, float x2, float y2, const Color& color, float /*thickness*/) override {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLineF(sdlRenderer_,
            x1 + transformX_, y1 + transformY_,
            x2 + transformX_, y2 + transformY_);
    }

    // Text
    void drawText(const std::string& text, float x, float y, const FontSpec& font, const Color& color) override {
        if (text.empty()) return;

        TTF_Font* ttfFont = getFont(font);
        if (!ttfFont) return;

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(ttfFont, text.c_str(), sdlColor);
        if (!surface) return;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer_, surface);
        if (texture) {
            SDL_FRect dst = {
                x + transformX_, y + transformY_,
                static_cast<float>(surface->w), static_cast<float>(surface->h)
            };
            SDL_RenderCopyF(sdlRenderer_, texture, nullptr, &dst);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }

    TextMetrics measureText(const std::string& text, const FontSpec& font) override {
        TTF_Font* ttfFont = getFont(font);
        if (!ttfFont) return {0, 0, 0, 0};

        int w, h;
        TTF_SizeUTF8(ttfFont, text.c_str(), &w, &h);

        return {
            static_cast<float>(w),
            static_cast<float>(h),
            static_cast<float>(TTF_FontAscent(ttfFont)),
            static_cast<float>(TTF_FontDescent(ttfFont))
        };
    }

    // Images
    int loadImage(const std::string& path) override {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) return -1;

        SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer_, surface);
        SDL_FreeSurface(surface);
        if (!texture) return -1;

        int id = nextImageId_++;
        imageCache_[id] = texture;
        return id;
    }

    void drawImage(int imageId, const Rect& dest) override {
        auto it = imageCache_.find(imageId);
        if (it == imageCache_.end()) return;

        SDL_FRect dst = toSDLFRect(dest);
        SDL_RenderCopyF(sdlRenderer_, it->second, nullptr, &dst);
    }

    void drawImage(int imageId, const Rect& src, const Rect& dest) override {
        auto it = imageCache_.find(imageId);
        if (it == imageCache_.end()) return;

        SDL_Rect srcRect = {
            static_cast<int>(src.x), static_cast<int>(src.y),
            static_cast<int>(src.width), static_cast<int>(src.height)
        };
        SDL_FRect dst = toSDLFRect(dest);
        SDL_RenderCopyF(sdlRenderer_, it->second, &srcRect, &dst);
    }

    void unloadImage(int imageId) override {
        auto it = imageCache_.find(imageId);
        if (it != imageCache_.end()) {
            SDL_DestroyTexture(it->second);
            imageCache_.erase(it);
        }
    }

    // Font registration
    void registerFontFile(const std::string& family, const std::string& filePath,
                          const std::string& variant = "regular") override {
        auto& entry = fontRegistry_[family];
        if (variant == "bold") entry.bold = filePath;
        else if (variant == "italic") entry.italic = filePath;
        else if (variant == "bolditalic") entry.boldItalic = filePath;
        else if (variant == "variable") entry.variableFont = filePath;
        else entry.regular = filePath;
    }

    bool registerFontURL(const std::string& family, const std::string& url,
                         const std::string& variant = "regular") override {
        std::string cacheDir = getFontCacheDir();
        std::string filename = family + "-" + variant + ".ttf";
        std::string localPath = cacheDir + "/" + filename;

        if (!downloadFile(url, localPath)) return false;

        registerFontFile(family, localPath, variant);
        return true;
    }

    void setDefaultFontFamily(const std::string& family) override {
        defaultFontFamily_ = family;
        auto it = fontRegistry_.find(family);
        if (it != fontRegistry_.end()) {
            if (!it->second.variableFont.empty()) defaultFontPath_ = it->second.variableFont;
            else if (!it->second.regular.empty()) defaultFontPath_ = it->second.regular;
        }
    }

    // Shadows (approximate with layered rects)
    void drawShadow(const Rect& rect, const BorderRadius& radius, const Shadow& shadow) override {
        int layers = static_cast<int>(shadow.blur);
        for (int i = layers; i > 0; i--) {
            float alpha = static_cast<float>(shadow.color.a) * (1.0f - static_cast<float>(i) / layers) * 0.5f;
            Color layerColor = Color::rgba(shadow.color.r, shadow.color.g, shadow.color.b,
                                            static_cast<uint8_t>(alpha));
            Rect shadowRect = {
                rect.x + shadow.offsetX - i,
                rect.y + shadow.offsetY - i,
                rect.width + 2 * i,
                rect.height + 2 * i
            };
            BorderRadius expandedRadius = {
                radius.topLeft + i, radius.topRight + i,
                radius.bottomRight + i, radius.bottomLeft + i
            };
            fillRoundedRect(shadowRect, expandedRadius, layerColor);
        }
    }

    // Window
    Size getWindowSize() const override {
        return {static_cast<float>(width_), static_cast<float>(height_)};
    }

    void setWindowSize(int width, int height) override {
        SDL_SetWindowSize(window_, width, height);
        width_ = width;
        height_ = height;
    }

    void setWindowTitle(const std::string& title) override {
        SDL_SetWindowTitle(window_, title.c_str());
    }

    float getScaleFactor() const override {
        return scaleFactor_;
    }

    RenderBackend getBackend() const override { return RenderBackend::SDL2; }
    std::string getBackendName() const override { return "SDL2"; }

    // Access SDL window for event handling
    SDL_Window* getSDLWindow() const { return window_; }
    SDL_Renderer* getSDLRenderer() const { return sdlRenderer_; }

private:
    SDL_FRect toSDLFRect(const Rect& r) const {
        return {r.x + transformX_, r.y + transformY_, r.width, r.height};
    }

    SDL_Rect toSDLRect(const Rect& r) const {
        return {
            static_cast<int>(r.x), static_cast<int>(r.y),
            static_cast<int>(r.width), static_cast<int>(r.height)
        };
    }

    void loadDefaultFont() {
        // 1. Try to load Inter (download if needed)
        std::string interPath = ensureInterFont();
        if (!interPath.empty()) {
            TTF_Font* font = TTF_OpenFont(interPath.c_str(), 14);
            if (font) {
                defaultFontPath_ = interPath;
                fontCache_["Inter:14:400:0"] = font;
                fontCache_["sans-serif:14:400:0"] = font; // alias
                // Register Inter in the font registry
                FontFamilyEntry entry;
                entry.variableFont = interPath;
                fontRegistry_["Inter"] = entry;
                fontRegistry_["inter"] = entry; // case-insensitive alias
                return;
            }
        }

        // 2. Fallback: Try common system font paths
        const char* fontPaths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
            "/usr/share/fonts/truetype/noto/NotoSans-Regular.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "/usr/share/fonts/noto/NotoSans-Regular.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "/System/Library/Fonts/SFPro.ttf",
            "C:\\Windows\\Fonts\\segoeui.ttf",
            "C:\\Windows\\Fonts\\arial.ttf",
            nullptr
        };

        for (int i = 0; fontPaths[i]; i++) {
            TTF_Font* font = TTF_OpenFont(fontPaths[i], 14);
            if (font) {
                defaultFontPath_ = fontPaths[i];
                fontCache_["sans-serif:14:400:0"] = font;
                return;
            }
        }
        std::cerr << "Warning: No font found. Text rendering may not work." << std::endl;
    }

    std::string fontCacheKey(const FontSpec& spec) const {
        return spec.family + ":" +
               std::to_string(static_cast<int>(spec.size)) + ":" +
               std::to_string(static_cast<int>(spec.weight)) + ":" +
               std::to_string(static_cast<int>(spec.style));
    }

    TTF_Font* getFont(const FontSpec& spec) {
        std::string key = fontCacheKey(spec);
        auto it = fontCache_.find(key);
        if (it != fontCache_.end()) return it->second;

        // Look up family in font registry
        std::string fontPath = defaultFontPath_;

        auto regIt = fontRegistry_.find(spec.family);
        if (regIt != fontRegistry_.end()) {
            const auto& entry = regIt->second;
            // Pick the best file for requested weight/style
            if (!entry.variableFont.empty()) {
                fontPath = entry.variableFont;
            } else if (spec.weight >= FontWeight::Bold && spec.style == FontStyle::Italic && !entry.boldItalic.empty()) {
                fontPath = entry.boldItalic;
            } else if (spec.weight >= FontWeight::Bold && !entry.bold.empty()) {
                fontPath = entry.bold;
            } else if (spec.style == FontStyle::Italic && !entry.italic.empty()) {
                fontPath = entry.italic;
            } else if (!entry.regular.empty()) {
                fontPath = entry.regular;
            }
        } else if (spec.family == "monospace") {
            const char* monoPaths[] = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
                "C:\\Windows\\Fonts\\consola.ttf",
                nullptr
            };
            for (int i = 0; monoPaths[i]; i++) {
                if (std::filesystem::exists(monoPaths[i])) {
                    fontPath = monoPaths[i];
                    break;
                }
            }
        } else if (spec.family == "serif") {
            const char* serifPaths[] = {
                "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf",
                "C:\\Windows\\Fonts\\times.ttf",
                nullptr
            };
            for (int i = 0; serifPaths[i]; i++) {
                if (std::filesystem::exists(serifPaths[i])) {
                    fontPath = serifPaths[i];
                    break;
                }
            }
        }

        if (fontPath.empty()) return nullptr;

        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), static_cast<int>(spec.size));
        if (font) {
            int style = TTF_STYLE_NORMAL;
            if (spec.weight >= FontWeight::Bold) style |= TTF_STYLE_BOLD;
            if (spec.style == FontStyle::Italic) style |= TTF_STYLE_ITALIC;
            TTF_SetFontStyle(font, style);
            fontCache_[key] = font;
        }
        return font;
    }

    // Rounded rect using the midpoint circle algorithm
    void drawRoundedRectImpl(const Rect& rect, float radius, const Color& color, bool fill) {
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);

        float x = rect.x + transformX_;
        float y = rect.y + transformY_;
        float w = rect.width;
        float h = rect.height;
        float r = std::min(radius, std::min(w / 2, h / 2));

        if (fill) {
            // Fill the body rectangles
            SDL_FRect bodyH = {x + r, y, w - 2 * r, h};
            SDL_FRect bodyV = {x, y + r, w, h - 2 * r};
            SDL_RenderFillRectF(sdlRenderer_, &bodyH);
            SDL_RenderFillRectF(sdlRenderer_, &bodyV);

            // Fill the four corners
            fillCornerCircle(x + r, y + r, r);                      // top-left
            fillCornerCircle(x + w - r, y + r, r);                  // top-right
            fillCornerCircle(x + r, y + h - r, r);                  // bottom-left
            fillCornerCircle(x + w - r, y + h - r, r);              // bottom-right
        } else {
            // Top edge
            SDL_RenderDrawLineF(sdlRenderer_, x + r, y, x + w - r, y);
            // Bottom edge
            SDL_RenderDrawLineF(sdlRenderer_, x + r, y + h, x + w - r, y + h);
            // Left edge
            SDL_RenderDrawLineF(sdlRenderer_, x, y + r, x, y + h - r);
            // Right edge
            SDL_RenderDrawLineF(sdlRenderer_, x + w, y + r, x + w, y + h - r);

            // Draw corner arcs
            drawCornerArc(x + r, y + r, r, 180, 270);           // top-left
            drawCornerArc(x + w - r, y + r, r, 270, 360);       // top-right
            drawCornerArc(x + r, y + h - r, r, 90, 180);        // bottom-left
            drawCornerArc(x + w - r, y + h - r, r, 0, 90);      // bottom-right
        }
    }

    void fillCornerCircle(float cx, float cy, float radius) {
        int r = static_cast<int>(radius);
        for (int dy = -r; dy <= r; dy++) {
            int dx = static_cast<int>(std::sqrt(r * r - dy * dy));
            SDL_RenderDrawLineF(sdlRenderer_, cx - dx, cy + dy, cx + dx, cy + dy);
        }
    }

    void drawCornerArc(float cx, float cy, float radius, int startAngle, int endAngle) {
        for (int angle = startAngle; angle <= endAngle; angle++) {
            float rad = angle * 3.14159265f / 180.0f;
            float px = cx + radius * std::cos(rad);
            float py = cy + radius * std::sin(rad);
            SDL_RenderDrawPointF(sdlRenderer_, px, py);
        }
    }

    void drawCircleImpl(float cx, float cy, float radius, bool fill) {
        int r = static_cast<int>(radius);
        if (fill) {
            for (int dy = -r; dy <= r; dy++) {
                int dx = static_cast<int>(std::sqrt(r * r - dy * dy));
                SDL_RenderDrawLineF(sdlRenderer_, cx - dx, cy + dy, cx + dx, cy + dy);
            }
        } else {
            // Midpoint circle algorithm
            int x = r, y = 0, err = 1 - r;
            while (x >= y) {
                SDL_RenderDrawPointF(sdlRenderer_, cx + x, cy + y);
                SDL_RenderDrawPointF(sdlRenderer_, cx + y, cy + x);
                SDL_RenderDrawPointF(sdlRenderer_, cx - y, cy + x);
                SDL_RenderDrawPointF(sdlRenderer_, cx - x, cy + y);
                SDL_RenderDrawPointF(sdlRenderer_, cx - x, cy - y);
                SDL_RenderDrawPointF(sdlRenderer_, cx - y, cy - x);
                SDL_RenderDrawPointF(sdlRenderer_, cx + y, cy - x);
                SDL_RenderDrawPointF(sdlRenderer_, cx + x, cy - y);
                y++;
                if (err < 0) {
                    err += 2 * y + 1;
                } else {
                    x--;
                    err += 2 * (y - x) + 1;
                }
            }
        }
    }

    struct SavedState {
        float tx, ty;
        Rect clip;
        float opacity;
    };

    SDL_Window* window_ = nullptr;
    SDL_Renderer* sdlRenderer_ = nullptr;
    bool initialized_ = false;
    int width_ = 0, height_ = 0;
    float scaleFactor_ = 1.0f;

    // Transform state
    float transformX_ = 0, transformY_ = 0;
    float opacity_ = 1.0f;
    Rect clipRect_ = {0, 0, 0, 0};
    std::vector<SavedState> stateStack_;

    // Caches
    std::unordered_map<std::string, TTF_Font*> fontCache_;
    std::unordered_map<int, SDL_Texture*> imageCache_;
    int nextImageId_ = 1;
    std::string defaultFontPath_;
};

// ============================================================
// Factory
// ============================================================

std::unique_ptr<IRenderer> createRenderer(RenderBackend backend) {
    switch (backend) {
        case RenderBackend::SDL2:
            return std::make_unique<SDL2Renderer>();
        case RenderBackend::Skia:
            // TODO: Implement Skia backend when Skia is available
            std::cerr << "Skia backend not yet available, falling back to SDL2" << std::endl;
            return std::make_unique<SDL2Renderer>();
    }
    return std::make_unique<SDL2Renderer>();
}

} // namespace gui
} // namespace stratos
