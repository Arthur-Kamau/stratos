#include "stratos/gui/Widget.h"
#include "stratos/gui/App.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <regex>

namespace stratos {
namespace gui {

// Helper to get the active theme (returns nullptr if no app)
static const Theme* activeTheme() {
    App* app = App::current();
    return app ? &app->getTheme() : nullptr;
}

// ============================================================
// Base Widget
// ============================================================

void Widget::layout(const Constraints& constraints) {
    // Default: size to style dimensions or zero
    float w = style_.width >= 0 ? style_.width : 0;
    float h = style_.height >= 0 ? style_.height : 0;

    w = std::clamp(w, constraints.minWidth, constraints.maxWidth);
    h = std::clamp(h, constraints.minHeight, constraints.maxHeight);

    bounds_.width = w;
    bounds_.height = h;

    for (auto& child : children_) {
        child->layout(constraints);
    }
}

void Widget::paint(IRenderer& renderer) {
    if (!visible_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    paintShadow(renderer);
    paintBackground(renderer);
    paintChildren(renderer);
    paintBorder(renderer);

    renderer.restore();
}

bool Widget::handleEvent(const Event& event) {
    if (!visible_ || !isEnabled()) return false;

    // Check mouse events for hover state
    if (event.type == Event::MouseMove) {
        auto& me = event.mouseMove();
        bool inside = hitTest(me.x, me.y);
        if (inside && !hovered_) {
            hovered_ = true;
            state_ = WidgetState::Hovered;
            if (onMouseEnter_) onMouseEnter_(me.x, me.y);
        } else if (!inside && hovered_) {
            hovered_ = false;
            state_ = WidgetState::Normal;
            if (onMouseLeave_) onMouseLeave_(me.x, me.y);
        }
    }

    // Check click — dispatch to children first, then handle self
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y)) {
            // Let children handle it first
            if (dispatchToChildren(event)) return true;

            // No child consumed it — handle it ourselves
            if (me.pressed) {
                state_ = WidgetState::Pressed;
            } else {
                if (state_ == WidgetState::Pressed && onClick_) {
                    onClick_();
                }
                state_ = hovered_ ? WidgetState::Hovered : WidgetState::Normal;
            }
            return onClick_ != nullptr; // only consume if we have a handler
        }
    }

    return dispatchToChildren(event);
}

void Widget::addChild(WidgetPtr child) {
    child->parent_ = this;
    children_.push_back(std::move(child));
    markDirty();
}

void Widget::removeChild(WidgetPtr child) {
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->parent_ = nullptr;
        children_.erase(it);
        markDirty();
    }
}

void Widget::clearChildren() {
    for (auto& child : children_) child->parent_ = nullptr;
    children_.clear();
    markDirty();
}

bool Widget::hitTest(float x, float y) const {
    return bounds_.contains(x, y);
}

void Widget::markDirty() {
    dirty_ = true;
    if (parent_) parent_->markDirty();
}

void Widget::paintBackground(IRenderer& renderer) {
    if (style_.backgroundColor.a == 0) return;
    Rect local = {0, 0, bounds_.width, bounds_.height};
    if (style_.borderRadius.topLeft > 0 || style_.borderRadius.topRight > 0 ||
        style_.borderRadius.bottomLeft > 0 || style_.borderRadius.bottomRight > 0) {
        renderer.fillRoundedRect(local, style_.borderRadius, style_.backgroundColor);
    } else {
        renderer.fillRect(local, style_.backgroundColor);
    }
}

void Widget::paintBorder(IRenderer& renderer) {
    if (style_.borderWidth <= 0 || style_.borderColor.a == 0) return;
    Rect local = {0, 0, bounds_.width, bounds_.height};
    if (style_.borderRadius.topLeft > 0) {
        renderer.drawRoundedRect(local, style_.borderRadius, style_.borderColor);
    } else {
        renderer.drawRect(local, style_.borderColor);
    }
}

void Widget::paintShadow(IRenderer& renderer) {
    if (!style_.hasShadow) return;
    Rect local = {0, 0, bounds_.width, bounds_.height};
    renderer.drawShadow(local, style_.borderRadius, style_.shadow);
}

void Widget::paintChildren(IRenderer& renderer) {
    for (auto& child : children_) {
        child->paint(renderer);
    }
}

bool Widget::dispatchToChildren(const Event& event) {
    // Transform mouse coordinates to local space before dispatching
    Event localEvent = event;
    float ox = bounds_.x, oy = bounds_.y;

    if (event.type == Event::MouseButton) {
        auto me = event.mouseButton();
        me.x -= ox;
        me.y -= oy;
        localEvent.data = me;
    } else if (event.type == Event::MouseMove) {
        auto me = event.mouseMove();
        me.x -= ox;
        me.y -= oy;
        localEvent.data = me;
    } else if (event.type == Event::MouseScroll) {
        auto me = event.mouseScroll();
        me.x -= ox;
        me.y -= oy;
        localEvent.data = me;
    }

    // Dispatch in reverse order (top-most first)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if ((*it)->handleEvent(localEvent)) return true;
    }
    return false;
}

// ============================================================
// Row Layout
// ============================================================

void Row::layout(const Constraints& constraints) {
    float maxWidth = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float maxHeight = style_.height >= 0 ? style_.height : constraints.maxHeight;

    float totalFixed = 0;
    float totalFlex = 0;
    float maxChildHeight = 0;

    // First pass: layout non-flex children, sum up flex factors
    for (auto& child : children_) {
        if (child->style().flex > 0 || child->style().flexGrow > 0) {
            totalFlex += child->style().flex > 0 ? child->style().flex : child->style().flexGrow;
        } else {
            Constraints childC = {0, maxWidth, 0, maxHeight};
            child->layout(childC);
            totalFixed += child->getComputedSize().width;
            maxChildHeight = std::max(maxChildHeight, child->getComputedSize().height);
        }
    }

    float spacing = spacing_ * (children_.size() > 1 ? children_.size() - 1 : 0);
    float availableForFlex = maxWidth - totalFixed - spacing;

    // Second pass: layout flex children
    if (totalFlex > 0 && availableForFlex > 0) {
        for (auto& child : children_) {
            float flex = child->style().flex > 0 ? child->style().flex : child->style().flexGrow;
            if (flex > 0) {
                float childWidth = (flex / totalFlex) * availableForFlex;
                Constraints childC = {childWidth, childWidth, 0, maxHeight};
                child->layout(childC);
                maxChildHeight = std::max(maxChildHeight, child->getComputedSize().height);
            }
        }
    }

    // Calculate total width
    float totalWidth = 0;
    for (auto& child : children_) {
        totalWidth += child->getComputedSize().width;
    }
    totalWidth += spacing;

    float height = style_.height >= 0 ? style_.height : maxChildHeight;
    height = std::clamp(height, constraints.minHeight, constraints.maxHeight);

    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(totalWidth, constraints.minWidth, constraints.maxWidth);
    bounds_.height = height;

    // Position children
    float x = 0;

    // Main axis alignment
    float extraSpace = bounds_.width - totalWidth;
    switch (mainAlign_) {
        case MainAxisAlignment::Start: break;
        case MainAxisAlignment::Center: x = extraSpace / 2; break;
        case MainAxisAlignment::End: x = extraSpace; break;
        case MainAxisAlignment::SpaceBetween:
            if (children_.size() > 1) spacing_ = extraSpace / (children_.size() - 1);
            break;
        case MainAxisAlignment::SpaceAround:
            if (!children_.empty()) {
                float pad = extraSpace / (children_.size() * 2);
                x = pad;
                spacing_ = pad * 2;
            }
            break;
        case MainAxisAlignment::SpaceEvenly:
            if (!children_.empty()) {
                float gap = extraSpace / (children_.size() + 1);
                x = gap;
                spacing_ = gap;
            }
            break;
    }

    for (auto& child : children_) {
        float childY = 0;
        switch (crossAlign_) {
            case CrossAxisAlignment::Start: break;
            case CrossAxisAlignment::Center:
                childY = (height - child->getComputedSize().height) / 2;
                break;
            case CrossAxisAlignment::End:
                childY = height - child->getComputedSize().height;
                break;
            case CrossAxisAlignment::Stretch:
                child->setBounds({child->getBounds().x, child->getBounds().y,
                                   child->getBounds().width, height});
                break;
        }
        child->setPosition(x, childY);
        x += child->getComputedSize().width + spacing_;
    }
}

// ============================================================
// Column Layout
// ============================================================

void Column::layout(const Constraints& constraints) {
    float maxWidth = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float maxHeight = style_.height >= 0 ? style_.height : constraints.maxHeight;

    float totalFixed = 0;
    float totalFlex = 0;
    float maxChildWidth = 0;

    for (auto& child : children_) {
        if (child->style().flex > 0 || child->style().flexGrow > 0) {
            totalFlex += child->style().flex > 0 ? child->style().flex : child->style().flexGrow;
        } else {
            Constraints childC = {0, maxWidth, 0, maxHeight};
            child->layout(childC);
            totalFixed += child->getComputedSize().height;
            maxChildWidth = std::max(maxChildWidth, child->getComputedSize().width);
        }
    }

    float spacing = spacing_ * (children_.size() > 1 ? children_.size() - 1 : 0);
    float availableForFlex = maxHeight - totalFixed - spacing;

    if (totalFlex > 0 && availableForFlex > 0) {
        for (auto& child : children_) {
            float flex = child->style().flex > 0 ? child->style().flex : child->style().flexGrow;
            if (flex > 0) {
                float childHeight = (flex / totalFlex) * availableForFlex;
                Constraints childC = {0, maxWidth, childHeight, childHeight};
                child->layout(childC);
                maxChildWidth = std::max(maxChildWidth, child->getComputedSize().width);
            }
        }
    }

    float totalHeight = 0;
    for (auto& child : children_) {
        totalHeight += child->getComputedSize().height;
    }
    totalHeight += spacing;

    float width = style_.width >= 0 ? style_.width : maxChildWidth;
    width = std::clamp(width, constraints.minWidth, constraints.maxWidth);

    bounds_.width = width;
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(totalHeight, constraints.minHeight, constraints.maxHeight);

    float y = 0;
    float extraSpace = bounds_.height - totalHeight;

    switch (mainAlign_) {
        case MainAxisAlignment::Start: break;
        case MainAxisAlignment::Center: y = extraSpace / 2; break;
        case MainAxisAlignment::End: y = extraSpace; break;
        case MainAxisAlignment::SpaceBetween:
            if (children_.size() > 1) spacing_ = extraSpace / (children_.size() - 1);
            break;
        case MainAxisAlignment::SpaceAround:
            if (!children_.empty()) {
                float pad = extraSpace / (children_.size() * 2);
                y = pad;
                spacing_ = pad * 2;
            }
            break;
        case MainAxisAlignment::SpaceEvenly:
            if (!children_.empty()) {
                float gap = extraSpace / (children_.size() + 1);
                y = gap;
                spacing_ = gap;
            }
            break;
    }

    for (auto& child : children_) {
        float childX = 0;
        switch (crossAlign_) {
            case CrossAxisAlignment::Start: break;
            case CrossAxisAlignment::Center:
                childX = (width - child->getComputedSize().width) / 2;
                break;
            case CrossAxisAlignment::End:
                childX = width - child->getComputedSize().width;
                break;
            case CrossAxisAlignment::Stretch:
                child->setBounds({child->getBounds().x, child->getBounds().y,
                                   width, child->getBounds().height});
                break;
        }
        child->setPosition(childX, y);
        y += child->getComputedSize().height + spacing_;
    }
}

