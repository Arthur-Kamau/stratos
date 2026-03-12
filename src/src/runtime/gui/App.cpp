#include "stratos/gui/App.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>

namespace stratos {
namespace gui {

// Forward declaration
KeyCode translateKeyCode(SDL_Keycode sym);

// ============================================================
// Theme Presets
// ============================================================

Theme Theme::light() {
    Theme t;
    // Defaults are already light theme
    return t;
}

Theme Theme::dark() {
    Theme t;
    t.primary = Color::rgb(100, 181, 246);
    t.primaryDark = Color::rgb(66, 165, 245);
    t.primaryLight = Color::rgb(144, 202, 249);
    t.secondary = Color::rgb(255, 128, 171);

    t.background = Color::rgb(18, 18, 18);
    t.surface = Color::rgb(30, 30, 30);

    t.onPrimary = Color::black();
    t.onSecondary = Color::black();
    t.onBackground = Color::white();
    t.onSurface = Color::white();

    t.textPrimary = Color::rgb(255, 255, 255);
    t.textSecondary = Color::rgb(176, 176, 176);
    t.textDisabled = Color::rgb(100, 100, 100);
    t.textHint = Color::rgb(130, 130, 130);

    t.divider = Color::rgb(50, 50, 50);
    return t;
}

// ============================================================
// Application
// ============================================================

App* App::currentApp_ = nullptr;

App::App(const std::string& title, int width, int height, RenderBackend backend)
    : title_(title), width_(width), height_(height), backendType_(backend) {
    currentApp_ = this;
}

App::~App() {
    // Release widget tree before renderer shutdown so widgets
    // are destroyed while SDL/TTF resources are still valid.
    root_.reset();
    if (renderer_) renderer_->shutdown();
    if (currentApp_ == this) currentApp_ = nullptr;
}

App* App::current() {
    return currentApp_;
}

bool App::init() {
    renderer_ = createRenderer(backendType_);
    if (!renderer_) {
        std::cerr << "Failed to create renderer" << std::endl;
        return false;
    }

    if (!renderer_->initialize(width_, height_, title_)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return false;
    }

    return true;
}

void App::setRoot(WidgetPtr root) {
    root_ = std::move(root);
    needsRedraw_ = true;
}

Size App::getWindowSize() const {
    if (renderer_) return renderer_->getWindowSize();
    return {static_cast<float>(width_), static_cast<float>(height_)};
}

void App::setWindowSize(int width, int height) {
    width_ = width;
    height_ = height;
    if (renderer_) renderer_->setWindowSize(width, height);
    needsRedraw_ = true;
}

void App::setWindowTitle(const std::string& title) {
    title_ = title;
    if (renderer_) renderer_->setWindowTitle(title);
}

void App::run() {
    if (!renderer_) {
        if (!init()) return;
    }

    running_ = true;
    auto frameTime = std::chrono::microseconds(1000000 / targetFPS_);

    while (running_) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        processEvents();
        update();
        render();

        // Frame rate limiting
        auto frameEnd = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(frameEnd - frameStart);
        if (elapsed < frameTime) {
            std::this_thread::sleep_for(frameTime - elapsed);
        }
    }

    // Don't call renderer_->shutdown() here — let the App destructor
    // handle it. This allows the native cleanup code to destroy widgets
    // (which may reference SDL/TTF resources) BEFORE SDL_Quit() runs.
}

void App::quit() {
    running_ = false;
}

void App::processEvents() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        if (sdlEvent.type == SDL_QUIT) {
            running_ = false;
            return;
        }

        Event event = translateSDLEvent(&sdlEvent);
        if (event.type == Event::None) continue;

        if (event.type == Event::WindowResize) {
            auto& we = event.windowResize();
            width_ = we.width;
            height_ = we.height;
            needsRedraw_ = true;
        }

        // Dispatch to widget tree
        if (root_) {
            root_->handleEvent(event);
            needsRedraw_ = true; // For simplicity, redraw on any event
        }
    }
}

