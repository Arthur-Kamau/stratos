#pragma once

#include "stratos/gui/Renderer.h"
#include "stratos/gui/Event.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <any>

namespace stratos {
namespace gui {

// Forward declarations
class Widget;
class Layout;

using WidgetPtr = std::shared_ptr<Widget>;
using WidgetList = std::vector<WidgetPtr>;

// ============================================================
// Widget State
// ============================================================

enum class WidgetState {
    Normal,
    Hovered,
    Pressed,
    Focused,
    Disabled
};

// ============================================================
// Style Properties
// ============================================================

struct WidgetStyle {
    // Dimensions
    float width = -1;          // -1 = auto
    float height = -1;         // -1 = auto
    float minWidth = 0;
    float minHeight = 0;
    float maxWidth = -1;       // -1 = none
    float maxHeight = -1;      // -1 = none

    // Spacing
    EdgeInsets padding = EdgeInsets::all(0);
    EdgeInsets margin = EdgeInsets::all(0);

    // Appearance
    Color backgroundColor = Color::transparent();
    Color borderColor = Color::transparent();
    float borderWidth = 0;
    BorderRadius borderRadius = BorderRadius::all(0);
    float opacity = 1.0f;

    // Shadow
    bool hasShadow = false;
    Shadow shadow;

    // Flex
    float flex = 0;            // 0 = no flex
    float flexGrow = 0;
    float flexShrink = 1;
};

// ============================================================
// Constraints (for layout)
// ============================================================

struct Constraints {
    float minWidth = 0;
    float maxWidth = 1e6f;
    float minHeight = 0;
    float maxHeight = 1e6f;

    Constraints tighten(float w, float h) const {
        return {w, w, h, h};
    }

    Constraints loosen() const {
        return {0, maxWidth, 0, maxHeight};
    }
};

// ============================================================
// Base Widget
// ============================================================

class Widget : public std::enable_shared_from_this<Widget> {
public:
    Widget() = default;
    virtual ~Widget() = default;

    // Core lifecycle
    virtual void layout(const Constraints& constraints);
    virtual void paint(IRenderer& renderer);
    virtual bool handleEvent(const Event& event);

    // Tree structure
    void addChild(WidgetPtr child);
    void removeChild(WidgetPtr child);
    void clearChildren();
    const WidgetList& getChildren() const { return children_; }
    Widget* getParent() const { return parent_; }

    // Geometry
    Rect getBounds() const { return bounds_; }
    void setBounds(const Rect& bounds) { bounds_ = bounds; }
    Size getComputedSize() const { return {bounds_.width, bounds_.height}; }

    // Position (relative to parent)
    float getX() const { return bounds_.x; }
    float getY() const { return bounds_.y; }
    void setPosition(float x, float y) { bounds_.x = x; bounds_.y = y; }

    // Style
    WidgetStyle& style() { return style_; }
    const WidgetStyle& style() const { return style_; }

    // State
    WidgetState getState() const { return state_; }
    void setState(WidgetState state) { state_ = state; }
    bool isVisible() const { return visible_; }
    void setVisible(bool v) { visible_ = v; }
    bool isEnabled() const { return state_ != WidgetState::Disabled; }
    void setEnabled(bool e) { state_ = e ? WidgetState::Normal : WidgetState::Disabled; }

    // Identity
    const std::string& getId() const { return id_; }
    void setId(const std::string& id) { id_ = id; }

    // Event handlers
    void setOnClick(OnClickHandler handler) { onClick_ = std::move(handler); }
    void setOnMouseEnter(OnMouseHandler handler) { onMouseEnter_ = std::move(handler); }
    void setOnMouseLeave(OnMouseHandler handler) { onMouseLeave_ = std::move(handler); }
    void setOnKeyPress(OnKeyHandler handler) { onKeyPress_ = std::move(handler); }

    // Hit testing
    virtual bool hitTest(float x, float y) const;

    // Dirty flag for re-rendering
    void markDirty();
    bool isDirty() const { return dirty_; }
    void clearDirty() { dirty_ = false; }