// ============================================================
// Stack Layout
// ============================================================

void Stack::layout(const Constraints& constraints) {
    float maxW = 0, maxH = 0;
    for (auto& child : children_) {
        child->layout(constraints);
        maxW = std::max(maxW, child->getComputedSize().width);
        maxH = std::max(maxH, child->getComputedSize().height);
    }
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(maxW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(maxH, constraints.minHeight, constraints.maxHeight);
}

// ============================================================
// Padding
// ============================================================

void Padding::layout(const Constraints& constraints) {
    float hPad = insets_.left + insets_.right;
    float vPad = insets_.top + insets_.bottom;

    Constraints inner = {
        std::max(0.0f, constraints.minWidth - hPad),
        std::max(0.0f, constraints.maxWidth - hPad),
        std::max(0.0f, constraints.minHeight - vPad),
        std::max(0.0f, constraints.maxHeight - vPad)
    };

    for (auto& child : children_) {
        child->layout(inner);
        child->setPosition(insets_.left, insets_.top);
    }

    float childW = children_.empty() ? 0 : children_[0]->getComputedSize().width;
    float childH = children_.empty() ? 0 : children_[0]->getComputedSize().height;

    bounds_.width = std::clamp(childW + hPad, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(childH + vPad, constraints.minHeight, constraints.maxHeight);
}

// ============================================================
// Center
// ============================================================

void Center::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;

    Constraints loose = constraints.loosen();
    for (auto& child : children_) {
        child->layout(loose);
        float cx = (bounds_.width - child->getComputedSize().width) / 2;
        float cy = (bounds_.height - child->getComputedSize().height) / 2;
        child->setPosition(cx, cy);
    }
}

// ============================================================
// ScrollView
// ============================================================

void ScrollView::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;

    // Layout children with unbounded height (if vertical scroll)
    Constraints childC = {0, bounds_.width, 0, 1e6f};
    contentHeight_ = 0;
    contentWidth_ = 0;

    for (auto& child : children_) {
        child->layout(childC);
        contentHeight_ += child->getComputedSize().height;
        contentWidth_ = std::max(contentWidth_, child->getComputedSize().width);
    }

    // Position children
    float y = -scrollY_;
    for (auto& child : children_) {
        child->setPosition(-scrollX_, y);
        y += child->getComputedSize().height;
    }
}

void ScrollView::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    renderer.clipRect({0, 0, bounds_.width, bounds_.height});

    paintBackground(renderer);
    paintChildren(renderer);

    // Draw scrollbar if content overflows
    if (contentHeight_ > bounds_.height) {
        float barHeight = (bounds_.height / contentHeight_) * bounds_.height;
        float barY = (scrollY_ / contentHeight_) * bounds_.height;
        Rect scrollbar = {bounds_.width - 6, barY, 4, barHeight};
        renderer.fillRoundedRect(scrollbar, BorderRadius::all(2), Color::rgba(0, 0, 0, 80));
    }

    renderer.restore();
}

bool ScrollView::handleEvent(const Event& event) {
    if (event.type == Event::MouseScroll) {
        auto& se = event.mouseScroll();
        if (hitTest(se.x, se.y)) {
            scrollY_ -= se.scrollY * 30;
            scrollY_ = std::max(0.0f, std::min(scrollY_, contentHeight_ - bounds_.height));
            markDirty();
            return true;
        }
    }
    return Widget::handleEvent(event);
}

// ============================================================
// Text Widget
// ============================================================

void Text::layout(const Constraints& constraints) {
    if (text_.empty()) {
        bounds_.width = 0;
        bounds_.height = font_.size;
        return;
    }
    // Use a dummy renderer to measure — will be set during paint
    // For now, estimate based on font size
    float estimatedWidth = text_.length() * font_.size * 0.6f;
    float estimatedHeight = font_.size * 1.4f;

    bounds_.width = std::clamp(estimatedWidth, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(estimatedHeight, constraints.minHeight, constraints.maxHeight);
}

void Text::paint(IRenderer& renderer) {
    if (!visible_ || text_.empty()) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Re-measure with actual renderer
    TextMetrics metrics = renderer.measureText(text_, font_);
    bounds_.width = std::max(bounds_.width, metrics.width);
    bounds_.height = std::max(bounds_.height, metrics.height);

    paintBackground(renderer);

    float textX = 0;
    if (align_ == TextAlign::Center) textX = (bounds_.width - metrics.width) / 2;
    else if (align_ == TextAlign::Right) textX = bounds_.width - metrics.width;

    // Use theme text color if the widget color is still the default black
    Color textColor = color_;
    if (auto* theme = activeTheme()) {
        if (color_.r == 0 && color_.g == 0 && color_.b == 0 && color_.a == 255) {
            textColor = theme->textPrimary;
        }
    }
    renderer.drawText(text_, textX, 0, font_, textColor);

    renderer.restore();
}

// ============================================================
// Button Widget
// ============================================================

void Button::layout(const Constraints& constraints) {
    float textW = label_.length() * font_.size * 0.6f;
    float textH = font_.size * 1.4f;

    float padH = 24.0f;  // horizontal padding
    float padV = 12.0f;  // vertical padding

    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(textW + padH * 2, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(textH + padV * 2, constraints.minHeight, constraints.maxHeight);
}

void Button::paint(IRenderer& renderer) {
    if (!visible_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Choose color based on state
    Color bgColor = normalColor_;
    if (style_.backgroundColor.a > 0) bgColor = style_.backgroundColor;

    switch (state_) {
        case WidgetState::Hovered: bgColor = hoverColor_; break;
        case WidgetState::Pressed: bgColor = pressedColor_; break;
        case WidgetState::Disabled: bgColor = disabledColor_; break;
        default: break;
    }

    // Background
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), bgColor);

    // Label
    TextMetrics metrics = renderer.measureText(label_, font_);
    float textX = (bounds_.width - metrics.width) / 2;
    float textY = (bounds_.height - metrics.height) / 2;
    renderer.drawText(label_, textX, textY, font_, textColor_);

    renderer.restore();
}

bool Button::handleEvent(const Event& event) {
    return Widget::handleEvent(event);
}

// ============================================================
// Image Widget
// ============================================================

void Image::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;
}

void Image::paint(IRenderer& renderer) {
    if (!visible_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    if (imageId_ < 0 && !path_.empty()) {
        imageId_ = renderer.loadImage(path_);
    }

    if (imageId_ >= 0) {
        renderer.drawImage(imageId_, {0, 0, bounds_.width, bounds_.height});
    } else {
        // Placeholder
        renderer.fillRect({0, 0, bounds_.width, bounds_.height}, Color::rgb(230, 230, 230));
        renderer.drawText("?", bounds_.width / 2 - 5, bounds_.height / 2 - 8,
                           {"sans-serif", 16, FontWeight::Regular, FontStyle::Normal},
                           Color::gray());
    }

    renderer.restore();
}

// ============================================================
// Box Widget
// ============================================================

void Box::layout(const Constraints& constraints) {
    float maxChildW = 0, maxChildH = 0;

    for (auto& child : children_) {
        Constraints childC = {0, constraints.maxWidth, 0, constraints.maxHeight};
        child->layout(childC);
        maxChildW = std::max(maxChildW, child->getComputedSize().width);
        maxChildH = std::max(maxChildH, child->getComputedSize().height);
    }

    float padH = style_.padding.left + style_.padding.right;
    float padV = style_.padding.top + style_.padding.bottom;

    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(maxChildW + padH, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(maxChildH + padV, constraints.minHeight, constraints.maxHeight);

    for (auto& child : children_) {
        child->setPosition(style_.padding.left, style_.padding.top);
    }
}

void Box::paint(IRenderer& renderer) {
    Widget::paint(renderer);
}

// ============================================================
// TextField Widget
// ============================================================

void TextField::layout(const Constraints& constraints) {
    // Fill available width when not explicitly sized; fall back to 200 if unconstrained
    float defaultW = constraints.maxWidth < 1e5f ? constraints.maxWidth : 200.0f;
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(defaultW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(40.0f, constraints.minHeight, constraints.maxHeight);
}

void TextField::paint(IRenderer& renderer) {
    if (!visible_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Theme-aware colors
    const Theme* theme = activeTheme();
    Color surfaceColor = theme ? theme->surface : Color::white();
    Color surfaceDim = theme ? Color::rgb(
        std::min(255, (int)surfaceColor.r + 10),
        std::min(255, (int)surfaceColor.g + 10),
        std::min(255, (int)surfaceColor.b + 10)) : Color::rgb(245, 245, 245);
    Color primaryColor = theme ? theme->primary : Color::rgb(33, 150, 243);
    Color borderNormal = theme ? theme->divider : Color::rgb(200, 200, 200);
    Color hintColor = theme ? theme->textHint : Color::rgb(158, 158, 158);
    Color textColor = theme ? theme->textPrimary : Color::black();

    // Background
    Color bg = state_ == WidgetState::Focused ? surfaceColor : surfaceDim;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), bg);

    // Border
    Color borderColor = state_ == WidgetState::Focused ? primaryColor : borderNormal;
    renderer.drawRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), borderColor);

    // Text or placeholder
    float textX = 8;
    float textY = (bounds_.height - font_.size * 1.4f) / 2;

    if (value_.empty() && !placeholder_.empty()) {
        renderer.drawText(placeholder_, textX, textY, font_, hintColor);
    } else {
        renderer.drawText(value_, textX, textY, font_, textColor);

        // Cursor
        if (state_ == WidgetState::Focused && cursorVisible_) {
            std::string beforeCursor = value_.substr(0, cursorPos_);
            TextMetrics m = renderer.measureText(beforeCursor, font_);
            renderer.drawLine(textX + m.width, textY + 2,
                               textX + m.width, textY + font_.size + 2,
                               textColor, 1.5f);
        }
    }

    renderer.restore();
}