void App::update() {
    if (!root_ || !needsRedraw_) return;

    // Re-layout from root
    Size windowSize = renderer_->getWindowSize();

    // Update MediaQuery with current window dimensions
    MediaQueryRegistry::instance().update(windowSize.width, windowSize.height);

    Constraints rootConstraints = {windowSize.width, windowSize.width,
                                    windowSize.height, windowSize.height};
    root_->layout(rootConstraints);
    root_->setPosition(0, 0);
}

void App::render() {
    if (!needsRedraw_) return;

    renderer_->beginFrame();
    renderer_->clear(theme_.background);

    if (root_) {
        root_->paint(*renderer_);
    }

    renderer_->endFrame();
    renderer_->present();
    needsRedraw_ = false;
}

Event App::translateSDLEvent(void* raw) {
    SDL_Event* sdlEvent = static_cast<SDL_Event*>(raw);
    Event event;

    switch (sdlEvent->type) {
        case SDL_MOUSEMOTION: {
            event.type = Event::MouseMove;
            event.data = MouseMoveEvent{
                static_cast<float>(sdlEvent->motion.x),
                static_cast<float>(sdlEvent->motion.y),
                static_cast<float>(sdlEvent->motion.xrel),
                static_cast<float>(sdlEvent->motion.yrel)
            };
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            event.type = Event::MouseButton;
            MouseButton btn = MouseButton::Left;
            if (sdlEvent->button.button == SDL_BUTTON_MIDDLE) btn = MouseButton::Middle;
            else if (sdlEvent->button.button == SDL_BUTTON_RIGHT) btn = MouseButton::Right;
            event.data = MouseButtonEvent{
                static_cast<float>(sdlEvent->button.x),
                static_cast<float>(sdlEvent->button.y),
                btn,
                sdlEvent->type == SDL_MOUSEBUTTONDOWN,
                static_cast<int>(sdlEvent->button.clicks)
            };
            break;
        }
        case SDL_MOUSEWHEEL: {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            event.type = Event::MouseScroll;
            event.data = MouseScrollEvent{
                static_cast<float>(mx), static_cast<float>(my),
                static_cast<float>(sdlEvent->wheel.x),
                static_cast<float>(sdlEvent->wheel.y)
            };
            break;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            event.type = Event::Key;
            event.data = KeyEvent{
                translateKeyCode(sdlEvent->key.keysym.sym),
                KeyModifiers{
                    (sdlEvent->key.keysym.mod & KMOD_SHIFT) != 0,
                    (sdlEvent->key.keysym.mod & KMOD_CTRL) != 0,
                    (sdlEvent->key.keysym.mod & KMOD_ALT) != 0,
                    (sdlEvent->key.keysym.mod & KMOD_GUI) != 0
                },
                sdlEvent->type == SDL_KEYDOWN,
                sdlEvent->key.repeat != 0
            };
            break;
        }
        case SDL_TEXTINPUT: {
            event.type = Event::TextInput;
            event.data = TextInputEvent{std::string(sdlEvent->text.text)};
            break;
        }
        case SDL_WINDOWEVENT: {
            switch (sdlEvent->window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    event.type = Event::WindowResize;
                    event.data = WindowResizeEvent{
                        sdlEvent->window.data1,
                        sdlEvent->window.data2
                    };
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::Close};
                    break;
                case SDL_WINDOWEVENT_MINIMIZED:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::Minimize};
                    break;
                case SDL_WINDOWEVENT_MAXIMIZED:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::Maximize};
                    break;
                case SDL_WINDOWEVENT_RESTORED:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::Restore};
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::FocusGained};
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    event.type = Event::Window;
                    event.data = WindowEvent{WindowEvent::FocusLost};
                    break;
            }
            break;
        }
    }

    return event;
}