    // Widget type name (for debugging/serialization)
    virtual std::string typeName() const { return "Widget"; }

protected:
    // Paint helpers
    void paintBackground(IRenderer& renderer);
    void paintBorder(IRenderer& renderer);
    void paintShadow(IRenderer& renderer);
    void paintChildren(IRenderer& renderer);

    // Event dispatch
    bool dispatchToChildren(const Event& event);

    WidgetList children_;
    Widget* parent_ = nullptr;
    Rect bounds_ = {0, 0, 0, 0};
    WidgetStyle style_;
    WidgetState state_ = WidgetState::Normal;
    bool visible_ = true;
    bool dirty_ = true;
    std::string id_;

    // Event handlers
    OnClickHandler onClick_;
    OnMouseHandler onMouseEnter_;
    OnMouseHandler onMouseLeave_;
    OnKeyHandler onKeyPress_;
    bool hovered_ = false;
};

// ============================================================
// Layout Widgets
// ============================================================

enum class MainAxisAlignment {
    Start,
    Center,
    End,
    SpaceBetween,
    SpaceAround,
    SpaceEvenly
};

enum class CrossAxisAlignment {
    Start,
    Center,
    End,
    Stretch
};

class Row : public Widget {
public:
    Row() = default;
    explicit Row(float spacing) : spacing_(spacing) {}

    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Row"; }

    void setSpacing(float s) { spacing_ = s; }
    void setMainAxisAlignment(MainAxisAlignment a) { mainAlign_ = a; }
    void setCrossAxisAlignment(CrossAxisAlignment a) { crossAlign_ = a; }

private:
    float spacing_ = 0;
    MainAxisAlignment mainAlign_ = MainAxisAlignment::Start;
    CrossAxisAlignment crossAlign_ = CrossAxisAlignment::Start;
};

class Column : public Widget {
public:
    Column() = default;
    explicit Column(float spacing) : spacing_(spacing) {}

    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Column"; }

    void setSpacing(float s) { spacing_ = s; }
    void setMainAxisAlignment(MainAxisAlignment a) { mainAlign_ = a; }
    void setCrossAxisAlignment(CrossAxisAlignment a) { crossAlign_ = a; }

private:
    float spacing_ = 0;
    MainAxisAlignment mainAlign_ = MainAxisAlignment::Start;
    CrossAxisAlignment crossAlign_ = CrossAxisAlignment::Start;
};

class Stack : public Widget {
public:
    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Stack"; }
};

class Padding : public Widget {
public:
    explicit Padding(EdgeInsets insets) : insets_(insets) {}

    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Padding"; }

private:
    EdgeInsets insets_;
};

class Center : public Widget {
public:
    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Center"; }
};

class Spacer : public Widget {
public:
    explicit Spacer(float size = 0) {
        style_.flex = size > 0 ? 0 : 1;
        if (size > 0) {
            style_.width = size;
            style_.height = size;
        }
    }
    std::string typeName() const override { return "Spacer"; }
};

class ScrollView : public Widget {
public:
    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "ScrollView"; }

    float getScrollX() const { return scrollX_; }
    float getScrollY() const { return scrollY_; }

private:
    float scrollX_ = 0, scrollY_ = 0;
    float contentWidth_ = 0, contentHeight_ = 0;
    bool vertical_ = true;
    bool horizontal_ = false;
};

// ============================================================
// Content Widgets
// ============================================================

class Text : public Widget {
public:
    Text() = default;
    explicit Text(const std::string& text) : text_(text) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    std::string typeName() const override { return "Text"; }

    void setText(const std::string& text) { text_ = text; markDirty(); }
    const std::string& getText() const { return text_; }
    void setFont(const FontSpec& font) { font_ = font; markDirty(); }
    const FontSpec& getFont() const { return font_; }
    void setColor(const Color& color) { color_ = color; }
    void setTextAlign(TextAlign align) { align_ = align; }

private:
    std::string text_;
    FontSpec font_;
    Color color_ = Color::black();
    TextAlign align_ = TextAlign::Left;
};

class Button : public Widget {
public:
    Button() = default;
    explicit Button(const std::string& label) : label_(label) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Button"; }

