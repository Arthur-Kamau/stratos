#include "stratos/HTMLGenerator.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace stratos {
namespace fs = std::filesystem;

HTMLGenerator::HTMLGenerator() {}

std::string HTMLGenerator::nextId(const std::string& prefix) {
    return prefix + "-" + std::to_string(idCounter_++);
}

std::string HTMLGenerator::ind(int level) {
    return std::string(level * 2, ' ');
}

// ============================================================================
// Main Entry Point
// ============================================================================

HTMLGenerateResult HTMLGenerator::generate(const stui::STUIFile& file,
                                            const std::string& outputDir,
                                            const std::string& title) {
    HTMLGenerateResult result;
    fs::create_directories(outputDir);

    // Reset state
    html_.str(""); css_.str(""); js_.str("");
    idCounter_ = 0;
    stateVars_.clear();

    generateCSS(file);
    generateJS(file);
    generateHTML(file, title);

    // Write files
    {
        std::ofstream out(outputDir + "/index.html");
        out << html_.str();
        result.htmlPath = outputDir + "/index.html";
    }
    {
        std::ofstream out(outputDir + "/styles.css");
        out << css_.str();
        result.cssPath = outputDir + "/styles.css";
    }
    {
        std::ofstream out(outputDir + "/app.js");
        out << js_.str();
        result.jsPath = outputDir + "/app.js";
    }

    result.success = true;
    return result;
}

// ============================================================================
// HTML Generation
// ============================================================================

void HTMLGenerator::generateHTML(const stui::STUIFile& file, const std::string& title) {
    html_ << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html_ << "  <meta charset=\"UTF-8\">\n";
    html_ << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html_ << "  <title>" << title << "</title>\n";
    html_ << "  <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">\n";
    html_ << "  <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>\n";
    html_ << "  <link href=\"https://fonts.googleapis.com/css2?family=Inter:ital,opsz,wght@0,14..32,100..900;1,14..32,100..900&display=swap\" rel=\"stylesheet\">\n";
    html_ << "  <link rel=\"stylesheet\" href=\"styles.css\">\n";
    html_ << "</head>\n<body>\n";
    html_ << "  <div id=\"app\">\n";

    // Find App component and render its view
    for (const auto& comp : file.components) {
        if (comp.name == "App" && comp.viewRoot) {
            // If root is Window, skip it and render children
            if (comp.viewRoot->widgetType == "Window") {
                for (const auto& child : comp.viewRoot->children) {
                    generateWidget(*child, 2);
                }
            } else {
                generateWidget(*comp.viewRoot, 2);
            }
        }
    }

    html_ << "  </div>\n";
    html_ << "  <script src=\"app.js\"></script>\n";
    html_ << "</body>\n</html>\n";
}

void HTMLGenerator::generateWidget(const stui::WidgetNode& widget, int indent) {
    std::string id = nextId(widget.widgetType);

    // Check if this is a component reference (not a built-in widget)
    // Components are PascalCase names that aren't standard widgets
    static const std::vector<std::string> builtinWidgets = {
        "Text", "Button", "Image", "Box", "Container", "Spacer",
        "TextField", "TextArea", "Checkbox", "RadioButton", "Switch", "Slider", "Dropdown",
        "Row", "Column", "Stack", "Grid", "ScrollView", "ListView", "Padding", "Center",
        "AppBar", "Drawer", "TabBar", "Dialog", "Modal", "Menu",
        "Window"
    };

    bool isBuiltin = std::find(builtinWidgets.begin(), builtinWidgets.end(), widget.widgetType) != builtinWidgets.end();

    if (!isBuiltin) {
        // Component reference — render as a div with component class
        html_ << ind(indent) << "<div class=\"component-" << widget.widgetType << "\" id=\"" << id << "\">";
        html_ << "<!-- " << widget.widgetType << " component -->";
        html_ << "</div>\n";
        return;
    }

    generateWidgetOpen(widget, id, indent);
    generateWidgetCSS(widget, id);
    generateEventJS(widget, id);

    // Render children
    for (const auto& child : widget.children) {
        generateWidget(*child, indent + 1);
    }

    generateWidgetClose(widget, indent);
}

