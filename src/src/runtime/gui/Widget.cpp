#include "stratos/gui/Widget.h"
#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace stratos {
namespace gui {

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

    // Check click
    if (event.type == Event::MouseButton) {
        auto& me = event.mouseButton();
        if (hitTest(me.x, me.y)) {
            if (me.pressed) {
                state_ = WidgetState::Pressed;
            } else {
                if (state_ == WidgetState::Pressed && onClick_) {
                    onClick_();
                }
                state_ = hovered_ ? WidgetState::Hovered : WidgetState::Normal;
            }
            return true;
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
    // Dispatch in reverse order (top-most first)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if ((*it)->handleEvent(event)) return true;
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

    renderer.drawText(text_, textX, 0, font_, color_);

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

    // Shadow
    if (state_ != WidgetState::Disabled) {
        Shadow shadow = {0, 2, 4, Color::rgba(0, 0, 0, 40)};
        renderer.drawShadow({0, 0, bounds_.width, bounds_.height},
                             BorderRadius::all(4), shadow);
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
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(200.0f, constraints.minWidth, constraints.maxWidth);
    bounds_.height = style_.height >= 0 ? style_.height :
                     std::clamp(40.0f, constraints.minHeight, constraints.maxHeight);
}

void TextField::paint(IRenderer& renderer) {
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

    // Text or placeholder
    float textX = 8;
    float textY = (bounds_.height - font_.size * 1.4f) / 2;

    if (value_.empty() && !placeholder_.empty()) {
        renderer.drawText(placeholder_, textX, textY, font_, Color::rgb(158, 158, 158));
    } else {
        renderer.drawText(value_, textX, textY, font_, Color::black());

        // Cursor
        if (state_ == WidgetState::Focused && cursorVisible_) {
            std::string beforeCursor = value_.substr(0, cursorPos_);
            TextMetrics m = renderer.measureText(beforeCursor, font_);
            renderer.drawLine(textX + m.width, textY + 2,
                               textX + m.width, textY + font_.size + 2,
                               Color::black(), 1.5f);
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

    // Box
    if (checked_) {
        renderer.fillRoundedRect({0, boxY, boxSize, boxSize},
                                  BorderRadius::all(3), checkColor_);
        // Checkmark
        renderer.drawLine(4, boxY + boxSize / 2, 7, boxY + boxSize - 5, Color::white(), 2);
        renderer.drawLine(7, boxY + boxSize - 5, 14, boxY + 4, Color::white(), 2);
    } else {
        renderer.drawRoundedRect({0, boxY, boxSize, boxSize},
                                  BorderRadius::all(3), Color::rgb(158, 158, 158));
    }

    // Label
    if (!label_.empty()) {
        FontSpec font = {"sans-serif", 14, FontWeight::Regular, FontStyle::Normal};
        renderer.drawText(label_, boxSize + 8, (bounds_.height - 14) / 2, font, Color::black());
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
    bounds_.width = style_.width >= 0 ? style_.width :
                    std::clamp(200.0f, constraints.minWidth, constraints.maxWidth);
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

    // Track background
    renderer.fillRoundedRect({0, trackY, bounds_.width, trackH},
                              BorderRadius::all(2), trackColor_);

    // Active track
    renderer.fillRoundedRect({0, trackY, thumbX, trackH},
                              BorderRadius::all(2), activeColor_);

    // Thumb shadow
    renderer.drawShadow({thumbX - thumbR, bounds_.height / 2 - thumbR, thumbR * 2, thumbR * 2},
                          BorderRadius::all(thumbR), {0, 1, 3, Color::rgba(0, 0, 0, 40)});

    // Thumb
    renderer.fillCircle(thumbX, bounds_.height / 2, thumbR, thumbColor_);
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

} // namespace gui
} // namespace stratos