bool TextField::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && me.pressed) {
            state_ = WidgetState::Focused;
            SDL_StartTextInput();
            return true;
        } else if (!hitTest(me.x, me.y) && me.pressed) {
            state_ = WidgetState::Normal;
            SDL_StopTextInput();
        }
    }

    if (state_ != WidgetState::Focused) return false;

    if (event.type == Event::TextInput) {
        auto& te = event.textInput();
        value_.insert(cursorPos_, te.text);
        cursorPos_ += te.text.length();
        if (onChange_) onChange_(value_);
        markDirty();
        return true;
    }

    if (event.type == Event::Key && event.key().pressed) {
        auto& ke = event.key();
        if (ke.key == KeyCode::Backspace && cursorPos_ > 0) {
            value_.erase(cursorPos_ - 1, 1);
            cursorPos_--;
            if (onChange_) onChange_(value_);
            markDirty();
            return true;
        }
        if (ke.key == KeyCode::Delete && cursorPos_ < (int)value_.size()) {
            value_.erase(cursorPos_, 1);
            if (onChange_) onChange_(value_);
            markDirty();
            return true;
        }
        if (ke.key == KeyCode::Left && cursorPos_ > 0) { cursorPos_--; return true; }
        if (ke.key == KeyCode::Right && cursorPos_ < (int)value_.size()) { cursorPos_++; return true; }
        if (ke.key == KeyCode::Enter && onSubmit_) { onSubmit_(); return true; }
        if (ke.key == KeyCode::Home) { cursorPos_ = 0; return true; }
        if (ke.key == KeyCode::End) { cursorPos_ = value_.size(); return true; }
    }

    return false;
}

// ============================================================
// Checkbox Widget
// ============================================================

void Checkbox::layout(const Constraints& constraints) {
    float boxSize = 20;
    float textW = label_.length() * 8.0f; // rough estimate
    bounds_.width = std::clamp(boxSize + 8 + textW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(std::max(boxSize, 20.0f), constraints.minHeight, constraints.maxHeight);
}

void Checkbox::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float boxSize = 18;
    float boxY = (bounds_.height - boxSize) / 2;

    // Theme-aware colors
    const Theme* theme = activeTheme();
    Color uncheckedBorder = theme ? theme->textHint : Color::rgb(158, 158, 158);
    Color labelColor = theme ? theme->textPrimary : Color::black();

    // Box
    if (checked_) {
        renderer.fillRoundedRect({0, boxY, boxSize, boxSize},
                                  BorderRadius::all(3), checkColor_);
        // Checkmark
        renderer.drawLine(4, boxY + boxSize / 2, 7, boxY + boxSize - 5, Color::white(), 2);
        renderer.drawLine(7, boxY + boxSize - 5, 14, boxY + 4, Color::white(), 2);
    } else {
        renderer.drawRoundedRect({0, boxY, boxSize, boxSize},
                                  BorderRadius::all(3), uncheckedBorder);
    }

    // Label
    if (!label_.empty()) {
        FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};
        renderer.drawText(label_, boxSize + 8, (bounds_.height - 14) / 2, font, labelColor);
    }

    renderer.restore();
}

bool Checkbox::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && !me.pressed) {
            checked_ = !checked_;
            if (onChange_) onChange_(checked_);
            markDirty();
            return true;
        }
    }
    return Widget::handleEvent(event);
}

// ============================================================
// Slider Widget
// ============================================================

void Slider::layout(const Constraints& constraints) {
    float defaultW = constraints.maxWidth < 1e5f ? constraints.maxWidth : 200.0f;
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(defaultW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(32.0f, constraints.minHeight, constraints.maxHeight);
}

void Slider::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float trackH = 4;
    float trackY = (bounds_.height - trackH) / 2;
    float thumbR = 10;
    float progress = (value_ - min_) / (max_ - min_);
    float thumbX = progress * (bounds_.width - thumbR * 2) + thumbR;

    // Theme-aware track color
    const Theme* theme = activeTheme();
    Color trackBg = trackColor_;
    if (theme && trackColor_.r == 200 && trackColor_.g == 200 && trackColor_.b == 200) {
        trackBg = theme->divider;
    }

    // Track background
    renderer.fillRoundedRect({0, trackY, bounds_.width, trackH},
                              BorderRadius::all(2), trackBg);

    // Active track
    renderer.fillRoundedRect({0, trackY, thumbX, trackH},
                              BorderRadius::all(2), activeColor_);

    // Thumb shadow
    renderer.drawShadow({thumbX - thumbR, bounds_.height / 2 - thumbR, thumbR * 2, thumbR * 2},
                          BorderRadius::all(thumbR), {0, 1, 3, Color::rgba(0, 0, 0, 40)});

    // Thumb
    Color thumbBg = thumbColor_;
    if (theme && thumbColor_.r == 255 && thumbColor_.g == 255 && thumbColor_.b == 255) {
        thumbBg = theme->surface;
    }
    renderer.fillCircle(thumbX, bounds_.height / 2, thumbR, thumbBg);
    renderer.drawCircle(thumbX, bounds_.height / 2, thumbR, activeColor_);

    renderer.restore();
}

bool Slider::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y)) {
            dragging_ = me.pressed;
            if (me.pressed) {
                float localX = me.x - bounds_.x;
                float progress = std::clamp(localX / bounds_.width, 0.0f, 1.0f);
                value_ = min_ + progress * (max_ - min_);
                if (onChange_) onChange_(value_);
                markDirty();
            }
            return true;
        }
        if (!me.pressed) dragging_ = false;
    }

    if (event.type == Event::MouseMove && dragging_) {
        auto& me = event.mouseMove();
        float localX = me.x - bounds_.x;
        float progress = std::clamp(localX / bounds_.width, 0.0f, 1.0f);
        value_ = min_ + progress * (max_ - min_);
        if (onChange_) onChange_(value_);
        markDirty();
        return true;
    }

    return false;
}

// ============================================================
// Switch Widget
// ============================================================

void Switch::layout(const Constraints& constraints) {
    bounds_.width = std::clamp(48.0f, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(28.0f, constraints.minHeight, constraints.maxHeight);
}

void Switch::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    Color bgColor = on_ ? onColor_ : offColor_;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(bounds_.height / 2), bgColor);

    // Thumb
    float thumbR = bounds_.height / 2 - 3;
    float thumbX = on_ ? bounds_.width - thumbR - 3 : thumbR + 3;
    renderer.fillCircle(thumbX, bounds_.height / 2, thumbR, Color::white());

    renderer.restore();
}

bool Switch::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && !me.pressed) {
            on_ = !on_;
            if (onChange_) onChange_(on_);
            markDirty();
            return true;
        }
    }
    return Widget::handleEvent(event);
}

// ============================================================
// AppBar Widget
// ============================================================

void AppBar::layout(const Constraints& constraints) {
    bounds_.width = constraints.maxWidth;
    bounds_.height = height_;
}

void AppBar::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Shadow
    renderer.drawShadow({0, 0, bounds_.width, bounds_.height},
                          BorderRadius::all(0), {0, 2, 4, Color::rgba(0, 0, 0, 40)});

    // Background
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, barColor_);

    // Title
    FontSpec titleFont = {"sans-serif", 20, FontWeight::Medium, FontStyle::Normal};
    TextMetrics m = renderer.measureText(title_, titleFont);
    renderer.drawText(title_, 16, (bounds_.height - m.height) / 2, titleFont, titleColor_);

    // Paint action children on the right
    float rightX = bounds_.width - 16;
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        rightX -= (*it)->getComputedSize().width;
        (*it)->setPosition(rightX, (bounds_.height - (*it)->getComputedSize().height) / 2);
        rightX -= 8;
    }
    paintChildren(renderer);

    renderer.restore();
}

// ============================================================
// Dialog Widget
// ============================================================

void Dialog::layout(const Constraints& constraints) {
    if (!open_) return;
    bounds_.width = constraints.maxWidth;
    bounds_.height = constraints.maxHeight;

    float dialogW = std::min(400.0f, bounds_.width * 0.8f);
    float dialogH = 200.0f;

    Constraints childC = {0, dialogW - 32, 0, dialogH - 80};
    for (auto& child : children_) {
        child->layout(childC);
        dialogH = std::max(dialogH, child->getComputedSize().height + 80);
    }

    // Center children
    float startY = 56;
    for (auto& child : children_) {
        child->setPosition((dialogW - child->getComputedSize().width) / 2, startY);
        startY += child->getComputedSize().height + 8;
    }
}

void Dialog::paint(IRenderer& renderer) {
    if (!visible_ || !open_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Overlay
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, Color::rgba(0, 0, 0, 100));

    float dialogW = std::min(400.0f, bounds_.width * 0.8f);
    float dialogH = 200.0f;
    float dialogX = (bounds_.width - dialogW) / 2;
    float dialogY = (bounds_.height - dialogH) / 2;

    // Shadow + background
    renderer.drawShadow({dialogX, dialogY, dialogW, dialogH},
                          BorderRadius::all(8), {0, 8, 24, Color::rgba(0, 0, 0, 60)});
    renderer.fillRoundedRect({dialogX, dialogY, dialogW, dialogH},
                              BorderRadius::all(8), Color::white());

    // Title
    FontSpec titleFont = {"sans-serif", 18, FontWeight::Medium, FontStyle::Normal};
    renderer.drawText(title_, dialogX + 16, dialogY + 16, titleFont, Color::black());

    // Divider
    renderer.drawLine(dialogX, dialogY + 48, dialogX + dialogW, dialogY + 48,
                       Color::rgb(224, 224, 224));

    renderer.save();
    renderer.translate(dialogX, dialogY);
    paintChildren(renderer);
    renderer.restore();

    renderer.restore();
}

bool Dialog::handleEvent(const Event& event) {
    if (!open_) return false;

    // Click on overlay closes dialog
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        float dialogW = std::min(400.0f, bounds_.width * 0.8f);
        float dialogH = 200.0f;
        float dialogX = (bounds_.width - dialogW) / 2;
        float dialogY = (bounds_.height - dialogH) / 2;

        Rect dialogBounds = {dialogX, dialogY, dialogW, dialogH};
        if (!dialogBounds.contains(me.x, me.y) && !me.pressed) {
            if (onClose_) onClose_();
            return true;
        }
    }

    if (event.type == Event::Key && event.key().key == KeyCode::Escape && event.key().pressed) {
        if (onClose_) onClose_();
        return true;
    }

    return Widget::handleEvent(event);
}

// ============================================================
// ListView Widget
// ============================================================

void ListView::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;
}