    void setLabel(const std::string& label) { label_ = label; markDirty(); }
    const std::string& getLabel() const { return label_; }
    void setFont(const FontSpec& font) { font_ = font; }
    void setTextColor(const Color& color) { textColor_ = color; }

private:
    std::string label_;
    FontSpec font_;
    Color textColor_ = Color::white();
    Color normalColor_ = Color::rgb(33, 150, 243);    // Material blue
    Color hoverColor_ = Color::rgb(25, 118, 210);
    Color pressedColor_ = Color::rgb(21, 101, 192);
    Color disabledColor_ = Color::gray();
};

class Image : public Widget {
public:
    Image() = default;
    explicit Image(const std::string& path) : path_(path) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    std::string typeName() const override { return "Image"; }

    void setSource(const std::string& path) { path_ = path; imageId_ = -1; markDirty(); }

private:
    std::string path_;
    int imageId_ = -1;
};

class Box : public Widget {
public:
    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    std::string typeName() const override { return "Box"; }
};

// ============================================================
// Input Widgets
// ============================================================

class TextField : public Widget {
public:
    TextField() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "TextField"; }

    void setValue(const std::string& value) { value_ = value; markDirty(); }
    const std::string& getValue() const { return value_; }
    void setPlaceholder(const std::string& placeholder) { placeholder_ = placeholder; }
    void setOnChange(OnChangeHandler handler) { onChange_ = std::move(handler); }
    void setOnSubmit(OnClickHandler handler) { onSubmit_ = std::move(handler); }

private:
    std::string value_;
    std::string placeholder_;
    FontSpec font_;
    int cursorPos_ = 0;
    int selectionStart_ = -1;
    bool cursorVisible_ = true;
    float cursorBlinkTimer_ = 0;
    OnChangeHandler onChange_;
    OnClickHandler onSubmit_;
};

class Checkbox : public Widget {
public:
    Checkbox() = default;
    explicit Checkbox(const std::string& label) : label_(label) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Checkbox"; }

    void setChecked(bool checked) { checked_ = checked; markDirty(); }
    bool isChecked() const { return checked_; }
    void setLabel(const std::string& label) { label_ = label; }
    void setOnChange(OnBoolChangeHandler handler) { onChange_ = std::move(handler); }

private:
    std::string label_;
    bool checked_ = false;
    Color checkColor_ = Color::rgb(33, 150, 243);
    OnBoolChangeHandler onChange_;
};

class Slider : public Widget {
public:
    Slider() = default;
    Slider(float min, float max) : min_(min), max_(max) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Slider"; }

    void setValue(float value) { value_ = value; markDirty(); }
    float getValue() const { return value_; }
    void setRange(float min, float max) { min_ = min; max_ = max; }
    void setOnChange(OnFloatChangeHandler handler) { onChange_ = std::move(handler); }

private:
    float value_ = 0;
    float min_ = 0, max_ = 100;
    bool dragging_ = false;
    Color trackColor_ = Color::rgb(200, 200, 200);
    Color activeColor_ = Color::rgb(33, 150, 243);
    Color thumbColor_ = Color::white();
    OnFloatChangeHandler onChange_;
};

class Switch : public Widget {
public:
    Switch() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Switch"; }

    void setOn(bool on) { on_ = on; markDirty(); }
    bool isOn() const { return on_; }
    void setOnChange(OnBoolChangeHandler handler) { onChange_ = std::move(handler); }

private:
    bool on_ = false;
    Color onColor_ = Color::rgb(33, 150, 243);
    Color offColor_ = Color::rgb(200, 200, 200);
    OnBoolChangeHandler onChange_;
};

// ============================================================
// Navigation Widgets
// ============================================================

class AppBar : public Widget {
public:
    AppBar() = default;
    explicit AppBar(const std::string& title) : title_(title) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    std::string typeName() const override { return "AppBar"; }