void HTMLGenerator::generateWidgetOpen(const stui::WidgetNode& widget, const std::string& id, int indent) {
    std::string tag = getHtmlTag(widget.widgetType);
    std::string classes = getWidgetClasses(widget);

    if (widget.widgetType == "Text") {
        html_ << ind(indent) << "<" << tag << " class=\"" << classes << "\" id=\"" << id << "\">";
        // Text content from first positional arg
        if (!widget.arguments.empty()) {
            html_ << exprToString(*widget.arguments[0]);
        }
    } else if (widget.widgetType == "Button") {
        html_ << ind(indent) << "<button class=\"" << classes << "\" id=\"" << id << "\">";
        if (!widget.arguments.empty()) {
            html_ << exprToString(*widget.arguments[0]);
        }
    } else if (widget.widgetType == "Image") {
        std::string src;
        if (!widget.arguments.empty()) src = exprToString(*widget.arguments[0]);
        html_ << ind(indent) << "<img class=\"" << classes << "\" id=\"" << id << "\" src=\"" << src << "\" alt=\"\"";
        html_ << " />\n";
        return; // self-closing
    } else if (widget.widgetType == "TextField") {
        std::string placeholder;
        if (!widget.arguments.empty()) placeholder = exprToString(*widget.arguments[0]);
        html_ << ind(indent) << "<input type=\"text\" class=\"" << classes << "\" id=\"" << id << "\" placeholder=\"" << placeholder << "\"";
        html_ << " />\n";
        return; // self-closing
    } else if (widget.widgetType == "TextArea") {
        std::string placeholder;
        if (!widget.arguments.empty()) placeholder = exprToString(*widget.arguments[0]);
        html_ << ind(indent) << "<textarea class=\"" << classes << "\" id=\"" << id << "\" placeholder=\"" << placeholder << "\"></textarea>\n";
        return;
    } else if (widget.widgetType == "Checkbox") {
        html_ << ind(indent) << "<label class=\"" << classes << "\" id=\"" << id << "\"><input type=\"checkbox\" /> ";
        if (!widget.arguments.empty()) html_ << exprToString(*widget.arguments[0]);
        html_ << "</label>\n";
        return;
    } else if (widget.widgetType == "Switch") {
        html_ << ind(indent) << "<label class=\"st-switch " << classes << "\" id=\"" << id << "\"><input type=\"checkbox\" /><span class=\"st-switch-slider\"></span></label>\n";
        return;
    } else if (widget.widgetType == "Slider") {
        html_ << ind(indent) << "<input type=\"range\" class=\"" << classes << "\" id=\"" << id << "\"";
        for (const auto& prop : widget.properties) {
            if (prop.name == "min") html_ << " min=\"" << exprToString(*prop.value) << "\"";
            else if (prop.name == "max") html_ << " max=\"" << exprToString(*prop.value) << "\"";
            else if (prop.name == "value") html_ << " value=\"" << exprToString(*prop.value) << "\"";
        }
        html_ << " />\n";
        return;
    } else if (widget.widgetType == "RadioButton") {
        html_ << ind(indent) << "<label class=\"" << classes << "\" id=\"" << id << "\"><input type=\"radio\"";
        for (const auto& prop : widget.properties) {
            if (prop.name == "group") html_ << " name=\"" << exprToString(*prop.value) << "\"";
            if (prop.name == "value") html_ << " value=\"" << exprToString(*prop.value) << "\"";
        }
        html_ << " /> ";
        if (!widget.arguments.empty()) html_ << exprToString(*widget.arguments[0]);
        html_ << "</label>\n";
        return;
    } else if (widget.widgetType == "Dropdown") {
        html_ << ind(indent) << "<select class=\"" << classes << "\" id=\"" << id << "\">\n";
        // Render children as <option> elements
        for (const auto& child : widget.children) {
            html_ << ind(indent + 1) << "<option>" << child->widgetType << "</option>\n";
        }
        html_ << ind(indent) << "</select>\n";
        return;
    } else if (widget.widgetType == "Icon") {
        html_ << ind(indent) << "<span class=\"" << classes << "\" id=\"" << id << "\">";
        if (!widget.arguments.empty()) html_ << exprToString(*widget.arguments[0]);
        html_ << "</span>\n";
        return;
    } else if (widget.widgetType == "AppBar") {
        html_ << ind(indent) << "<header class=\"" << classes << "\" id=\"" << id << "\">";
        if (!widget.arguments.empty()) {
            html_ << "<h1>" << exprToString(*widget.arguments[0]) << "</h1>";
        }
    } else if (widget.widgetType == "Dialog" || widget.widgetType == "Modal") {
        html_ << ind(indent) << "<dialog class=\"" << classes << "\" id=\"" << id << "\">\n";
    } else if (widget.widgetType == "Drawer") {
        html_ << ind(indent) << "<aside class=\"" << classes << "\" id=\"" << id << "\">\n";
    } else if (widget.widgetType == "TabBar") {
        html_ << ind(indent) << "<nav class=\"" << classes << "\" id=\"" << id << "\" role=\"tablist\">\n";
    } else if (widget.widgetType == "Menu") {
        html_ << ind(indent) << "<div class=\"" << classes << "\" id=\"" << id << "\" role=\"menu\">\n";
    } else if (widget.widgetType == "Spacer") {
        html_ << ind(indent) << "<div class=\"" << classes << "\" id=\"" << id << "\"></div>\n";
        return;
    } else {
        // Generic container: Row, Column, Stack, Grid, Box, Container, ScrollView, etc.
        html_ << ind(indent) << "<div class=\"" << classes << "\" id=\"" << id << "\">\n";
    }
}

