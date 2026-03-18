#include "stratos/gui/Renderer.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>
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

        if (scaleFactor_ > 1.0f) {
            SDL_RenderSetLogicalSize(sdlRenderer_, width, height);
        }

        // Load a default font
        loadDefaultFont();

        initialized_ = true;
        return true;
    }

    void shutdown() override {
        if (!initialized_) return;

        // Clean up fonts — deduplicate pointers to avoid double-free
        // (e.g. "Inter:14:400:0" and "sans-serif:14:400:0" share the same TTF_Font*)
        std::set<TTF_Font*> closedFonts;
        for (auto& [key, font] : fontCache_) {
            if (font && closedFonts.find(font) == closedFonts.end()) {
                TTF_CloseFont(font);
                closedFonts.insert(font);
            }
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

    void drawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness) override {
        float ax = x1 + transformX_;
        float ay = y1 + transformY_;
        float bx = x2 + transformX_;
        float by = y2 + transformY_;
        drawAALine(ax, ay, bx, by, color, thickness);
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
            // On HiDPI, font was opened at size*scaleFactor_ so surface is larger;
            // divide back to get logical coordinates
            float dispW = static_cast<float>(surface->w) / scaleFactor_;
            float dispH = static_cast<float>(surface->h) / scaleFactor_;
            SDL_FRect dst = {
                x + transformX_, y + transformY_,
                dispW, dispH
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

        // Divide by scaleFactor_ since fonts are opened at size*scaleFactor_ on HiDPI
        float sf = scaleFactor_;
        return {
            static_cast<float>(w) / sf,
            static_cast<float>(h) / sf,
            static_cast<float>(TTF_FontAscent(ttfFont)) / sf,
            static_cast<float>(TTF_FontDescent(ttfFont)) / sf
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

        // Scale font size by scaleFactor_ for HiDPI — renders at higher resolution
        TTF_Font* font = TTF_OpenFont(fontPath.c_str(), static_cast<int>(spec.size * scaleFactor_));
        if (font) {
            int style = TTF_STYLE_NORMAL;
            if (spec.weight >= FontWeight::Bold) style |= TTF_STYLE_BOLD;
            if (spec.style == FontStyle::Italic) style |= TTF_STYLE_ITALIC;
            TTF_SetFontStyle(font, style);
            fontCache_[key] = font;
        }
        return font;
    }

    // ---- SDF-based anti-aliased rendering ----

    // Rounded-rect signed distance function
    // Returns negative inside, positive outside, 0 on edge
    float roundedRectSDF(float px, float py, float cx, float cy, float halfW, float halfH, float r) {
        float dx = std::max(std::abs(px - cx) - halfW + r, 0.0f);
        float dy = std::max(std::abs(py - cy) - halfH + r, 0.0f);
        return std::sqrt(dx * dx + dy * dy) - r;
    }

    // Draw a single pixel with alpha blending
    void drawPixelAlpha(float px, float py, const Color& color, float alpha) {
        if (alpha <= 0.0f) return;
        alpha = std::min(alpha, 1.0f);
        uint8_t a = static_cast<uint8_t>(color.a * alpha);
        if (a == 0) return;
        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, a);
        SDL_RenderDrawPointF(sdlRenderer_, px, py);
    }

    void drawRoundedRectImpl(const Rect& rect, float radius, const Color& color, bool fill) {
        float x = rect.x + transformX_;
        float y = rect.y + transformY_;
        float w = rect.width;
        float h = rect.height;
        float r = std::min(radius, std::min(w / 2.0f, h / 2.0f));

        float cx = x + w / 2.0f;
        float cy = y + h / 2.0f;
        float halfW = w / 2.0f;
        float halfH = h / 2.0f;

        if (fill) {
            if (r <= 0) {
                SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                SDL_FRect body = {x, y, w, h};
                SDL_RenderFillRectF(sdlRenderer_, &body);
                return;
            }

            // Scanline approach: for each row, compute SDF-clipped left/right edges
            // This avoids all body-rect/corner seam issues
            int iy = static_cast<int>(std::floor(y));
            int iyEnd = static_cast<int>(std::ceil(y + h));

            SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);

            for (int row = iy - 1; row <= iyEnd; row++) {
                float py = row + 0.5f;

                // Skip rows clearly outside
                if (py < y - 1.0f || py > y + h + 1.0f) continue;

                // For this row, find the leftmost and rightmost visible pixels
                int ix = static_cast<int>(std::floor(x));
                int ixEnd = static_cast<int>(std::ceil(x + w));

                // Check if this row is in the corner zone (top r pixels or bottom r pixels)
                bool inCornerZone = (py < y + r + 0.5f) || (py > y + h - r - 0.5f);

                if (!inCornerZone) {
                    // Middle rows: straight edges, no AA needed on left/right
                    // But we still need AA on the top/bottom edges of the straight portion
                    float d = roundedRectSDF(x + 0.5f, py, cx, cy, halfW, halfH, r);
                    if (d < -0.5f) {
                        // Fully inside — fill entire row
                        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                        SDL_RenderDrawLineF(sdlRenderer_, x, py, x + w - 1, py);
                    } else {
                        // Edge row — need per-pixel SDF
                        for (int col = ix - 1; col <= ixEnd; col++) {
                            float px = col + 0.5f;
                            float dd = roundedRectSDF(px, py, cx, cy, halfW, halfH, r);
                            float coverage = std::clamp(0.5f - dd, 0.0f, 1.0f);
                            if (coverage >= 0.99f) {
                                SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                                SDL_RenderDrawPointF(sdlRenderer_, px, py);
                            } else if (coverage > 0.0f) {
                                drawPixelAlpha(px, py, color, coverage);
                            }
                        }
                    }
                } else {
                    // Corner zone rows: need per-pixel SDF for the corners,
                    // but can fill the middle straight section in bulk

                    // Find where the curve starts/ends using SDF
                    // Left corner zone: x to x+r+1
                    // Right corner zone: x+w-r-1 to x+w
                    // Middle: x+r+1 to x+w-r-1

                    // Left corner pixels
                    int leftEnd = static_cast<int>(std::ceil(x + r)) + 1;
                    for (int col = ix - 1; col <= leftEnd; col++) {
                        float px = col + 0.5f;
                        float dd = roundedRectSDF(px, py, cx, cy, halfW, halfH, r);
                        float coverage = std::clamp(0.5f - dd, 0.0f, 1.0f);
                        if (coverage >= 0.99f) {
                            SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                            SDL_RenderDrawPointF(sdlRenderer_, px, py);
                        } else if (coverage > 0.0f) {
                            drawPixelAlpha(px, py, color, coverage);
                        }
                    }

                    // Middle section (guaranteed inside if the row itself is inside)
                    float midLeft = x + r + 1;
                    float midRight = x + w - r - 1;
                    float dMid = roundedRectSDF(midLeft, py, cx, cy, halfW, halfH, r);
                    if (dMid < -0.5f && midRight > midLeft) {
                        SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                        SDL_RenderDrawLineF(sdlRenderer_, midLeft, py, midRight, py);
                    }

                    // Right corner pixels
                    int rightStart = static_cast<int>(std::floor(x + w - r)) - 1;
                    for (int col = rightStart; col <= ixEnd + 1; col++) {
                        float px = col + 0.5f;
                        float dd = roundedRectSDF(px, py, cx, cy, halfW, halfH, r);
                        float coverage = std::clamp(0.5f - dd, 0.0f, 1.0f);
                        if (coverage >= 0.99f) {
                            SDL_SetRenderDrawColor(sdlRenderer_, color.r, color.g, color.b, color.a);
                            SDL_RenderDrawPointF(sdlRenderer_, px, py);
                        } else if (coverage > 0.0f) {
                            drawPixelAlpha(px, py, color, coverage);
                        }
                    }
                }
            }
        } else {
            // Stroke: draw an AA outline using SDF — iterate entire perimeter band
            float strokeW = 1.0f;
            int iy = static_cast<int>(std::floor(y)) - 1;
            int iyEnd = static_cast<int>(std::ceil(y + h)) + 1;
            int ix = static_cast<int>(std::floor(x)) - 1;
            int ixEnd = static_cast<int>(std::ceil(x + w)) + 1;

            for (int row = iy; row <= iyEnd; row++) {
                float py = row + 0.5f;
                for (int col = ix; col <= ixEnd; col++) {
                    float px = col + 0.5f;
                    float d = roundedRectSDF(px, py, cx, cy, halfW, halfH, r);
                    // Only process pixels near the edge (within strokeW + 1 of boundary)
                    if (d > 1.0f || d < -(strokeW + 1.0f)) continue;
                    float outer = std::clamp(0.5f - d, 0.0f, 1.0f);
                    float inner = std::clamp(0.5f - (-(d + strokeW)), 0.0f, 1.0f);
                    float coverage = outer * inner;
                    drawPixelAlpha(px, py, color, coverage);
                }
            }
        }
    }

    // Anti-aliased circle using SDF
    void drawCircleImpl(float cx, float cy, float radius, bool fill) {
        int r = static_cast<int>(std::ceil(radius)) + 2;

        if (fill) {
            // Get the current draw color (set by callers)
            uint8_t cr, cg, cb, ca;
            SDL_GetRenderDrawColor(sdlRenderer_, &cr, &cg, &cb, &ca);
            Color color = Color::rgba(cr, cg, cb, ca);

            // Fill interior scanlines
            int ir = static_cast<int>(radius - 1);
            SDL_SetRenderDrawColor(sdlRenderer_, cr, cg, cb, ca);
            for (int dy = -ir; dy <= ir; dy++) {
                int dx = static_cast<int>(std::sqrt(radius * radius - dy * dy) - 1);
                if (dx > 0) {
                    SDL_RenderDrawLineF(sdlRenderer_, cx - dx, cy + dy, cx + dx, cy + dy);
                }
            }

            // AA the edge pixels
            for (int dy = -r; dy <= r; dy++) {
                for (int dx = -r; dx <= r; dx++) {
                    float px = cx + dx + 0.5f;
                    float py = cy + dy + 0.5f;
                    float dist = std::sqrt((px - cx) * (px - cx) + (py - cy) * (py - cy)) - radius;
                    // Only process edge pixels (within ~1px of the edge)
                    if (dist > -1.5f && dist < 1.0f) {
                        float coverage = std::clamp(0.5f - dist, 0.0f, 1.0f);
                        drawPixelAlpha(px, py, color, coverage);
                    }
                }
            }
        } else {
            // Stroke circle
            uint8_t cr, cg, cb, ca;
            SDL_GetRenderDrawColor(sdlRenderer_, &cr, &cg, &cb, &ca);
            Color color = Color::rgba(cr, cg, cb, ca);
            float strokeW = 1.0f;

            for (int dy = -r; dy <= r; dy++) {
                for (int dx = -r; dx <= r; dx++) {
                    float px = cx + dx + 0.5f;
                    float py = cy + dy + 0.5f;
                    float dist = std::sqrt((px - cx) * (px - cx) + (py - cy) * (py - cy)) - radius;
                    float outer = std::clamp(0.5f - dist, 0.0f, 1.0f);
                    float inner = std::clamp(0.5f - (-(dist + strokeW)), 0.0f, 1.0f);
                    float coverage = outer * inner;
                    drawPixelAlpha(px, py, color, coverage);
                }
            }
        }
    }

    // Anti-aliased line using perpendicular distance
    void drawAALine(float x1, float y1, float x2, float y2, const Color& color, float thickness) {
        float dx = x2 - x1;
        float dy = y2 - y1;
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 0.001f) return;

        float halfW = std::max(thickness / 2.0f, 0.5f);

        // Bounding box with margin
        int minX = static_cast<int>(std::floor(std::min(x1, x2) - halfW - 1));
        int maxX = static_cast<int>(std::ceil(std::max(x1, x2) + halfW + 1));
        int minY = static_cast<int>(std::floor(std::min(y1, y2) - halfW - 1));
        int maxY = static_cast<int>(std::ceil(std::max(y1, y2) + halfW + 1));

        // Unit direction and normal
        float ux = dx / len, uy = dy / len;
        float nx = -uy, ny = ux;

        for (int py = minY; py <= maxY; py++) {
            for (int px = minX; px <= maxX; px++) {
                float fpx = px + 0.5f;
                float fpy = py + 0.5f;

                // Project onto line segment
                float t = ((fpx - x1) * ux + (fpy - y1) * uy);
                // Perpendicular distance
                float perpDist = std::abs((fpx - x1) * nx + (fpy - y1) * ny);

                // Distance along the line (for end caps)
                float alongDist = 0.0f;
                if (t < 0) alongDist = -t;
                else if (t > len) alongDist = t - len;

                float totalDist = std::sqrt(perpDist * perpDist + alongDist * alongDist);
                float coverage = std::clamp(halfW + 0.5f - totalDist, 0.0f, 1.0f);
                drawPixelAlpha(fpx, fpy, color, coverage);
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