    void setTitle(const std::string& title) { title_ = title; markDirty(); }
    void setColor(const Color& color) { barColor_ = color; }

private:
    std::string title_;
    Color barColor_ = Color::rgb(33, 150, 243);
    Color titleColor_ = Color::white();
    float height_ = 56.0f;
};

class Dialog : public Widget {
public:
    Dialog() = default;
    explicit Dialog(const std::string& title) : title_(title) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Dialog"; }

    void setTitle(const std::string& title) { title_ = title; }
    void setOpen(bool open) { open_ = open; markDirty(); }
    bool isOpen() const { return open_; }
    void setOnClose(OnClickHandler handler) { onClose_ = std::move(handler); }

private:
    std::string title_;
    bool open_ = false;
    OnClickHandler onClose_;
};

class ListView : public Widget {
public:
    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "ListView"; }

    void setItemCount(int count) { itemCount_ = count; }
    void setItemHeight(float height) { itemHeight_ = height; }
    using ItemBuilder = std::function<WidgetPtr(int index)>;
    void setItemBuilder(ItemBuilder builder) { itemBuilder_ = std::move(builder); }

private:
    int itemCount_ = 0;
    float itemHeight_ = 48.0f;
    float scrollOffset_ = 0;
    ItemBuilder itemBuilder_;
};

// ============================================================
// Grid Layout
// ============================================================

class Grid : public Widget {
public:
    Grid() = default;
    Grid(int columns, float spacing) : columns_(columns), spacing_(spacing) {}

    void layout(const Constraints& constraints) override;
    std::string typeName() const override { return "Grid"; }

    void setColumns(int cols) { columns_ = cols; }
    void setSpacing(float s) { spacing_ = s; }

private:
    int columns_ = 2;
    float spacing_ = 8;
};

// ============================================================
// TextArea Widget (multi-line text input)
// ============================================================

class TextArea : public Widget {
public:
    TextArea() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "TextArea"; }

    void setValue(const std::string& value) { value_ = value; markDirty(); }
    const std::string& getValue() const { return value_; }
    void setPlaceholder(const std::string& placeholder) { placeholder_ = placeholder; }
    void setOnChange(OnChangeHandler handler) { onChange_ = std::move(handler); }

private:
    std::string value_;
    std::string placeholder_;
    FontSpec font_;
    int cursorPos_ = 0;
    int cursorLine_ = 0;
    int cursorCol_ = 0;
    float scrollY_ = 0;
    bool cursorVisible_ = true;
    OnChangeHandler onChange_;

    // Helper to split text into lines
    std::vector<std::string> getLines() const;
    void updateCursorLineCol();
};

// ============================================================
// RadioButton Widget
// ============================================================

class RadioButton : public Widget {
public:
    RadioButton() = default;
    RadioButton(const std::string& label, const std::string& group)
        : label_(label), group_(group) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "RadioButton"; }

    void setSelected(bool selected) { selected_ = selected; markDirty(); }
    bool isSelected() const { return selected_; }
    void setLabel(const std::string& label) { label_ = label; }
    void setGroup(const std::string& group) { group_ = group; }
    const std::string& getGroup() const { return group_; }
    void setOnChange(OnBoolChangeHandler handler) { onChange_ = std::move(handler); }

    // Static: deselect all other radio buttons in the same group
    static void deselectGroup(Widget* root, const std::string& group, RadioButton* except);

private:
    std::string label_;
    std::string group_;
    bool selected_ = false;
    Color activeColor_ = Color::rgb(33, 150, 243);
    OnBoolChangeHandler onChange_;
};

// ============================================================
// Dropdown / Select Widget
// ============================================================

class Dropdown : public Widget {
public:
    Dropdown() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Dropdown"; }