void HTMLGenerator::generateWidgetClose(const stui::WidgetNode& widget, int indent) {
    std::string tag = getHtmlTag(widget.widgetType);

    if (widget.widgetType == "Text") {
        html_ << "</" << tag << ">\n";
    } else if (widget.widgetType == "Button") {
        html_ << "</button>\n";
    } else if (widget.widgetType == "AppBar") {
        html_ << "</header>\n";
    } else if (widget.widgetType == "Dialog" || widget.widgetType == "Modal") {
        html_ << ind(indent) << "</dialog>\n";
    } else if (widget.widgetType == "Drawer") {
        html_ << ind(indent) << "</aside>\n";
    } else if (widget.widgetType == "TabBar") {
        html_ << ind(indent) << "</nav>\n";
    } else if (widget.widgetType == "Menu") {
        html_ << ind(indent) << "</div>\n";
    } else {
        html_ << ind(indent) << "</div>\n";
    }
}

std::string HTMLGenerator::getHtmlTag(const std::string& widgetType) {
    if (widgetType == "Text") return "span";
    if (widgetType == "Button") return "button";
    if (widgetType == "Image") return "img";
    if (widgetType == "TextField") return "input";
    if (widgetType == "TextArea") return "textarea";
    if (widgetType == "AppBar") return "header";
    if (widgetType == "Dialog" || widgetType == "Modal") return "dialog";
    if (widgetType == "ListView") return "ul";
    return "div";
}

std::string HTMLGenerator::getWidgetClasses(const stui::WidgetNode& widget) {
    std::string cls = "st-" + widget.widgetType;
    // Lowercase the class name
    std::transform(cls.begin(), cls.end(), cls.begin(), ::tolower);
    return cls;
}

// ============================================================================
// CSS Generation
// ============================================================================

void HTMLGenerator::generateCSS(const stui::STUIFile& file) {
    generateBaseStyles();

    // Generate component-specific styles from widget properties
    // This is done during widget traversal in generateWidgetCSS
}