void ListView::paint(IRenderer& renderer) {
    if (!visible_ || !itemBuilder_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    renderer.clipRect({0, 0, bounds_.width, bounds_.height});

    int firstVisible = static_cast<int>(scrollOffset_ / itemHeight_);
    int visibleCount = static_cast<int>(bounds_.height / itemHeight_) + 2;

    for (int i = firstVisible; i < std::min(firstVisible + visibleCount, itemCount_); i++) {
        auto widget = itemBuilder_(i);
        if (widget) {
            float y = i * itemHeight_ - scrollOffset_;
            Constraints c = {bounds_.width, bounds_.width, itemHeight_, itemHeight_};
            widget->layout(c);
            widget->setPosition(0, y);
            widget->paint(renderer);
        }
    }

    // Scrollbar
    if (itemCount_ * itemHeight_ > bounds_.height) {
        float totalH = itemCount_ * itemHeight_;
        float barH = (bounds_.height / totalH) * bounds_.height;
        float barY = (scrollOffset_ / totalH) * bounds_.height;
        renderer.fillRoundedRect({bounds_.width - 6, barY, 4, barH},
                                  BorderRadius::all(2), Color::rgba(0, 0, 0, 80));
    }

    renderer.restore();
}

bool ListView::handleEvent(const Event& event) {
    if (event.type == Event::MouseScroll) {
        auto& se = event.mouseScroll();
        if (hitTest(se.x, se.y)) {
            scrollOffset_ -= se.scrollY * 30;
            float maxScroll = std::max(0.0f, itemCount_ * itemHeight_ - bounds_.height);
            scrollOffset_ = std::clamp(scrollOffset_, 0.0f, maxScroll);
            markDirty();
            return true;
        }
    }
    return Widget::handleEvent(event);
}

// ============================================================
// Grid Layout
// ============================================================

void Grid::layout(const Constraints& constraints) {
    if (children_.empty() || columns_ <= 0) return;

    float availWidth = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float cellWidth = (availWidth - spacing_ * (columns_ - 1)) / columns_;

    int row = 0, col = 0;
    float maxRowHeight = 0;
    float totalHeight = 0;

    for (auto& child : children_) {
        Constraints cellC = {cellWidth, cellWidth, 0, constraints.maxHeight};
        child->layout(cellC);

        float x = col * (cellWidth + spacing_);
        float y = totalHeight;
        child->setPosition(x, y);

        maxRowHeight = std::max(maxRowHeight, child->getComputedSize().height);

        col++;
        if (col >= columns_) {
            col = 0;
            totalHeight += maxRowHeight + spacing_;
            maxRowHeight = 0;
        }
    }
    if (col > 0) totalHeight += maxRowHeight;

    bounds_.width = availWidth;
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(totalHeight, constraints.minHeight, constraints.maxHeight);
}

// ============================================================
// TextArea Widget
// ============================================================

std::vector<std::string> TextArea::getLines() const {
    std::vector<std::string> lines;
    std::string line;
    for (char c : value_) {
        if (c == '\n') {
            lines.push_back(line);
            line.clear();
        } else {
            line += c;
        }
    }
    lines.push_back(line);
    return lines;
}

void TextArea::updateCursorLineCol() {
    int pos = 0;
    cursorLine_ = 0;
    cursorCol_ = 0;
    for (char c : value_) {
        if (pos >= cursorPos_) break;
        if (c == '\n') {
            cursorLine_++;
            cursorCol_ = 0;
        } else {
            cursorCol_++;
        }
        pos++;
    }
}

void TextArea::layout(const Constraints& constraints) {
    float defaultW = constraints.maxWidth < 1e5f ? constraints.maxWidth : 300.0f;
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(defaultW, constraints.minWidth, constraints.maxWidth);
    float defaultH = constraints.maxHeight < 1e5f ? std::min(120.0f, constraints.maxHeight) : 120.0f;
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(defaultH, constraints.minHeight, constraints.maxHeight);
}

void TextArea::paint(IRenderer& renderer) {
    if (!visible_) return;

    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Background
    Color bg = state_ == WidgetState::Focused ? Color::white() : Color::rgb(245, 245, 245);
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), bg);

    // Border
    Color borderColor = state_ == WidgetState::Focused ?
                         Color::rgb(33, 150, 243) : Color::rgb(200, 200, 200);
    renderer.drawRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), borderColor);

    // Clip content area
    renderer.clipRect({4, 4, bounds_.width - 8, bounds_.height - 8});

    float textX = 8;
    float lineHeight = font_.size > 0 ? font_.size * 1.4f : 14.0f * 1.4f;

    if (value_.empty() && !placeholder_.empty()) {
        renderer.drawText(placeholder_, textX, 6 - scrollY_, font_, Color::rgb(158, 158, 158));
    } else {
        auto lines = getLines();
        for (size_t i = 0; i < lines.size(); i++) {
            float y = 6 + i * lineHeight - scrollY_;
            if (y + lineHeight < 0) continue;
            if (y > bounds_.height) break;
            renderer.drawText(lines[i], textX, y, font_, Color::black());
        }

        // Cursor
        if (state_ == WidgetState::Focused && cursorVisible_) {
            updateCursorLineCol();
            auto lines2 = getLines();
            std::string beforeCursor = (cursorLine_ < (int)lines2.size()) ?
                lines2[cursorLine_].substr(0, cursorCol_) : "";
            TextMetrics m = renderer.measureText(beforeCursor, font_);
            float cursorY = 6 + cursorLine_ * lineHeight - scrollY_;
            renderer.drawLine(textX + m.width, cursorY + 2,
                               textX + m.width, cursorY + lineHeight - 2,
                               Color::black(), 1.5f);
        }
    }

    renderer.restore();
}

bool TextArea::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && me.pressed) {
            state_ = WidgetState::Focused;
            SDL_StartTextInput();
            return true;
        } else if (!hitTest(me.x, me.y) && me.pressed) {
            state_ = WidgetState::Normal;
            SDL_StopTextInput();
        }
    }

    if (state_ != WidgetState::Focused) return false;

    if (event.type == Event::TextInput) {
        auto& te = event.textInput();
        value_.insert(cursorPos_, te.text);
        cursorPos_ += te.text.length();
        if (onChange_) onChange_(value_);
        markDirty();
        return true;
    }

    if (event.type == Event::Key && event.key().pressed) {
        auto& ke = event.key();
        if (ke.key == KeyCode::Backspace && cursorPos_ > 0) {
            value_.erase(cursorPos_ - 1, 1);
            cursorPos_--;
            if (onChange_) onChange_(value_);
            markDirty();
            return true;
        }
        if (ke.key == KeyCode::Delete && cursorPos_ < (int)value_.size()) {
            value_.erase(cursorPos_, 1);
            if (onChange_) onChange_(value_);
            markDirty();
            return true;
        }
        if (ke.key == KeyCode::Enter) {
            value_.insert(cursorPos_, 1, '\n');
            cursorPos_++;
            if (onChange_) onChange_(value_);
            markDirty();
            return true;
        }
        if (ke.key == KeyCode::Left && cursorPos_ > 0) { cursorPos_--; markDirty(); return true; }
        if (ke.key == KeyCode::Right && cursorPos_ < (int)value_.size()) { cursorPos_++; markDirty(); return true; }
        if (ke.key == KeyCode::Home) { cursorPos_ = 0; markDirty(); return true; }
        if (ke.key == KeyCode::End) { cursorPos_ = value_.size(); markDirty(); return true; }
    }

    if (event.type == Event::MouseScroll) {
        auto& se = event.mouseScroll();
        if (hitTest(se.x, se.y)) {
            float lineHeight = font_.size > 0 ? font_.size * 1.4f : 14.0f * 1.4f;
            float contentHeight = getLines().size() * lineHeight;
            scrollY_ -= se.scrollY * 30;
            scrollY_ = std::max(0.0f, std::min(scrollY_, contentHeight - bounds_.height + 16));
            markDirty();
            return true;
        }
    }

    return false;
}

// ============================================================
// RadioButton Widget
// ============================================================

void RadioButton::deselectGroup(Widget* root, const std::string& group, RadioButton* except) {
    if (!root) return;
    auto* rb = dynamic_cast<RadioButton*>(root);
    if (rb && rb != except && rb->group_ == group) {
        rb->selected_ = false;
        rb->markDirty();
    }
    for (const auto& child : root->getChildren()) {
        deselectGroup(child.get(), group, except);
    }
}

void RadioButton::layout(const Constraints& constraints) {
    float circleSize = 20;
    float textW = label_.length() * 8.0f;
    bounds_.width = std::clamp(circleSize + 8 + textW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(std::max(circleSize, 20.0f), constraints.minHeight, constraints.maxHeight);
}

void RadioButton::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float radius = 9;
    float centerY = bounds_.height / 2;

    // Outer circle
    renderer.drawCircle(radius, centerY, radius,
                         selected_ ? activeColor_ : Color::rgb(158, 158, 158));

    // Inner filled circle when selected
    if (selected_) {
        renderer.fillCircle(radius, centerY, 5, activeColor_);
    }

    // Label
    if (!label_.empty()) {
        FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};
        renderer.drawText(label_, radius * 2 + 8, (bounds_.height - 14) / 2, font, Color::black());
    }

    renderer.restore();
}

bool RadioButton::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && !me.pressed && !selected_) {
            // Deselect others in the same group
            Widget* root = this;
            while (root->getParent()) root = root->getParent();
            deselectGroup(root, group_, this);

            selected_ = true;
            if (onChange_) onChange_(selected_);
            markDirty();
            return true;
        }
    }
    return Widget::handleEvent(event);
}

// ============================================================
// Dropdown Widget
// ============================================================

void Dropdown::layout(const Constraints& constraints) {
    float defaultW = constraints.maxWidth < 1e5f ? constraints.maxWidth : 200.0f;
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(defaultW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(40.0f, constraints.minHeight, constraints.maxHeight);
}

void Dropdown::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Button area
    Color bg = open_ ? Color::rgb(235, 235, 235) : Color::rgb(245, 245, 245);
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), bg);
    renderer.drawRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), Color::rgb(200, 200, 200));

    // Selected text or placeholder
    FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};
    std::string displayText = (selectedIndex_ >= 0 && selectedIndex_ < (int)items_.size()) ?
                               items_[selectedIndex_] : placeholder_;
    Color textColor = (selectedIndex_ >= 0) ? Color::black() : Color::rgb(158, 158, 158);
    renderer.drawText(displayText, 12, (bounds_.height - 14) / 2, font, textColor);

    // Dropdown arrow
    float arrowX = bounds_.width - 20;
    float arrowY = bounds_.height / 2;
    renderer.drawLine(arrowX - 4, arrowY - 2, arrowX, arrowY + 3, Color::rgb(100, 100, 100), 2);
    renderer.drawLine(arrowX, arrowY + 3, arrowX + 4, arrowY - 2, Color::rgb(100, 100, 100), 2);

    // Dropdown list overlay
    if (open_ && !items_.empty()) {
        float itemH = 36.0f;
        float listH = items_.size() * itemH;

        // Shadow
        renderer.drawShadow({0, bounds_.height, bounds_.width, listH},
                              BorderRadius::all(4), {0, 4, 12, Color::rgba(0, 0, 0, 40)});

        // Background
        renderer.fillRoundedRect({0, bounds_.height, bounds_.width, listH},
                                  BorderRadius::all(4), Color::white());
        renderer.drawRoundedRect({0, bounds_.height, bounds_.width, listH},
                                  BorderRadius::all(4), Color::rgb(200, 200, 200));

        for (int i = 0; i < (int)items_.size(); i++) {
            float y = bounds_.height + i * itemH;
            if (i == hoveredIndex_) {
                renderer.fillRect({1, y, bounds_.width - 2, itemH}, Color::rgb(230, 230, 250));
            }
            if (i == selectedIndex_) {
                renderer.drawText(items_[i], 12, y + (itemH - 14) / 2, font, Color::rgb(33, 150, 243));
            } else {
                renderer.drawText(items_[i], 12, y + (itemH - 14) / 2, font, Color::black());
            }
        }
    }

    renderer.restore();
}