    void addItem(const std::string& item) { items_.push_back(item); markDirty(); }
    void setItems(const std::vector<std::string>& items) { items_ = items; markDirty(); }
    const std::vector<std::string>& getItems() const { return items_; }
    int getSelectedIndex() const { return selectedIndex_; }
    std::string getSelectedItem() const {
        return (selectedIndex_ >= 0 && selectedIndex_ < (int)items_.size()) ? items_[selectedIndex_] : "";
    }
    void setSelectedIndex(int idx) { selectedIndex_ = idx; markDirty(); }
    void setPlaceholder(const std::string& ph) { placeholder_ = ph; }
    void setOnChange(OnChangeHandler handler) { onChange_ = std::move(handler); }

private:
    std::vector<std::string> items_;
    int selectedIndex_ = -1;
    std::string placeholder_ = "Select...";
    bool open_ = false;
    int hoveredIndex_ = -1;
    FontSpec font_;
    OnChangeHandler onChange_;
};

// ============================================================
// Icon Widget
// ============================================================

class Icon : public Widget {
public:
    Icon() = default;
    explicit Icon(const std::string& icon, float size = 24.0f)
        : icon_(icon), size_(size) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    std::string typeName() const override { return "Icon"; }

    void setIcon(const std::string& icon) { icon_ = icon; markDirty(); }
    void setIconSize(float size) { size_ = size; markDirty(); }
    void setColor(const Color& color) { color_ = color; }

private:
    std::string icon_;  // Unicode character or icon name
    float size_ = 24.0f;
    Color color_ = Color::black();
};

// ============================================================
// Drawer Widget (side panel)
// ============================================================

class Drawer : public Widget {
public:
    Drawer() = default;
    explicit Drawer(float width) : drawerWidth_(width) {}

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Drawer"; }

    void setOpen(bool open) { open_ = open; markDirty(); }
    bool isOpen() const { return open_; }
    void setDrawerWidth(float w) { drawerWidth_ = w; }
    void setOnClose(OnClickHandler handler) { onClose_ = std::move(handler); }

private:
    bool open_ = false;
    float drawerWidth_ = 280.0f;
    float animProgress_ = 0.0f; // 0 = closed, 1 = open
    Color backdropColor_ = Color::rgba(0, 0, 0, 100);
    OnClickHandler onClose_;
};

// ============================================================
// TabBar Widget
// ============================================================

class TabBar : public Widget {
public:
    TabBar() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "TabBar"; }

    void addTab(const std::string& label) { tabs_.push_back(label); markDirty(); }
    void setTabs(const std::vector<std::string>& tabs) { tabs_ = tabs; markDirty(); }
    int getActiveTab() const { return activeTab_; }
    void setActiveTab(int idx) { activeTab_ = idx; markDirty(); }
    void setOnTabChange(std::function<void(int)> handler) { onTabChange_ = std::move(handler); }

private:
    std::vector<std::string> tabs_;
    int activeTab_ = 0;
    int hoveredTab_ = -1;
    Color activeColor_ = Color::rgb(33, 150, 243);
    Color inactiveColor_ = Color::rgb(117, 117, 117);
    std::function<void(int)> onTabChange_;
};

// ============================================================
// Menu / ContextMenu Widget
// ============================================================

struct MenuItem {
    std::string label;
    std::string icon;  // Unicode icon (optional)
    OnClickHandler onClick;
    bool enabled = true;
    bool separator = false; // If true, renders as a horizontal line
};

class Menu : public Widget {
public:
    Menu() = default;

    void layout(const Constraints& constraints) override;
    void paint(IRenderer& renderer) override;
    bool handleEvent(const Event& event) override;
    std::string typeName() const override { return "Menu"; }

    void addItem(const MenuItem& item) { items_.push_back(item); markDirty(); }
    void addSeparator() { items_.push_back({"", "", nullptr, true, true}); markDirty(); }
    void setItems(const std::vector<MenuItem>& items) { items_ = items; markDirty(); }

    void show(float x, float y) { posX_ = x; posY_ = y; open_ = true; markDirty(); }
    void hide() { open_ = false; markDirty(); }
    bool isOpen() const { return open_; }

private:
    std::vector<MenuItem> items_;
    bool open_ = false;
    float posX_ = 0, posY_ = 0;
    int hoveredIndex_ = -1;
    float itemHeight_ = 36.0f;
    FontSpec font_;
};

} // namespace gui
} // namespace stratos