void HTMLGenerator::generateBaseStyles() {
    css_ << R"(/* Stratos Web — Generated Styles */
:root {
  --st-primary: #6366f1;
  --st-primary-hover: #4f46e5;
  --st-bg: #1a1a2e;
  --st-surface: #16213e;
  --st-surface-hover: #1a2744;
  --st-text: #e0e0e0;
  --st-text-secondary: #888;
  --st-border: #0f3460;
  --st-radius: 8px;
  --st-shadow: 0 2px 8px rgba(0,0,0,0.3);
}

* { margin: 0; padding: 0; box-sizing: border-box; }

body {
  font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;
  background: var(--st-bg);
  color: var(--st-text);
  min-height: 100vh;
}

#app { min-height: 100vh; }

/* Layout */
.st-row { display: flex; flex-direction: row; }
.st-column { display: flex; flex-direction: column; }
.st-stack { position: relative; }
.st-stack > * { position: absolute; }
.st-grid { display: grid; }
.st-scrollview { overflow: auto; }
.st-center { display: flex; align-items: center; justify-content: center; }
.st-padding { padding: 16px; }
.st-spacer { flex: 1; }
.st-listview { list-style: none; }

/* Widgets */
.st-text { display: inline-block; }

.st-button {
  background: var(--st-primary);
  color: white;
  border: none;
  padding: 8px 16px;
  border-radius: var(--st-radius);
  cursor: pointer;
  font-size: 14px;
  transition: background 0.2s;
}
.st-button:hover { background: var(--st-primary-hover); }
.st-button:active { transform: scale(0.98); }

.st-textfield, .st-textarea {
  background: var(--st-surface);
  color: var(--st-text);
  border: 1px solid var(--st-border);
  padding: 8px 12px;
  border-radius: var(--st-radius);
  font-size: 14px;
  outline: none;
  transition: border-color 0.2s;
}
.st-textfield:focus, .st-textarea:focus { border-color: var(--st-primary); }

.st-checkbox { display: flex; align-items: center; gap: 8px; cursor: pointer; }

.st-switch { position: relative; display: inline-block; width: 48px; height: 24px; }
.st-switch input { opacity: 0; width: 0; height: 0; }
.st-switch-slider {
  position: absolute; inset: 0;
  background: #444; border-radius: 24px;
  transition: 0.3s;
}
.st-switch-slider::before {
  content: ''; position: absolute;
  width: 18px; height: 18px; left: 3px; bottom: 3px;
  background: white; border-radius: 50%; transition: 0.3s;
}
.st-switch input:checked + .st-switch-slider { background: var(--st-primary); }
.st-switch input:checked + .st-switch-slider::before { transform: translateX(24px); }

.st-slider { width: 100%; accent-color: var(--st-primary); }

.st-appbar {
  background: var(--st-surface);
  padding: 12px 24px;
  border-bottom: 1px solid var(--st-border);
  display: flex; align-items: center;
}
.st-appbar h1 { font-size: 18px; font-weight: 600; }

.st-box, .st-container {
  background: var(--st-surface);
  border-radius: var(--st-radius);
}

.st-dialog {
  background: var(--st-surface);
  color: var(--st-text);
  border: 1px solid var(--st-border);
  border-radius: var(--st-radius);
  padding: 24px;
  box-shadow: var(--st-shadow);
}
.st-dialog::backdrop { background: rgba(0,0,0,0.5); }

.st-image { max-width: 100%; height: auto; }
.st-dropdown {
  background: var(--st-surface);
  color: var(--st-text);
  border: 1px solid var(--st-border);
  padding: 8px 12px;
  border-radius: var(--st-radius);
}

.st-radiobutton { display: flex; align-items: center; gap: 8px; cursor: pointer; }
.st-radiobutton input[type="radio"] { accent-color: var(--st-primary); }

.st-icon { display: inline-flex; align-items: center; justify-content: center; }

.st-drawer {
  position: fixed; top: 0; left: 0; bottom: 0;
  width: 280px; background: var(--st-surface);
  border-right: 1px solid var(--st-border);
  transform: translateX(-100%); transition: transform 0.3s ease;
  z-index: 100; overflow-y: auto;
}
.st-drawer.open { transform: translateX(0); }

.st-tabbar {
  display: flex; border-bottom: 2px solid var(--st-border);
  background: var(--st-surface);
}
.st-tabbar > * {
  padding: 12px 24px; cursor: pointer;
  border-bottom: 2px solid transparent; margin-bottom: -2px;
  color: var(--st-text-secondary); transition: all 0.2s;
}
.st-tabbar > *.active {
  color: var(--st-primary); border-bottom-color: var(--st-primary);
}

.st-menu {
  position: absolute; background: var(--st-surface);
  border: 1px solid var(--st-border); border-radius: var(--st-radius);
  box-shadow: var(--st-shadow); min-width: 180px; z-index: 200;
  display: none;
}
.st-menu.open { display: block; }
.st-menu [role="menuitem"] {
  padding: 8px 16px; cursor: pointer; transition: background 0.15s;
}
.st-menu [role="menuitem"]:hover { background: rgba(255,255,255,0.05); }
.st-menu hr { border: none; border-top: 1px solid var(--st-border); margin: 4px 0; }
)";
}