bool Dropdown::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (!me.pressed) {
            if (open_) {
                // Check if clicking on an item
                float itemH = 36.0f;
                float localY = me.y - bounds_.y - bounds_.height;
                if (localY >= 0 && me.x >= bounds_.x && me.x <= bounds_.x + bounds_.width) {
                    int idx = static_cast<int>(localY / itemH);
                    if (idx >= 0 && idx < (int)items_.size()) {
                        selectedIndex_ = idx;
                        if (onChange_) onChange_(items_[idx]);
                    }
                }
                open_ = false;
                markDirty();
                return true;
            } else if (hitTest(me.x, me.y)) {
                open_ = true;
                markDirty();
                return true;
            }
        }
    }

    if (event.type == Event::MouseMove && open_) {
        auto& me = event.mouseMove();
        float itemH = 36.0f;
        float localY = me.y - bounds_.y - bounds_.height;
        if (localY >= 0 && me.x >= bounds_.x && me.x <= bounds_.x + bounds_.width) {
            int idx = static_cast<int>(localY / itemH);
            if (idx >= 0 && idx < (int)items_.size() && idx != hoveredIndex_) {
                hoveredIndex_ = idx;
                markDirty();
            }
        } else {
            if (hoveredIndex_ != -1) {
                hoveredIndex_ = -1;
                markDirty();
            }
        }
    }

    return Widget::handleEvent(event);
}

// ============================================================
// Icon Widget
// ============================================================

void Icon::layout(const Constraints& constraints) {
    bounds_.width = std::clamp(size_, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(size_, constraints.minHeight, constraints.maxHeight);
}

void Icon::paint(IRenderer& renderer) {
    if (!visible_ || icon_.empty()) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    FontSpec iconFont = {"sans-serif", size_, FontWeight::Regular, FontStyle::Normal};
    TextMetrics m = renderer.measureText(icon_, iconFont);
    float x = (bounds_.width - m.width) / 2;
    float y = (bounds_.height - m.height) / 2;
    renderer.drawText(icon_, x, y, iconFont, color_);

    renderer.restore();
}

// ============================================================
// Drawer Widget
// ============================================================

void Drawer::layout(const Constraints& constraints) {
    bounds_.width = constraints.maxWidth;
    bounds_.height = constraints.maxHeight;

    if (open_) {
        // Layout children within drawer area
        Constraints childC = {0, drawerWidth_ - 16, 0, bounds_.height};
        float y = 8;
        for (auto& child : children_) {
            child->layout(childC);
            child->setPosition(8, y);
            y += child->getComputedSize().height + 8;
        }
    }
}

void Drawer::paint(IRenderer& renderer) {
    if (!visible_ || !open_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Backdrop
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, backdropColor_);

    // Drawer panel
    renderer.drawShadow({0, 0, drawerWidth_, bounds_.height},
                          BorderRadius::all(0), {4, 0, 16, Color::rgba(0, 0, 0, 60)});
    renderer.fillRect({0, 0, drawerWidth_, bounds_.height}, Color::white());

    // Paint children within drawer
    renderer.clipRect({0, 0, drawerWidth_, bounds_.height});
    paintChildren(renderer);

    renderer.restore();
}

bool Drawer::handleEvent(const Event& event) {
    if (!open_) return false;

    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        // Click on backdrop closes drawer
        if (me.x > drawerWidth_ && !me.pressed) {
            if (onClose_) onClose_();
            return true;
        }
    }

    if (event.type == Event::Key && event.key().key == KeyCode::Escape && event.key().pressed) {
        if (onClose_) onClose_();
        return true;
    }

    return Widget::handleEvent(event);
}

// ============================================================
// TabBar Widget
// ============================================================

void TabBar::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(48.0f, constraints.minHeight, constraints.maxHeight);
}

void TabBar::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Background
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, Color::rgb(245, 245, 245));

    if (tabs_.empty()) {
        renderer.restore();
        return;
    }

    float tabWidth = bounds_.width / tabs_.size();
    FontSpec font = {"sans-serif", 14, FontWeight::Medium, FontStyle::Normal};

    for (int i = 0; i < (int)tabs_.size(); i++) {
        float x = i * tabWidth;
        Color textColor = (i == activeTab_) ? activeColor_ : inactiveColor_;

        if (i == hoveredTab_ && i != activeTab_) {
            renderer.fillRect({x, 0, tabWidth, bounds_.height}, Color::rgb(235, 235, 235));
        }

        TextMetrics m = renderer.measureText(tabs_[i], font);
        float textX = x + (tabWidth - m.width) / 2;
        float textY = (bounds_.height - m.height) / 2;
        renderer.drawText(tabs_[i], textX, textY, font, textColor);

        // Active indicator
        if (i == activeTab_) {
            renderer.fillRect({x, bounds_.height - 3, tabWidth, 3}, activeColor_);
        }
    }

    // Bottom border
    renderer.drawLine(0, bounds_.height - 1, bounds_.width, bounds_.height - 1,
                       Color::rgb(224, 224, 224));

    renderer.restore();
}

bool TabBar::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y) && !me.pressed && !tabs_.empty()) {
            float tabWidth = bounds_.width / tabs_.size();
            int idx = static_cast<int>((me.x - bounds_.x) / tabWidth);
            if (idx >= 0 && idx < (int)tabs_.size() && idx != activeTab_) {
                activeTab_ = idx;
                if (onTabChange_) onTabChange_(activeTab_);
                markDirty();
                return true;
            }
        }
    }

    if (event.type == Event::MouseMove) {
        auto& me = event.mouseMove();
        if (hitTest(me.x, me.y) && !tabs_.empty()) {
            float tabWidth = bounds_.width / tabs_.size();
            int idx = static_cast<int>((me.x - bounds_.x) / tabWidth);
            if (idx >= 0 && idx < (int)tabs_.size() && idx != hoveredTab_) {
                hoveredTab_ = idx;
                markDirty();
            }
        } else if (hoveredTab_ != -1) {
            hoveredTab_ = -1;
            markDirty();
        }
    }

    return Widget::handleEvent(event);
}

// ============================================================
// Menu Widget
// ============================================================

void Menu::layout(const Constraints& constraints) {
    if (!open_) return;
    float maxW = 180.0f;
    FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};

    // Calculate width based on item text
    for (const auto& item : items_) {
        if (!item.separator) {
            float w = item.label.length() * 8.0f + 32;
            maxW = std::max(maxW, w);
        }
    }

    bounds_.width = maxW;
    bounds_.height = items_.size() * itemHeight_;
    bounds_.x = posX_;
    bounds_.y = posY_;
}

void Menu::paint(IRenderer& renderer) {
    if (!visible_ || !open_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Shadow
    renderer.drawShadow({0, 0, bounds_.width, bounds_.height},
                          BorderRadius::all(4), {0, 4, 16, Color::rgba(0, 0, 0, 50)});

    // Background
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), Color::white());
    renderer.drawRoundedRect({0, 0, bounds_.width, bounds_.height},
                              BorderRadius::all(4), Color::rgb(224, 224, 224));

    FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};

    for (int i = 0; i < (int)items_.size(); i++) {
        float y = i * itemHeight_;

        if (items_[i].separator) {
            renderer.drawLine(8, y + itemHeight_ / 2, bounds_.width - 8,
                               y + itemHeight_ / 2, Color::rgb(224, 224, 224));
            continue;
        }

        if (i == hoveredIndex_ && items_[i].enabled) {
            renderer.fillRect({1, y, bounds_.width - 2, itemHeight_}, Color::rgb(230, 230, 250));
        }

        Color textColor = items_[i].enabled ? Color::black() : Color::rgb(189, 189, 189);

        // Icon
        if (!items_[i].icon.empty()) {
            renderer.drawText(items_[i].icon, 8, y + (itemHeight_ - 14) / 2, font, textColor);
        }

        // Label
        float textX = items_[i].icon.empty() ? 12 : 32;
        renderer.drawText(items_[i].label, textX, y + (itemHeight_ - 14) / 2, font, textColor);
    }

    renderer.restore();
}

bool Menu::handleEvent(const Event& event) {
    if (!open_) return false;

    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (!me.pressed) {
            // Check if clicking on an item
            float localX = me.x - posX_;
            float localY = me.y - posY_;
            if (localX >= 0 && localX <= bounds_.width && localY >= 0 && localY <= bounds_.height) {
                int idx = static_cast<int>(localY / itemHeight_);
                if (idx >= 0 && idx < (int)items_.size() && items_[idx].enabled && !items_[idx].separator) {
                    if (items_[idx].onClick) items_[idx].onClick();
                    hide();
                    return true;
                }
            }
            // Click outside closes menu
            hide();
            return true;
        }
    }

    if (event.type == Event::MouseMove) {
        auto& me = event.mouseMove();
        float localX = me.x - posX_;
        float localY = me.y - posY_;
        if (localX >= 0 && localX <= bounds_.width && localY >= 0 && localY <= bounds_.height) {
            int idx = static_cast<int>(localY / itemHeight_);
            if (idx >= 0 && idx < (int)items_.size() && idx != hoveredIndex_) {
                hoveredIndex_ = idx;
                markDirty();
            }
        } else if (hoveredIndex_ != -1) {
            hoveredIndex_ = -1;
            markDirty();
        }
    }

    if (event.type == Event::Key && event.key().key == KeyCode::Escape && event.key().pressed) {
        hide();
        return true;
    }

    return false;
}

// ============================================================
// Phase 8 — Card
// ============================================================