// translate SDL keycode to our KeyCode enum — declared as static before App methods use it
KeyCode translateKeyCode(SDL_Keycode sym) {
    switch (sym) {
        case SDLK_a: return KeyCode::A;
        case SDLK_b: return KeyCode::B;
        case SDLK_c: return KeyCode::C;
        case SDLK_d: return KeyCode::D;
        case SDLK_e: return KeyCode::E;
        case SDLK_f: return KeyCode::F;
        case SDLK_g: return KeyCode::G;
        case SDLK_h: return KeyCode::H;
        case SDLK_i: return KeyCode::I;
        case SDLK_j: return KeyCode::J;
        case SDLK_k: return KeyCode::K;
        case SDLK_l: return KeyCode::L;
        case SDLK_m: return KeyCode::M;
        case SDLK_n: return KeyCode::N;
        case SDLK_o: return KeyCode::O;
        case SDLK_p: return KeyCode::P;
        case SDLK_q: return KeyCode::Q;
        case SDLK_r: return KeyCode::R;
        case SDLK_s: return KeyCode::S;
        case SDLK_t: return KeyCode::T;
        case SDLK_u: return KeyCode::U;
        case SDLK_v: return KeyCode::V;
        case SDLK_w: return KeyCode::W;
        case SDLK_x: return KeyCode::X;
        case SDLK_y: return KeyCode::Y;
        case SDLK_z: return KeyCode::Z;
        case SDLK_0: return KeyCode::Num0;
        case SDLK_1: return KeyCode::Num1;
        case SDLK_2: return KeyCode::Num2;
        case SDLK_3: return KeyCode::Num3;
        case SDLK_4: return KeyCode::Num4;
        case SDLK_5: return KeyCode::Num5;
        case SDLK_6: return KeyCode::Num6;
        case SDLK_7: return KeyCode::Num7;
        case SDLK_8: return KeyCode::Num8;
        case SDLK_9: return KeyCode::Num9;
        case SDLK_F1: return KeyCode::F1;
        case SDLK_F2: return KeyCode::F2;
        case SDLK_F3: return KeyCode::F3;
        case SDLK_F4: return KeyCode::F4;
        case SDLK_F5: return KeyCode::F5;
        case SDLK_F6: return KeyCode::F6;
        case SDLK_F7: return KeyCode::F7;
        case SDLK_F8: return KeyCode::F8;
        case SDLK_F9: return KeyCode::F9;
        case SDLK_F10: return KeyCode::F10;
        case SDLK_F11: return KeyCode::F11;
        case SDLK_F12: return KeyCode::F12;
        case SDLK_UP: return KeyCode::Up;
        case SDLK_DOWN: return KeyCode::Down;
        case SDLK_LEFT: return KeyCode::Left;
        case SDLK_RIGHT: return KeyCode::Right;
        case SDLK_HOME: return KeyCode::Home;
        case SDLK_END: return KeyCode::End;
        case SDLK_PAGEUP: return KeyCode::PageUp;
        case SDLK_PAGEDOWN: return KeyCode::PageDown;
        case SDLK_BACKSPACE: return KeyCode::Backspace;
        case SDLK_DELETE: return KeyCode::Delete;
        case SDLK_TAB: return KeyCode::Tab;
        case SDLK_RETURN: return KeyCode::Enter;
        case SDLK_ESCAPE: return KeyCode::Escape;
        case SDLK_SPACE: return KeyCode::Space;
        case SDLK_LSHIFT: return KeyCode::LeftShift;
        case SDLK_RSHIFT: return KeyCode::RightShift;
        case SDLK_LCTRL: return KeyCode::LeftCtrl;
        case SDLK_RCTRL: return KeyCode::RightCtrl;
        case SDLK_LALT: return KeyCode::LeftAlt;
        case SDLK_RALT: return KeyCode::RightAlt;
        case SDLK_LGUI: return KeyCode::LeftSuper;
        case SDLK_RGUI: return KeyCode::RightSuper;
        default: return KeyCode::Unknown;
    }
}

} // namespace gui
} // namespace stratos