void HTMLGenerator::generateWidgetCSS(const stui::WidgetNode& widget, const std::string& id) {
    bool hasCustomStyles = false;
    std::stringstream styles;

    for (const auto& prop : widget.properties) {
        std::string cssVal = propertyToCSS(prop.name, exprToString(*prop.value));
        if (!cssVal.empty()) {
            styles << cssVal;
            hasCustomStyles = true;
        }
    }

    if (hasCustomStyles) {
        css_ << "#" << id << " { " << styles.str() << "}\n";
    }
}

std::string HTMLGenerator::propertyToCSS(const std::string& name, const std::string& value) {
    // Map STUI property names to CSS
    if (name == "fontSize") return "font-size: " + value + "px; ";
    if (name == "fontWeight") return "font-weight: " + value + "; ";
    if (name == "color") return "color: " + value + "; ";
    if (name == "backgroundColor") return "background-color: " + value + "; ";
    if (name == "padding") return "padding: " + value + "px; ";
    if (name == "margin") return "margin: " + value + "px; ";
    if (name == "borderRadius") return "border-radius: " + value + "px; ";
    if (name == "width") return "width: " + value + "px; ";
    if (name == "height") return "height: " + value + "px; ";
    if (name == "spacing") return "gap: " + value + "px; ";
    if (name == "flex") return "flex: " + value + "; ";
    if (name == "alignment") {
        if (value == "center") return "align-items: center; justify-content: center; ";
        if (value == "start") return "align-items: flex-start; ";
        if (value == "end") return "align-items: flex-end; ";
    }
    if (name == "columns") return "grid-template-columns: repeat(" + value + ", 1fr); ";
    return "";
}

// ============================================================================
// JS Generation
// ============================================================================

void HTMLGenerator::generateJS(const stui::STUIFile& file) {
    js_ << "// Stratos Web — Generated App\n";
    js_ << "'use strict';\n\n";

    // Reactive state system
    js_ << R"(const _state = {};
const _watchers = {};
function createState(name, initial) {
  _state[name] = initial;
  _watchers[name] = [];
  return {
    get() { return _state[name]; },
    set(v) { _state[name] = v; _watchers[name].forEach(fn => fn(v)); },
    watch(fn) { _watchers[name].push(fn); }
  };
}
)";
    js_ << "\n";

    // Generate state + event code for each component
    for (const auto& comp : file.components) {
        if (!comp.states.empty()) {
            js_ << "// State for " << comp.name << "\n";
            generateStateJS(comp);
        }
    }

    // Event bindings generated during widget traversal
    js_ << "\n// Event bindings\n";
    js_ << "document.addEventListener('DOMContentLoaded', () => {\n";

    // Re-traverse widgets to generate event bindings
    for (const auto& comp : file.components) {
        if (comp.name == "App" && comp.viewRoot) {
            if (comp.viewRoot->widgetType == "Window") {
                for (const auto& child : comp.viewRoot->children) {
                    generateEventJS(*child, "");
                }
            } else {
                generateEventJS(*comp.viewRoot, "");
            }
        }
    }

    js_ << "});\n";
}