void Card::layout(const Constraints& constraints) {
    float pad = style_.padding.left + style_.padding.right;
    float childMaxW = constraints.maxWidth - pad;
    float childMaxH = constraints.maxHeight - style_.padding.top - style_.padding.bottom;

    Constraints childConstraints{0, childMaxW, 0, childMaxH};
    float contentH = 0;
    float contentW = 0;
    for (auto& child : children_) {
        child->layout(childConstraints);
        auto sz = child->getComputedSize();
        child->setPosition(style_.padding.left, style_.padding.top + contentH);
        contentH += sz.height;
        contentW = std::max(contentW, sz.width);
    }

    float w = style_.width >= 0 ? style_.width : contentW + pad;
    float h = style_.height >= 0 ? style_.height : contentH + style_.padding.top + style_.padding.bottom;
    bounds_.width = std::clamp(w, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(h, constraints.minHeight, constraints.maxHeight);
}

void Card::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    paintShadow(renderer);
    paintBackground(renderer);
    paintChildren(renderer);
    paintBorder(renderer);
    renderer.restore();
}

// ============================================================
// Phase 8 — Divider
// ============================================================

void Divider::layout(const Constraints& constraints) {
    if (vertical_) {
        bounds_.width = thickness_;
        bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;
    } else {
        bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
        bounds_.height = thickness_;
    }
}

void Divider::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, style_.backgroundColor);
    renderer.restore();
}

// ============================================================
// Phase 8 — Badge
// ============================================================

void Badge::layout(const Constraints& constraints) {
    if (dot_) {
        bounds_.width = 8;
        bounds_.height = 8;
    } else {
        float textW = label_.length() * 7.0f + 12;
        bounds_.width = std::max(20.0f, textW);
        bounds_.height = 20;
    }
}

void Badge::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float radius = bounds_.height / 2;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height}, BorderRadius::all(radius), style_.backgroundColor);

    if (!dot_ && !label_.empty()) {
        font_.size = 11;
        TextMetrics m = renderer.measureText(label_, font_);
        renderer.drawText(label_, (bounds_.width - m.width) / 2, (bounds_.height - m.height) / 2, font_, Color::white());
    }

    renderer.restore();
}

// ============================================================
// Phase 8 — Tooltip
// ============================================================

void Tooltip::layout(const Constraints& constraints) {
    // Layout child first
    for (auto& child : children_) {
        child->layout(constraints);
    }
    if (!children_.empty()) {
        auto sz = children_[0]->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    }
}

void Tooltip::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    paintChildren(renderer);

    if (showing_ && !message_.empty()) {
        font_.size = 12;
        TextMetrics m = renderer.measureText(message_, font_);
        float tipW = m.width + 16;
        float tipH = m.height + 12;
        float tipX = (bounds_.width - tipW) / 2;
        float tipY = -tipH - 4;

        renderer.fillRoundedRect({tipX, tipY, tipW, tipH}, BorderRadius::all(4), Color::rgb(50, 50, 50));
        renderer.drawText(message_, tipX + 8, tipY + 6, font_, Color::white());
    }

    renderer.restore();
}

bool Tooltip::handleEvent(const Event& event) {
    if (event.type == Event::MouseMove) {
        auto& me = event.mouseMove();
        bool inside = hitTest(me.x, me.y);
        if (inside && !showing_) {
            showing_ = true;
            markDirty();
        } else if (!inside && showing_) {
            showing_ = false;
            markDirty();
        }
    }
    return dispatchToChildren(event);
}

// ============================================================
// Phase 8 — Chip
// ============================================================

void Chip::layout(const Constraints& constraints) {
    float textW = label_.length() * 7.0f;
    bounds_.width = textW + 24;
    bounds_.height = 32;
}

void Chip::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    Color bg = selected_ ? selectedColor_ : normalColor_;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height}, BorderRadius::all(16), bg);

    Color textColor = selected_ ? Color::white() : Color::rgb(50, 50, 50);
    font_.size = 13;
    renderer.drawText(label_, 12, 8, font_, textColor);

    renderer.restore();
}

bool Chip::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (me.pressed && hitTest(me.x, me.y)) {
            if (onClick_) onClick_();
            return true;
        }
    }
    return false;
}

// ============================================================
// Phase 8 — FAB
// ============================================================

void FAB::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : 56;
    bounds_.height = style_.height >= 0 ? style_.height : 56;
}

void FAB::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    paintShadow(renderer);
    float radius = bounds_.width / 2;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height}, BorderRadius::all(radius), style_.backgroundColor);

    font_.size = 24;
    TextMetrics m = renderer.measureText(icon_, font_);
    renderer.drawText(icon_, (bounds_.width - m.width) / 2, (bounds_.height - m.height) / 2, font_, iconColor_);

    renderer.restore();
}

bool FAB::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (me.pressed && hitTest(me.x, me.y)) {
            if (onClick_) onClick_();
            return true;
        }
    }
    return false;
}

// ============================================================
// Phase 8 — ProgressBar
// ============================================================

void ProgressBar::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : 4;
}

void ProgressBar::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Track
    float r = bounds_.height / 2;
    renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height}, BorderRadius::all(r), trackColor_);

    // Active portion
    if (indeterminate_) {
        float barW = bounds_.width * 0.3f;
        float offset = fmod(animOffset_, bounds_.width + barW) - barW;
        renderer.fillRoundedRect({offset, 0, barW, bounds_.height}, BorderRadius::all(r), activeColor_);
    } else {
        float fillW = bounds_.width * std::clamp(value_, 0.0f, 1.0f);
        if (fillW > 0) {
            renderer.fillRoundedRect({0, 0, fillW, bounds_.height}, BorderRadius::all(r), activeColor_);
        }
    }

    renderer.restore();
}

// ============================================================
// Phase 8 — CircularProgress
// ============================================================

void CircularProgress::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : 40;
    bounds_.height = style_.height >= 0 ? style_.height : 40;
}

void CircularProgress::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float cx = bounds_.width / 2;
    float cy = bounds_.height / 2;
    float radius = std::min(cx, cy) - strokeWidth_;

    // Draw arc segments as line approximation
    int segments = 36;
    float startAngle = indeterminate_ ? animAngle_ : -M_PI / 2;
    float sweep = indeterminate_ ? M_PI * 1.5f : value_ * 2 * M_PI;

    for (int i = 0; i < segments; i++) {
        float a1 = startAngle + (sweep * i / segments);
        float a2 = startAngle + (sweep * (i + 1) / segments);
        float x1 = cx + radius * cosf(a1);
        float y1 = cy + radius * sinf(a1);
        float x2 = cx + radius * cosf(a2);
        float y2 = cy + radius * sinf(a2);
        renderer.drawLine(x1, y1, x2, y2, activeColor_, strokeWidth_);
    }

    renderer.restore();
}

// ============================================================
// Phase 8 — SnackBar
// ============================================================

void SnackBar::layout(const Constraints& constraints) {
    float textW = message_.length() * 8.0f;
    float actionW = actionLabel_.empty() ? 0 : actionLabel_.length() * 8.0f + 16;
    bounds_.width = style_.width >= 0 ? style_.width : std::min(textW + actionW + 32, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height : 48;
}

void SnackBar::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    paintBackground(renderer);

    font_.size = 14;
    renderer.drawText(message_, 16, 14, font_, Color::white());

    if (!actionLabel_.empty()) {
        TextMetrics m = renderer.measureText(actionLabel_, font_);
        float actionX = bounds_.width - m.width - 16;
        renderer.drawText(actionLabel_, actionX, 14, font_, Color::rgb(187, 134, 252));
    }

    renderer.restore();
}

// ============================================================
// Phase 8 — Wrap
// ============================================================

void Wrap::layout(const Constraints& constraints) {
    float maxW = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float x = 0, y = 0, rowHeight = 0;
    float totalW = 0;

    for (auto& child : children_) {
        child->layout(constraints.loosen());
        auto sz = child->getComputedSize();

        if (x + sz.width > maxW && x > 0) {
            // Wrap to next line
            x = 0;
            y += rowHeight + runSpacing_;
            rowHeight = 0;
        }

        child->setPosition(x, y);
        x += sz.width + spacing_;
        rowHeight = std::max(rowHeight, sz.height);
        totalW = std::max(totalW, x - spacing_);
    }

    bounds_.width = std::clamp(totalW, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(y + rowHeight, constraints.minHeight, constraints.maxHeight);
}

// ============================================================
// Phase 8 — ExpansionPanel
// ============================================================

void ExpansionPanel::layout(const Constraints& constraints) {
    float w = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float h = headerHeight_;

    if (expanded_) {
        Constraints childConstraints{0, w, 0, constraints.maxHeight - headerHeight_};
        float childY = headerHeight_;
        for (auto& child : children_) {
            child->layout(childConstraints);
            child->setPosition(0, childY);
            childY += child->getComputedSize().height;
        }
        h = childY;
    }

    bounds_.width = std::clamp(w, constraints.minWidth, constraints.maxWidth);
    bounds_.height = h;
}

void ExpansionPanel::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Header
    renderer.fillRect({0, 0, bounds_.width, headerHeight_}, headerColor_);

    font_.size = 14;
    renderer.drawText(title_, 16, 14, font_, Color::black());

    // Expand/collapse indicator
    std::string arrow = expanded_ ? "v" : ">";
    renderer.drawText(arrow, bounds_.width - 30, 14, font_, Color::black());

    // Children (only if expanded)
    if (expanded_) {
        paintChildren(renderer);
    }

    renderer.restore();
}

bool ExpansionPanel::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (me.pressed) {
            float localX = me.x - bounds_.x;
            float localY = me.y - bounds_.y;
            if (localX >= 0 && localX <= bounds_.width && localY >= 0 && localY <= headerHeight_) {
                expanded_ = !expanded_;
                markDirty();
                return true;
            }
        }
    }
    if (expanded_) return dispatchToChildren(event);
    return false;
}

// ============================================================
// Phase 8 — Scaffold
// ============================================================

void Scaffold::layout(const Constraints& constraints) {
    float w = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float h = style_.height >= 0 ? style_.height : constraints.maxHeight;
    bounds_.width = w;
    bounds_.height = h;

    float bodyTop = 0;
    if (appBar_) {
        Constraints barConstraints{w, w, 0, 64};
        appBar_->layout(barConstraints);
        appBar_->setPosition(0, 0);
        bodyTop = appBar_->getComputedSize().height;
    }

    if (body_) {
        Constraints bodyConstraints{0, w, 0, h - bodyTop};
        body_->layout(bodyConstraints);
        body_->setPosition(0, bodyTop);
    }

    if (fab_) {
        fab_->layout(constraints.loosen());
        auto fabSz = fab_->getComputedSize();
        fab_->setPosition(w - fabSz.width - 16, h - fabSz.height - 16);
    }
}

void Scaffold::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    paintBackground(renderer);
    if (body_) body_->paint(renderer);
    if (appBar_) appBar_->paint(renderer);
    if (fab_) fab_->paint(renderer);

    renderer.restore();
}

// ============================================================
// Phase 8 — BottomNavBar
// ============================================================

void BottomNavBar::layout(const Constraints& constraints) {
    bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
    bounds_.height = style_.height >= 0 ? style_.height : 56;
}

void BottomNavBar::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    // Background
    renderer.fillRect({0, 0, bounds_.width, bounds_.height}, style_.backgroundColor);

    // Top border line
    renderer.fillRect({0, 0, bounds_.width, 1}, Color::rgb(224, 224, 224));

    if (items_.empty()) {
        renderer.restore();
        return;
    }

    float itemW = bounds_.width / items_.size();
    for (size_t i = 0; i < items_.size(); i++) {
        float x = i * itemW;
        bool active = (int)i == activeIndex_;
        Color color = active ? activeColor_ : inactiveColor_;

        font_.size = 20;
        TextMetrics im = renderer.measureText(items_[i].icon, font_);
        renderer.drawText(items_[i].icon, x + (itemW - im.width) / 2, 6, font_, color);

        font_.size = 11;
        TextMetrics lm = renderer.measureText(items_[i].label, font_);
        renderer.drawText(items_[i].label, x + (itemW - lm.width) / 2, 34, font_, color);
    }

    renderer.restore();
}

bool BottomNavBar::handleEvent(const Event& event) {
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (me.pressed) {
            float localX = me.x - bounds_.x;
            float localY = me.y - bounds_.y;
            if (localX >= 0 && localX <= bounds_.width && localY >= 0 && localY <= bounds_.height && !items_.empty()) {
                float itemW = bounds_.width / items_.size();
                int idx = static_cast<int>(localX / itemW);
                if (idx >= 0 && idx < (int)items_.size() && idx != activeIndex_) {
                    activeIndex_ = idx;
                    if (onChange_) onChange_(idx);
                    markDirty();
                    return true;
                }
            }
        }
    }
    return false;
}

// ============================================================
// FormField Widget
// ============================================================

bool FormField::validate(const std::string& value) {
    for (auto& entry : validations_) {
        switch (entry.rule) {
            case ValidationRule::Required:
                if (value.empty()) {
                    error_ = entry.errorMessage;
                    markDirty();
                    return false;
                }
                break;
            case ValidationRule::MinLength:
                if ((int)value.length() < entry.intParam) {
                    error_ = entry.errorMessage;
                    markDirty();
                    return false;
                }
                break;
            case ValidationRule::MaxLength:
                if ((int)value.length() > entry.intParam) {
                    error_ = entry.errorMessage;
                    markDirty();
                    return false;
                }
                break;
            case ValidationRule::Email:
                if (value.find('@') == std::string::npos || value.find('.') == std::string::npos) {
                    error_ = entry.errorMessage;
                    markDirty();
                    return false;
                }
                break;
            case ValidationRule::Regex: {
                try {
                    std::regex re(entry.stringParam);
                    if (!std::regex_match(value, re)) {
                        error_ = entry.errorMessage;
                        markDirty();
                        return false;
                    }
                } catch (...) {
                    error_ = "Invalid regex pattern";
                    markDirty();
                    return false;
                }
                break;
            }
            case ValidationRule::Custom:
                if (entry.customValidator && !entry.customValidator(value)) {
                    error_ = entry.errorMessage;
                    markDirty();
                    return false;
                }
                break;
        }
    }
    error_.clear();
    markDirty();
    return true;
}

void FormField::layout(const Constraints& constraints) {
    float maxWidth = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float yOffset = 0;

    // Label height
    float labelH = 20.0f;
    yOffset += labelH + 4.0f;

    // Input widget
    if (input_) {
        Constraints inputC = {0, maxWidth, 0, constraints.maxHeight - yOffset};
        input_->layout(inputC);
        input_->setBounds({0, yOffset, input_->getComputedSize().width, input_->getComputedSize().height});
        yOffset += input_->getComputedSize().height + 4.0f;
    }

    // Error text height
    if (!error_.empty()) {
        yOffset += 16.0f;
    }

    bounds_.width = maxWidth;
    bounds_.height = yOffset;
}

void FormField::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    float yOffset = 0;

    // Draw label
    font_.size = 14;
    font_.weight = FontWeight::Medium;
    renderer.drawText(label_, 0, yOffset, font_, Color::rgb(100, 100, 100));
    yOffset += 20.0f + 4.0f;

    // Draw input
    if (input_) {
        input_->setBounds({0, yOffset, input_->getComputedSize().width, input_->getComputedSize().height});
        input_->paint(renderer);
        yOffset += input_->getComputedSize().height + 4.0f;
    }

    // Draw error
    if (!error_.empty()) {
        font_.size = 12;
        font_.weight = FontWeight::Regular;
        renderer.drawText(error_, 0, yOffset, font_, Color::rgb(211, 47, 47));
    }

    renderer.restore();
}

// ============================================================
// Form Widget
// ============================================================

void Form::layout(const Constraints& constraints) {
    float maxWidth = style_.width >= 0 ? style_.width : constraints.maxWidth;
    float yOffset = 0;
    float spacing = 12.0f;

    for (size_t i = 0; i < children_.size(); i++) {
        Constraints childC = {0, maxWidth, 0, constraints.maxHeight - yOffset};
        children_[i]->layout(childC);
        children_[i]->setBounds({0, yOffset, children_[i]->getComputedSize().width, children_[i]->getComputedSize().height});
        yOffset += children_[i]->getComputedSize().height;
        if (i + 1 < children_.size()) yOffset += spacing;
    }

    bounds_.width = maxWidth;
    bounds_.height = yOffset;
}

void Form::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    for (auto& child : children_) {
        child->paint(renderer);
    }

    renderer.restore();
}

bool Form::validate() {
    valid_ = true;
    for (auto& [name, field] : fields_) {
        std::string value;
        auto input = field->getInput();
        if (auto tf = std::dynamic_pointer_cast<TextField>(input)) {
            value = tf->getValue();
        }
        if (!field->validate(value)) {
            valid_ = false;
        }
    }
    return valid_;
}

void Form::reset() {
    for (auto& [name, field] : fields_) {
        field->clearError();
    }
    valid_ = true;
}

// ============================================================
// Phase 8.2 — GestureDetector
// ============================================================

void GestureDetector::layout(const Constraints& constraints) {
    // Size to child
    if (!children_.empty()) {
        children_[0]->layout(constraints);
        auto sz = children_[0]->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    } else {
        bounds_.width = style_.width >= 0 ? style_.width : constraints.maxWidth;
        bounds_.height = style_.height >= 0 ? style_.height : constraints.maxHeight;
    }
}

void GestureDetector::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    paintChildren(renderer);
    renderer.restore();
}

bool GestureDetector::handleEvent(const Event& event) {
    if (!visible_ || !isEnabled()) return false;

    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (!hitTest(me.x, me.y)) return dispatchToChildren(event);

        uint32_t now = SDL_GetTicks();

        if (me.pressed) {
            pressed_ = true;
            pressStartX_ = me.x;
            pressStartY_ = me.y;
            pressStartTime_ = now;
            lastDragX_ = me.x;
            lastDragY_ = me.y;
        } else {
            // Release
            if (dragging_) {
                dragging_ = false;

                // Check swipe
                float dx = me.x - dragStartX_;
                float dy = me.y - dragStartY_;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist >= SWIPE_MIN_DISTANCE && onSwipe_) {
                    if (std::abs(dx) > std::abs(dy)) {
                        onSwipe_(dx > 0 ? SwipeDirection::Right : SwipeDirection::Left);
                    } else {
                        onSwipe_(dy > 0 ? SwipeDirection::Down : SwipeDirection::Up);
                    }
                }

                if (onDragEnd_) onDragEnd_();
            } else if (pressed_) {
                // Check double-tap
                if (waitingForDoubleClick_ && (now - lastClickTime_) < DOUBLE_TAP_MS) {
                    if (onDoubleTap_) onDoubleTap_();
                    waitingForDoubleClick_ = false;
                } else {
                    waitingForDoubleClick_ = true;
                    lastClickTime_ = now;
                }
            }
            pressed_ = false;
        }
        return true;
    }

    if (event.type == Event::MouseMove && pressed_) {
        auto& me = event.mouseMove();
        float dx = me.x - pressStartX_;
        float dy = me.y - pressStartY_;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (!dragging_ && dist >= DRAG_THRESHOLD) {
            dragging_ = true;
            dragStartX_ = pressStartX_;
            dragStartY_ = pressStartY_;
            if (onDragStart_) onDragStart_(pressStartX_, pressStartY_);
        }

        if (dragging_ && onDrag_) {
            float ddx = me.x - lastDragX_;
            float ddy = me.y - lastDragY_;
            onDrag_(me.x, me.y, ddx, ddy);
            lastDragX_ = me.x;
            lastDragY_ = me.y;
        }
        return true;
    }

    // Long press detection is checked frame-by-frame.
    // For simplicity, check on mouse button events:
    // The App event loop would need to pump timer events for true long press.
    // We approximate by checking elapsed time on the next mouse event.

    return dispatchToChildren(event);
}

// ============================================================
// Phase 8.4 — ErrorBoundary
// ============================================================

void ErrorBoundary::setError(const std::string& message) {
    hasError_ = true;
    errorMessage_ = message;
    if (onError_) onError_(message);
    markDirty();
}

void ErrorBoundary::reset() {
    hasError_ = false;
    errorMessage_.clear();
    markDirty();
}

void ErrorBoundary::layout(const Constraints& constraints) {
    if (hasError_) {
        if (fallback_) {
            fallback_->layout(constraints);
            auto sz = fallback_->getComputedSize();
            bounds_.width = sz.width;
            bounds_.height = sz.height;
        } else {
            // Default error display
            bounds_.width = std::min(constraints.maxWidth, 400.0f);
            bounds_.height = 80.0f;
        }
    } else {
        // Layout normal children
        float h = 0, w = 0;
        for (auto& child : children_) {
            child->layout(constraints);
            auto sz = child->getComputedSize();
            child->setPosition(0, h);
            h += sz.height;
            w = std::max(w, sz.width);
        }
        bounds_.width = std::clamp(w, constraints.minWidth, constraints.maxWidth);
        bounds_.height = std::clamp(h, constraints.minHeight, constraints.maxHeight);
    }
}

void ErrorBoundary::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    if (hasError_) {
        if (fallback_) {
            fallback_->paint(renderer);
        } else {
            // Default error UI
            renderer.fillRoundedRect({0, 0, bounds_.width, bounds_.height},
                                      BorderRadius::all(8), Color::rgb(255, 235, 238));
            renderer.drawRoundedRect({0, 0, bounds_.width, bounds_.height},
                                      BorderRadius::all(8), Color::rgb(211, 47, 47));
            font_.size = 14;
            font_.weight = FontWeight::Bold;
            renderer.drawText("Error", 16, 12, font_, Color::rgb(211, 47, 47));
            font_.size = 12;
            font_.weight = FontWeight::Regular;
            renderer.drawText(errorMessage_, 16, 36, font_, Color::rgb(100, 50, 50));
        }
    } else {
        paintChildren(renderer);
    }

    renderer.restore();
}