void HTMLGenerator::generateStateJS(const stui::ComponentDecl& comp) {
    for (const auto& state : comp.states) {
        std::string initVal = state.initialValue ? exprToJS(*state.initialValue) : "null";
        js_ << "const " << state.name << " = createState('" << state.name << "', " << initVal << ");\n";
        stateVars_.push_back(state.name);
    }
    js_ << "\n";
}

void HTMLGenerator::generateEventJS(const stui::WidgetNode& widget, const std::string&) {
    // We need the element ID — since IDs are generated sequentially,
    // we track them by regenerating the same sequence
    std::string id = widget.widgetType + "-" + std::to_string(idCounter_);

    for (const auto& event : widget.events) {
        std::string domEvent;
        if (event.eventName == "onClick") domEvent = "click";
        else if (event.eventName == "onChange") domEvent = "input";
        else if (event.eventName == "onSubmit") domEvent = "submit";
        else if (event.eventName == "onKeyPress") domEvent = "keypress";
        else if (event.eventName == "onMouseEnter") domEvent = "mouseenter";
        else if (event.eventName == "onMouseLeave") domEvent = "mouseleave";
        else domEvent = event.eventName.substr(2); // strip "on" prefix

        // Convert to lowercase first char
        if (!domEvent.empty()) {
            domEvent[0] = tolower(domEvent[0]);
        }

        js_ << "  {\n";
        js_ << "    const el = document.getElementById('" << id << "');\n";
        js_ << "    if (el) el.addEventListener('" << domEvent << "', (e) => {\n";

        // Generate handler body
        if (auto* lambda = dynamic_cast<const stui::LambdaExpr*>(event.handler.get())) {
            if (!lambda->bodySource.empty()) {
                js_ << "      " << lambda->bodySource << "\n";
            }
        }

        js_ << "    });\n";
        js_ << "  }\n";
    }

    for (const auto& child : widget.children) {
        generateEventJS(*child, "");
    }
}

// ============================================================================
// Expression Helpers
// ============================================================================

std::string HTMLGenerator::exprToString(const stui::STUIExpr& expr) {
    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(&expr)) {
        return lit->token.lexeme;
    }
    if (auto* id = dynamic_cast<const stui::IdentifierExpr*>(&expr)) {
        return id->name.lexeme;
    }
    if (auto* interp = dynamic_cast<const stui::InterpolatedStringExpr*>(&expr)) {
        return interp->raw;
    }
    if (auto* mem = dynamic_cast<const stui::MemberAccessExpr*>(&expr)) {
        return exprToString(*mem->object) + "." + mem->member.lexeme;
    }
    return "";
}

std::string HTMLGenerator::exprToJS(const stui::STUIExpr& expr) {
    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(&expr)) {
        if (lit->token.type == stui::STUITokenType::STRING) {
            return "\"" + lit->token.lexeme + "\"";
        }
        return lit->token.lexeme;
    }
    if (auto* id = dynamic_cast<const stui::IdentifierExpr*>(&expr)) {
        return id->name.lexeme;
    }
    if (auto* bin = dynamic_cast<const stui::BinaryExpr*>(&expr)) {
        return exprToJS(*bin->left) + " " + bin->op.lexeme + " " + exprToJS(*bin->right);
    }
    return "null";
}

} // namespace stratos