bool ErrorBoundary::handleEvent(const Event& event) {
    if (hasError_) {
        if (fallback_) return fallback_->handleEvent(event);
        return false;
    }
    return dispatchToChildren(event);
}

// ============================================================
// Phase 8.5 — Responsive
// ============================================================

void Responsive::rebuildIfNeeded() {
    auto& mq = MediaQueryRegistry::instance();
    Breakpoint current = mq.get().breakpoint;

    if (!needsRebuild_ && current == lastBreakpoint_) return;

    lastBreakpoint_ = current;
    needsRebuild_ = false;

    // Clear existing child
    currentChild_.reset();
    children_.clear();

    switch (current) {
        case Breakpoint::Mobile:
            if (mobileBuilder_) currentChild_ = mobileBuilder_();
            else if (tabletBuilder_) currentChild_ = tabletBuilder_();
            else if (desktopBuilder_) currentChild_ = desktopBuilder_();
            break;
        case Breakpoint::Tablet:
            if (tabletBuilder_) currentChild_ = tabletBuilder_();
            else if (desktopBuilder_) currentChild_ = desktopBuilder_();
            else if (mobileBuilder_) currentChild_ = mobileBuilder_();
            break;
        case Breakpoint::Desktop:
            if (desktopBuilder_) currentChild_ = desktopBuilder_();
            else if (tabletBuilder_) currentChild_ = tabletBuilder_();
            else if (mobileBuilder_) currentChild_ = mobileBuilder_();
            break;
    }

    if (currentChild_) {
        addChild(currentChild_);
    }
}

void Responsive::layout(const Constraints& constraints) {
    rebuildIfNeeded();

    if (currentChild_) {
        currentChild_->layout(constraints);
        auto sz = currentChild_->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    } else {
        bounds_.width = 0;
        bounds_.height = 0;
    }
}

void Responsive::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    if (currentChild_) currentChild_->paint(renderer);
    renderer.restore();
}

bool Responsive::handleEvent(const Event& event) {
    if (currentChild_) return currentChild_->handleEvent(event);
    return false;
}

// ============================================================
// Phase 9.2 — FutureBuilder
// ============================================================

void FutureBuilder::layout(const Constraints& constraints) {
    WidgetPtr active;
    switch (state_) {
        case AsyncState::Loading: active = loadingWidget_; break;
        case AsyncState::Success: active = successWidget_; break;
        case AsyncState::Error: active = errorWidget_; break;
    }

    if (active) {
        active->layout(constraints);
        auto sz = active->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    } else {
        bounds_.width = 0;
        bounds_.height = 0;
    }
}

void FutureBuilder::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    WidgetPtr active;
    switch (state_) {
        case AsyncState::Loading: active = loadingWidget_; break;
        case AsyncState::Success: active = successWidget_; break;
        case AsyncState::Error: active = errorWidget_; break;
    }

    if (active) active->paint(renderer);

    renderer.restore();
}

bool FutureBuilder::handleEvent(const Event& event) {
    WidgetPtr active;
    switch (state_) {
        case AsyncState::Loading: active = loadingWidget_; break;
        case AsyncState::Success: active = successWidget_; break;
        case AsyncState::Error: active = errorWidget_; break;
    }
    if (active) return active->handleEvent(event);
    return false;
}

// ============================================================
// Phase 9.2 — Suspense
// ============================================================

void Suspense::layout(const Constraints& constraints) {
    if (ready_) {
        // Layout children
        float h = 0, w = 0;
        for (auto& child : children_) {
            child->layout(constraints);
            auto sz = child->getComputedSize();
            child->setPosition(0, h);
            h += sz.height;
            w = std::max(w, sz.width);
        }
        bounds_.width = std::clamp(w, constraints.minWidth, constraints.maxWidth);
        bounds_.height = std::clamp(h, constraints.minHeight, constraints.maxHeight);
    } else if (fallback_) {
        fallback_->layout(constraints);
        auto sz = fallback_->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    } else {
        bounds_.width = 0;
        bounds_.height = 0;
    }
}

void Suspense::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);

    if (ready_) {
        paintChildren(renderer);
    } else if (fallback_) {
        fallback_->paint(renderer);
    }

    renderer.restore();
}

bool Suspense::handleEvent(const Event& event) {
    if (ready_) return dispatchToChildren(event);
    if (fallback_) return fallback_->handleEvent(event);
    return false;
}

// ============================================================
// Phase 9.3 — AnimatedContainer
// ============================================================

void AnimatedContainer::animateProperty(float current, float target, std::shared_ptr<Tween>& tween) {
    tween = AnimationManager::instance().createTween(current, target, duration_, easing_);
    tween->start();
    markDirty();
}

void AnimatedContainer::update(float deltaMs) {
    bool changed = false;
    if (widthTween_ && widthTween_->isRunning()) {
        widthTween_->update(deltaMs);
        currentWidth_ = widthTween_->getCurrentValue();
        changed = true;
    }
    if (heightTween_ && heightTween_->isRunning()) {
        heightTween_->update(deltaMs);
        currentHeight_ = heightTween_->getCurrentValue();
        changed = true;
    }
    if (animateColors_) {
        colorProgress_ += deltaMs / duration_;
        if (colorProgress_ >= 1.0f) {
            colorProgress_ = 1.0f;
            animateColors_ = false;
            style_.backgroundColor = targetColor_;
        } else {
            float t = applyEasing(colorProgress_, easing_);
            style_.backgroundColor.r = startColor_.r + (targetColor_.r - startColor_.r) * t;
            style_.backgroundColor.g = startColor_.g + (targetColor_.g - startColor_.g) * t;
            style_.backgroundColor.b = startColor_.b + (targetColor_.b - startColor_.b) * t;
            style_.backgroundColor.a = startColor_.a + (targetColor_.a - startColor_.a) * t;
        }
        changed = true;
    }
    if (changed) markDirty();
}

void AnimatedContainer::layout(const Constraints& constraints) {
    float w = currentWidth_ >= 0 ? currentWidth_ : (style_.width >= 0 ? style_.width : constraints.maxWidth);
    float h = currentHeight_ >= 0 ? currentHeight_ : (style_.height >= 0 ? style_.height : 0);

    Constraints childC = {0, w, 0, h > 0 ? h : constraints.maxHeight};
    for (auto& child : children_) {
        child->layout(childC);
        auto sz = child->getComputedSize();
        if (h <= 0) h = sz.height;
    }

    bounds_.width = std::clamp(w, constraints.minWidth, constraints.maxWidth);
    bounds_.height = std::clamp(h, constraints.minHeight, constraints.maxHeight);
}

void AnimatedContainer::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    paintBackground(renderer);
    paintBorder(renderer);
    paintChildren(renderer);
    renderer.restore();
}

// ============================================================
// Phase 9.3 — FadeTransition
// ============================================================

void FadeTransition::startFade(float from, float to) {
    opacityTween_ = AnimationManager::instance().createTween(from, to, duration_, easing_);
    opacityTween_->start();
    currentOpacity_ = from;
    markDirty();
}

void FadeTransition::update(float deltaMs) {
    if (opacityTween_ && opacityTween_->isRunning()) {
        opacityTween_->update(deltaMs);
        currentOpacity_ = opacityTween_->getCurrentValue();
        markDirty();
    }
}

void FadeTransition::layout(const Constraints& constraints) {
    for (auto& child : children_) {
        child->layout(constraints);
    }
    if (!children_.empty()) {
        auto sz = children_[0]->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    }
}

void FadeTransition::paint(IRenderer& renderer) {
    if (!visible_ || currentOpacity_ <= 0) return;
    renderer.save();
    renderer.translate(bounds_.x, bounds_.y);
    renderer.setOpacity(currentOpacity_);
    paintChildren(renderer);
    renderer.restore();
}

// ============================================================
// Phase 9.3 — SlideTransition
// ============================================================

void SlideTransition::startSlide(bool entering) {
    entering_ = entering;

    float from = entering ? 1.0f : 0.0f;
    float to = entering ? 0.0f : 1.0f;
    slideTween_ = AnimationManager::instance().createTween(from, to, duration_, easing_);
    slideTween_->start();
    markDirty();
}

void SlideTransition::update(float deltaMs) {
    if (slideTween_ && slideTween_->isRunning()) {
        slideTween_->update(deltaMs);
        float t = slideTween_->getCurrentValue();
        switch (direction_) {
            case Direction::Left: offsetX_ = -t * (bounds_.width > 0 ? bounds_.width : 400); offsetY_ = 0; break;
            case Direction::Right: offsetX_ = t * (bounds_.width > 0 ? bounds_.width : 400); offsetY_ = 0; break;
            case Direction::Up: offsetX_ = 0; offsetY_ = -t * (bounds_.height > 0 ? bounds_.height : 400); break;
            case Direction::Down: offsetX_ = 0; offsetY_ = t * (bounds_.height > 0 ? bounds_.height : 400); break;
        }
        markDirty();
    }
}

void SlideTransition::layout(const Constraints& constraints) {
    for (auto& child : children_) {
        child->layout(constraints);
    }
    if (!children_.empty()) {
        auto sz = children_[0]->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    }
}

void SlideTransition::paint(IRenderer& renderer) {
    if (!visible_) return;
    renderer.save();
    renderer.translate(bounds_.x + offsetX_, bounds_.y + offsetY_);
    paintChildren(renderer);
    renderer.restore();
}

// ============================================================
// Phase 9.3 — ScaleTransition
// ============================================================

void ScaleTransition::startScale(float from, float to) {
    scaleTween_ = AnimationManager::instance().createTween(from, to, duration_, easing_);
    scaleTween_->start();
    currentScale_ = from;
    markDirty();
}

void ScaleTransition::update(float deltaMs) {
    if (scaleTween_ && scaleTween_->isRunning()) {
        scaleTween_->update(deltaMs);
        currentScale_ = scaleTween_->getCurrentValue();
        markDirty();
    }
}

void ScaleTransition::layout(const Constraints& constraints) {
    for (auto& child : children_) {
        child->layout(constraints);
    }
    if (!children_.empty()) {
        auto sz = children_[0]->getComputedSize();
        bounds_.width = sz.width;
        bounds_.height = sz.height;
    }
}

void ScaleTransition::paint(IRenderer& renderer) {
    if (!visible_ || currentScale_ <= 0) return;
    renderer.save();
    float cx = bounds_.x + bounds_.width / 2;
    float cy = bounds_.y + bounds_.height / 2;
    renderer.translate(cx, cy);
    renderer.scale(currentScale_, currentScale_);
    renderer.translate(-bounds_.width / 2, -bounds_.height / 2);
    paintChildren(renderer);
    renderer.restore();
}

} // namespace gui
} // namespace stratos
