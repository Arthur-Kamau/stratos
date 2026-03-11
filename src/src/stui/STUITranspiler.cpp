#include "stratos/STUITranspiler.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include <iostream>
#include <algorithm>
#include <map>

// Aliases to disambiguate Stratos AST types from STUI AST types
// (both namespaces have LiteralExpr, BinaryExpr, etc.)
namespace st = stratos;
using SLiteralExpr = stratos::LiteralExpr;
using SBinaryExpr = stratos::BinaryExpr;
using SUnaryExpr = stratos::UnaryExpr;
using SCallExpr = stratos::CallExpr;
using SVariableExpr = stratos::VariableExpr;
using SLambdaExpr = stratos::LambdaExpr;

namespace stratos {
namespace stui {

STUITranspiler::STUITranspiler() {}

// ============================================================================
// Token/AST Helper Factories
// ============================================================================

Token STUITranspiler::makeToken(TokenType type, const std::string& lexeme) {
    return Token{type, lexeme, ++syntheticLine_, 1, "<stui-generated>", ""};
}

Token STUITranspiler::makeIdentifier(const std::string& name) {
    return makeToken(TokenType::IDENTIFIER, name);
}

Token STUITranspiler::makeString(const std::string& value) {
    return makeToken(TokenType::STRING, value);
}

Token STUITranspiler::makeNumber(const std::string& value) {
    return makeToken(TokenType::NUMBER, value);
}

std::unique_ptr<Expr> STUITranspiler::makeVar(const std::string& name) {
    return std::make_unique<SVariableExpr>(makeIdentifier(name));
}

std::unique_ptr<Expr> STUITranspiler::makeString(const std::string& value, bool) {
    return std::make_unique<SLiteralExpr>(value, TokenType::STRING);
}

std::unique_ptr<Expr> STUITranspiler::makeNumber(int value) {
    return std::make_unique<SLiteralExpr>(std::to_string(value), TokenType::NUMBER);
}

std::unique_ptr<Expr> STUITranspiler::makeNumber(double value) {
    return std::make_unique<SLiteralExpr>(std::to_string(value), TokenType::NUMBER);
}

std::unique_ptr<Expr> STUITranspiler::makeCall(const std::string& callee,
                                                std::vector<std::unique_ptr<Expr>> args) {
    // Check if callee has a dot (e.g., "gui.App")
    auto dotPos = callee.find('.');
    std::unique_ptr<Expr> calleeExpr;
    if (dotPos != std::string::npos) {
        std::string obj = callee.substr(0, dotPos);
        std::string method = callee.substr(dotPos + 1);
        // Build member access: obj.method
        auto objExpr = std::make_unique<SVariableExpr>(makeIdentifier(obj));
        // For Stratos, member calls are CallExpr with a dotted callee
        // We'll use a VariableExpr with the full dotted name
        calleeExpr = std::make_unique<SVariableExpr>(makeIdentifier(callee));
    } else {
        calleeExpr = std::make_unique<SVariableExpr>(makeIdentifier(callee));
    }

    return std::make_unique<SCallExpr>(
        std::move(calleeExpr),
        makeToken(TokenType::LEFT_PAREN, "("),
        std::move(args));
}

std::unique_ptr<Expr> STUITranspiler::makeMemberCall(const std::string& object,
                                                      const std::string& method,
                                                      std::vector<std::unique_ptr<Expr>> args) {
    // object.method(args) → CallExpr with BinaryExpr(DOT) callee
    auto objExpr = std::make_unique<SVariableExpr>(makeIdentifier(object));
    auto methodExpr = std::make_unique<SVariableExpr>(makeIdentifier(method));
    Token dotToken;
    dotToken.type = TokenType::DOT;
    dotToken.lexeme = ".";
    auto dotExpr = std::make_unique<SBinaryExpr>(
        std::move(objExpr), dotToken, std::move(methodExpr));

    Token paren;
    paren.type = TokenType::LEFT_PAREN;
    paren.lexeme = "(";
    return std::make_unique<SCallExpr>(
        std::move(dotExpr), paren,
        std::move(args));
}

std::unique_ptr<Expr> STUITranspiler::makeMapLiteral(
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> entries) {
    return std::make_unique<MapLiteralExpr>(std::move(entries));
}

std::unique_ptr<Expr> STUITranspiler::makeArrayLiteral(
    std::vector<std::unique_ptr<Expr>> elements) {
    return std::make_unique<ArrayLiteralExpr>(std::move(elements));
}

std::unique_ptr<Stmt> STUITranspiler::makeVarDecl(const std::string& name,
                                                    std::unique_ptr<Expr> init,
                                                    bool isMutable) {
    return std::make_unique<VarDecl>(
        makeIdentifier(name), "", std::move(init), isMutable);
}

std::unique_ptr<Stmt> STUITranspiler::makeExprStmt(std::unique_ptr<Expr> expr) {
    return std::make_unique<ExpressionStmt>(std::move(expr));
}

// ============================================================================
// Main Transpilation
// ============================================================================

std::vector<std::unique_ptr<Stmt>> STUITranspiler::transpile(const STUIFile& file) {
    std::vector<std::unique_ptr<Stmt>> statements;

    // 1. Package declaration
    if (!file.packageName.empty()) {
        std::vector<std::unique_ptr<Stmt>> pkgDecls;
        statements.push_back(std::make_unique<PackageDecl>(
            makeIdentifier(file.packageName), std::move(pkgDecls)));
    }

    // 2. Imports — always include std/gui
    auto imports = transpileImports(file);
    for (auto& imp : imports) {
        statements.push_back(std::move(imp));
    }

    // Collect component names for later use (to distinguish custom components from built-in widgets)
    componentNames_.clear();
    for (const auto& comp : file.components) {
        componentNames_.insert(comp.name);
    }

    // 3. Each component → a class or a function
    //    Skip the "App" component since generateMain() handles it inline.
    //    Generating fn App() would shadow the gui.App class.
    for (const auto& component : file.components) {
        if (component.name == "App") continue;
        statements.push_back(transpileComponent(component));
    }

    // 4. Generate main() if there's an "App" component
    bool hasApp = false;
    for (const auto& comp : file.components) {
        if (comp.name == "App") {
            hasApp = true;
            break;
        }
    }
    if (hasApp) {
        statements.push_back(generateMain(file));
    }

    return statements;
}

std::vector<std::unique_ptr<Stmt>> STUITranspiler::transpileImports(const STUIFile& file) {
    std::vector<std::unique_ptr<Stmt>> imports;

    // Always import std/gui
    bool hasGuiImport = false;
    for (const auto& imp : file.imports) {
        if (imp == "std/gui") hasGuiImport = true;
        imports.push_back(std::make_unique<UseStmt>(makeToken(TokenType::STRING, imp)));
    }

    if (!hasGuiImport) {
        imports.insert(imports.begin(),
            std::make_unique<UseStmt>(makeToken(TokenType::STRING, "std/gui")));
    }

    return imports;
}

// ============================================================================
// Component → Function
// ============================================================================

// Each component becomes a function that:
// 1. Creates state variables
// 2. Builds the widget tree
// 3. Returns the root widget
//
// component Counter { state count: int = 0; view { ... } }
// →
// fn Counter() {
//     val count = gui.State(0);
//     val root = gui.Column({ ... }, [ ... ]);
//     return root;
// }

std::unique_ptr<Stmt> STUITranspiler::transpileComponent(const ComponentDecl& component) {
    std::vector<std::unique_ptr<Stmt>> bodyStmts;

    // Collect state names for signal-aware transpilation
    currentStateNames_.clear();
    for (const auto& state : component.states) {
        currentStateNames_.insert(state.name);
    }

    // State declarations → gui.State() calls
    for (const auto& state : component.states) {
        bodyStmts.push_back(transpileStateDecl(state));
    }

    // Build the widget tree
    if (component.viewRoot) {
        std::vector<std::unique_ptr<Stmt>> widgetSetup;
        auto widgetExpr = transpileWidget(*component.viewRoot, widgetSetup);
        for (auto& s : widgetSetup) bodyStmts.push_back(std::move(s));
        bodyStmts.push_back(makeVarDecl("__root__", std::move(widgetExpr)));

        // Return the root widget
        bodyStmts.push_back(std::make_unique<ReturnStmt>(
            makeToken(TokenType::IDENTIFIER, "return"),
            makeVar("__root__")));
    }

    // Create function body
    auto body = std::make_unique<std::vector<std::unique_ptr<Stmt>>>();
    for (auto& stmt : bodyStmts) {
        body->push_back(std::move(stmt));
    }

    // Parameters from props (with default values if specified)
    std::vector<Parameter> params;
    for (const auto& prop : component.props) {
        std::unique_ptr<Expr> defaultVal = nullptr;
        if (prop.defaultValue) {
            defaultVal = transpileExpr(*prop.defaultValue);
        }
        params.push_back(Parameter(
            makeIdentifier(prop.name),
            prop.typeName,
            std::move(defaultVal), false));
    }

    return std::make_unique<FunctionDecl>(
        makeIdentifier(component.name),
        std::move(params),
        "any", // return type
        std::move(body),
        true,  // isPublic
        false  // isAsync
    );
}

// ============================================================================
// State → State()
// ============================================================================

std::unique_ptr<Stmt> STUITranspiler::transpileStateDecl(const StateDecl& state) {
    // state count: int = 0;  →  val count = gui.State(0);
    std::vector<std::unique_ptr<Expr>> args;
    if (state.initialValue) {
        args.push_back(transpileExpr(*state.initialValue));
    }
    auto stateCall = makeCall("State", std::move(args));
    return makeVarDecl(state.name, std::move(stateCall));
}

// ============================================================================
// Widget Tree → Widget() calls
// ============================================================================

std::string STUITranspiler::nextTempVar() {
    return "__w" + std::to_string(tempVarCounter_++);
}

// Ensure a numeric STUI expression becomes a float literal (append .0 to ints)
std::unique_ptr<Expr> STUITranspiler::transpileAsFloat(const STUIExpr& expr) {
    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(&expr)) {
        if (lit->token.type == STUITokenType::NUMBER) {
            std::string val = lit->token.lexeme;
            if (val.find('.') == std::string::npos) val += ".0";
            return std::make_unique<SLiteralExpr>(val, TokenType::NUMBER);
        }
    }
    return transpileExpr(expr);
}

// Simple version (no setup statements) — used by callers that don't need styling
std::unique_ptr<Expr> STUITranspiler::transpileWidget(const WidgetNode& widget) {
    std::vector<std::unique_ptr<Stmt>> unused;
    return transpileWidget(widget, unused);
}

std::unique_ptr<Expr> STUITranspiler::transpileWidget(const WidgetNode& widget,
                                                       std::vector<std::unique_ptr<Stmt>>& setupStmts) {
    // Window is a STUI-only concept — skip it and transpile children directly
    if (widget.widgetType == "Window") {
        if (widget.children.size() == 1) {
            return transpileWidget(*widget.children[0], setupStmts);
        } else if (widget.children.size() > 1) {
            std::vector<std::unique_ptr<Expr>> childExprs;
            for (const auto& child : widget.children) {
                childExprs.push_back(transpileWidget(*child, setupStmts));
            }
            std::vector<std::unique_ptr<Expr>> colArgs;
            colArgs.push_back(makeArrayLiteral(std::move(childExprs)));
            return makeCall("Column", std::move(colArgs));
        }
        return makeCall("Center", {});
    }

    // Check if this is a custom component (not a built-in widget)
    bool isCustomComponent = componentNames_.count(widget.widgetType) > 0;

    if (isCustomComponent) {
        // Custom component: all properties become function call arguments (positional, by prop order)
        std::vector<std::unique_ptr<Expr>> callArgs;
        // Positional arguments first
        for (const auto& arg : widget.arguments) {
            callArgs.push_back(transpileExpr(*arg));
        }
        // Named properties become additional positional args
        for (const auto& prop : widget.properties) {
            callArgs.push_back(transpileExpr(*prop.value));
        }
        return makeCall(widget.widgetType, std::move(callArgs));
    }

    // Built-in widget: separate properties into constructor args vs post-creation styling
    std::vector<std::unique_ptr<Expr>> callArgs;
    std::vector<const Property*> styleProps;

    // Positional arguments first
    for (const auto& arg : widget.arguments) {
        callArgs.push_back(transpileExpr(*arg));
    }

    // Collect constructor args from named properties (separate from style props).
    // We store them by name so we can order them correctly for each widget type.
    std::map<std::string, const Property*> ctorProps;
    for (const auto& prop : widget.properties) {
        bool isCtorArg =
            (prop.name == "spacing" && (widget.widgetType == "Row" || widget.widgetType == "Column" || widget.widgetType == "Grid")) ||
            (prop.name == "columns" && widget.widgetType == "Grid") ||
            ((prop.name == "min" || prop.name == "max") && widget.widgetType == "Slider") ||
            (prop.name == "group" && widget.widgetType == "RadioButton") ||
            (prop.name == "width" && widget.widgetType == "Drawer") ||
            (prop.name == "padding" && widget.widgetType == "Padding") ||
            (prop.name == "height" && widget.widgetType == "Spacer");
        if (isCtorArg) {
            ctorProps[prop.name] = &prop;
        } else {
            styleProps.push_back(&prop);
        }
    }

    // Build constructor args in the correct order for each widget type
    // (only if no positional args were already provided)
    if (callArgs.empty()) {
        // Define parameter order for each widget type
        std::vector<std::pair<std::string, bool>> paramOrder; // {name, isFloat}
        if (widget.widgetType == "Row" || widget.widgetType == "Column") {
            paramOrder = {{"spacing", true}};
        } else if (widget.widgetType == "Grid") {
            paramOrder = {{"columns", false}, {"spacing", true}};
        } else if (widget.widgetType == "Slider") {
            paramOrder = {{"min", true}, {"max", true}};
        } else if (widget.widgetType == "RadioButton") {
            paramOrder = {{"group", false}};
        } else if (widget.widgetType == "Drawer") {
            paramOrder = {{"width", true}};
        } else if (widget.widgetType == "Padding") {
            paramOrder = {{"padding", true}};
        } else if (widget.widgetType == "Spacer") {
            paramOrder = {{"height", true}};
        }

        for (const auto& [paramName, isFloat] : paramOrder) {
            auto it = ctorProps.find(paramName);
            if (it != ctorProps.end()) {
                if (isFloat) {
                    callArgs.push_back(transpileAsFloat(*it->second->value));
                } else {
                    callArgs.push_back(transpileExpr(*it->second->value));
                }
            }
        }
    }

    // Add default constructor args for widgets that require them
    if (callArgs.empty()) {
        if (widget.widgetType == "Row" || widget.widgetType == "Column" || widget.widgetType == "Grid") {
            callArgs.push_back(std::make_unique<SLiteralExpr>("0.0", TokenType::NUMBER));
        } else if (widget.widgetType == "Padding") {
            callArgs.push_back(std::make_unique<SLiteralExpr>("0.0", TokenType::NUMBER));
        }
        // Grid also needs columns as first arg
        if (widget.widgetType == "Grid" && callArgs.size() == 1) {
            // Insert columns before spacing: Grid(columns, spacing, children)
            auto spacing = std::move(callArgs[0]);
            callArgs[0] = std::make_unique<SLiteralExpr>("1", TokenType::NUMBER);
            callArgs.push_back(std::move(spacing));
        }
    }

    // Children → array literal
    if (!widget.children.empty()) {
        std::vector<std::unique_ptr<Expr>> childExprs;
        for (const auto& child : widget.children) {
            childExprs.push_back(transpileWidget(*child, setupStmts));
        }
        callArgs.push_back(makeArrayLiteral(std::move(childExprs)));
    }

    // Map Spacer with a size argument to FixedSpacer
    std::string resolvedType = widget.widgetType;
    if (resolvedType == "Spacer" && !callArgs.empty()) {
        resolvedType = "FixedSpacer";
    }

    // Check if Text widget has state-dependent interpolated strings (needs bindText)
    bool needsTextBinding = false;
    if (widget.widgetType == "Text" && !widget.arguments.empty() && !currentStateNames_.empty()) {
        auto* interp = dynamic_cast<const stui::InterpolatedStringExpr*>(widget.arguments[0].get());
        if (interp) {
            const std::string& raw = interp->raw;
            for (const auto& sn : currentStateNames_) {
                if (raw.find("${" + sn + "}") != std::string::npos) {
                    needsTextBinding = true;
                    break;
                }
            }
        }
    }

    // If no styling needed, return the call directly
    if (styleProps.empty() && widget.events.empty() && !needsTextBinding) {
        return makeCall(resolvedType, std::move(callArgs));
    }

    // Need styling: create temp var, apply styles, return var reference
    std::string tmpVar = nextTempVar();
    setupStmts.push_back(makeVarDecl(tmpVar, makeCall(resolvedType, std::move(callArgs))));

    // Generate style calls
    for (const auto* prop : styleProps) {
        std::string propName = prop->name;
        if (propName == "fontSize") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            // Ensure fontSize is a float
            auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop->value.get());
            if (lit) {
                std::string val = lit->token.lexeme;
                if (val.find('.') == std::string::npos) val += ".0";
                args.push_back(std::make_unique<SLiteralExpr>(val, TokenType::NUMBER));
            } else {
                args.push_back(transpileExpr(*prop->value));
            }
            setupStmts.push_back(makeExprStmt(makeCall("setFontSize", std::move(args))));
        } else if (propName == "color") {
            // Parse color string to RGB
            auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop->value.get());
            std::string colorStr = lit ? lit->token.lexeme : "";
            int r = 255, g = 255, b = 255;
            if (colorStr == "white") { r = 255; g = 255; b = 255; }
            else if (colorStr == "black") { r = 0; g = 0; b = 0; }
            else if (colorStr == "red") { r = 255; g = 0; b = 0; }
            else if (colorStr == "green") { r = 0; g = 255; b = 0; }
            else if (colorStr == "blue") { r = 0; g = 0; b = 255; }
            else if (colorStr == "gray" || colorStr == "grey") { r = 128; g = 128; b = 128; }
            else if (colorStr == "yellow") { r = 255; g = 255; b = 0; }
            else if (colorStr == "cyan") { r = 0; g = 255; b = 255; }
            else if (colorStr == "magenta") { r = 255; g = 0; b = 255; }
            else if (colorStr == "orange") { r = 255; g = 165; b = 0; }
            else if (colorStr == "purple") { r = 128; g = 0; b = 128; }
            else if (colorStr == "darkgray" || colorStr == "darkgrey") { r = 64; g = 64; b = 64; }
            else if (colorStr == "dark") { r = 40; g = 40; b = 50; }
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(makeNumber(r));
            args.push_back(makeNumber(g));
            args.push_back(makeNumber(b));
            setupStmts.push_back(makeExprStmt(makeCall("setTextColor", std::move(args))));
        } else if (propName == "backgroundColor") {
            auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop->value.get());
            std::string colorStr = lit ? lit->token.lexeme : "";
            int r = 30, g = 30, b = 50;
            if (colorStr == "white") { r = 255; g = 255; b = 255; }
            else if (colorStr == "black") { r = 0; g = 0; b = 0; }
            else if (colorStr == "gray" || colorStr == "grey") { r = 128; g = 128; b = 128; }
            else if (colorStr == "darkgray" || colorStr == "darkgrey") { r = 64; g = 64; b = 64; }
            else if (colorStr == "dark") { r = 40; g = 40; b = 50; }
            else if (colorStr == "red") { r = 255; g = 0; b = 0; }
            else if (colorStr == "green") { r = 0; g = 255; b = 0; }
            else if (colorStr == "blue") { r = 0; g = 0; b = 255; }
            else if (colorStr == "purple") { r = 128; g = 0; b = 128; }
            else if (colorStr == "orange") { r = 255; g = 165; b = 0; }
            else if (colorStr == "yellow") { r = 255; g = 255; b = 0; }
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(makeNumber(r));
            args.push_back(makeNumber(g));
            args.push_back(makeNumber(b));
            setupStmts.push_back(makeExprStmt(makeCall("setBackground", std::move(args))));
        } else if (propName == "fontWeight") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileExpr(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setFontWeight", std::move(args))));
        } else if (propName == "fontFamily") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileExpr(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setFontFamily", std::move(args))));
        } else if (propName == "padding") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileAsFloat(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setPadding", std::move(args))));
        } else if (propName == "margin") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileAsFloat(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setMargin", std::move(args))));
        } else if (propName == "borderRadius") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileAsFloat(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setBorderRadius", std::move(args))));
        } else if (propName == "opacity") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileAsFloat(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setOpacity", std::move(args))));
        } else if (propName == "width" || propName == "height") {
            // Need both width and height for setSize — skip individual for now
        } else if (propName == "visible") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileExpr(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setVisible", std::move(args))));
        } else if (propName == "flex") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileAsFloat(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setFlex", std::move(args))));
        } else if (propName == "italic") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileExpr(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setItalic", std::move(args))));
        } else if (propName == "textAlign") {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeVar(tmpVar));
            args.push_back(transpileExpr(*prop->value));
            setupStmts.push_back(makeExprStmt(makeCall("setTextAlign", std::move(args))));
        }
        // Other properties silently ignored for now
    }

    // Generate event handler bindings (signal-aware closures)
    for (const auto& event : widget.events) {
        auto* lam = dynamic_cast<const stui::LambdaExpr*>(event.handler.get());
        if (!lam) continue;

        if (!lam->bodySource.empty()) {
            // Transform state variable assignments in the body:
            //   count += 1  ->  count.set(count.get() + 1)
            //   count -= 1  ->  count.set(count.get() - 1)
            //   count = 0   ->  count.set(0)
            std::string body = lam->bodySource;
            for (const auto& stateName : currentStateNames_) {
                // Handle +=
                std::string plusEq = stateName + " += ";
                size_t pos = 0;
                while ((pos = body.find(plusEq, pos)) != std::string::npos) {
                    size_t exprStart = pos + plusEq.size();
                    size_t semi = body.find(';', exprStart);
                    if (semi == std::string::npos) semi = body.size();
                    std::string expr = body.substr(exprStart, semi - exprStart);
                    std::string replacement = stateName + ".set(" + stateName + ".get() + " + expr + ")";
                    body.replace(pos, semi - pos, replacement);
                    pos += replacement.size();
                }

                // Handle -=
                std::string minusEq = stateName + " -= ";
                pos = 0;
                while ((pos = body.find(minusEq, pos)) != std::string::npos) {
                    size_t exprStart = pos + minusEq.size();
                    size_t semi = body.find(';', exprStart);
                    if (semi == std::string::npos) semi = body.size();
                    std::string expr = body.substr(exprStart, semi - exprStart);
                    std::string replacement = stateName + ".set(" + stateName + ".get() - " + expr + ")";
                    body.replace(pos, semi - pos, replacement);
                    pos += replacement.size();
                }

                // Handle simple = (must come after += and -=)
                std::string eq = stateName + " = ";
                pos = 0;
                while ((pos = body.find(eq, pos)) != std::string::npos) {
                    // Make sure it's not part of += or -= by checking preceding char
                    if (pos > 0 && (body[pos - 1] == '+' || body[pos - 1] == '-')) {
                        pos++;
                        continue;
                    }
                    // Also skip if preceded by . (e.g. count.set)
                    if (pos > 0 && body[pos - 1] == '.') {
                        pos++;
                        continue;
                    }
                    size_t exprStart = pos + eq.size();
                    size_t semi = body.find(';', exprStart);
                    if (semi == std::string::npos) semi = body.size();
                    std::string expr = body.substr(exprStart, semi - exprStart);
                    std::string replacement = stateName + ".set(" + expr + ")";
                    body.replace(pos, semi - pos, replacement);
                    pos += replacement.size();
                }
            }

            // Build the lambda source
            std::string lambdaSource = "fn(";
            for (size_t i = 0; i < lam->params.size(); i++) {
                if (i > 0) lambdaSource += ", ";
                lambdaSource += lam->params[i].lexeme;
            }
            lambdaSource += ") {\n" + body + "\n}";

            // Parse the lambda using Stratos lexer/parser
            try {
                static const std::string stuiEventFile = "<stui-event>";
                stratos::Lexer lexer(lambdaSource, stuiEventFile);
                auto tokens = lexer.scanTokens();
                stratos::Parser parser(tokens);
                auto stmts = parser.parse();

                if (!stmts.empty()) {
                    auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
                    if (exprStmt && exprStmt->expression) {
                        auto* lambdaExpr = dynamic_cast<stratos::LambdaExpr*>(exprStmt->expression.get());
                        if (lambdaExpr) {
                            auto lambdaPtr = std::unique_ptr<Expr>(exprStmt->expression.release());

                            std::string bindFn;
                            if (event.eventName == "onClick") bindFn = "bindClick";
                            else if (event.eventName == "onChange") bindFn = "bindChange";
                            else continue;

                            std::vector<std::unique_ptr<Expr>> bindArgs;
                            bindArgs.push_back(makeVar(tmpVar));
                            bindArgs.push_back(std::move(lambdaPtr));
                            setupStmts.push_back(makeExprStmt(makeCall(bindFn, std::move(bindArgs))));
                        }
                    }
                }

                // Keep parsed stmts alive
                parsedEventStmts_.push_back(std::move(stmts));
            } catch (const std::exception& e) {
                std::cerr << "[STUI] Warning: failed to parse event handler: " << e.what() << std::endl;
            }
        }
    }

    // Generate reactive text bindings for Text widgets with state-dependent interpolated strings
    if (widget.widgetType == "Text" && !widget.arguments.empty()) {
        auto* interp = dynamic_cast<const stui::InterpolatedStringExpr*>(widget.arguments[0].get());
        if (interp) {
            // Check which state variables are referenced in the interpolated string
            std::set<std::string> referencedStates;
            const std::string& raw = interp->raw;
            size_t searchPos = 0;
            while (searchPos < raw.size()) {
                auto dp = raw.find("${", searchPos);
                if (dp == std::string::npos) break;
                auto eb = raw.find('}', dp + 2);
                if (eb == std::string::npos) break;
                std::string exprStr = raw.substr(dp + 2, eb - dp - 2);
                if (currentStateNames_.count(exprStr)) {
                    referencedStates.insert(exprStr);
                }
                searchPos = eb + 1;
            }

            // For each referenced state, generate a bindText call
            // bindText(stateName.id, widgetId, fn() string { return "interpolated string"; })
            for (const auto& stateName : referencedStates) {
                // Build the format lambda that reconstructs the interpolated string
                // The lambda body: return "Count: " + toString(count.get())
                // We'll use an interpolated string with .get() calls
                std::string fmtLambdaSrc = "fn() string {\n    return \"";

                // Rebuild the interpolated string with .get() calls
                size_t fmtPos = 0;
                while (fmtPos < raw.size()) {
                    auto dp = raw.find("${", fmtPos);
                    if (dp == std::string::npos) {
                        fmtLambdaSrc += raw.substr(fmtPos);
                        break;
                    }
                    fmtLambdaSrc += raw.substr(fmtPos, dp - fmtPos);
                    auto eb = raw.find('}', dp + 2);
                    if (eb == std::string::npos) {
                        fmtLambdaSrc += raw.substr(fmtPos);
                        break;
                    }
                    std::string exprStr = raw.substr(dp + 2, eb - dp - 2);
                    if (currentStateNames_.count(exprStr)) {
                        fmtLambdaSrc += "${" + exprStr + ".get()}";
                    } else {
                        fmtLambdaSrc += "${" + exprStr + "}";
                    }
                    fmtPos = eb + 1;
                }
                fmtLambdaSrc += "\";\n}";

                try {
                    static const std::string stuiBindFile = "<stui-bindtext>";
                    stratos::Lexer lexer(fmtLambdaSrc, stuiBindFile);
                    auto tokens = lexer.scanTokens();
                    stratos::Parser parser(tokens);
                    auto stmts = parser.parse();

                    if (!stmts.empty()) {
                        auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
                        if (exprStmt && exprStmt->expression) {
                            auto fmtLambda = std::unique_ptr<Expr>(exprStmt->expression.release());

                            // Generate: bindText(stateName.id, widgetId, fmtLambda)
                            std::vector<std::unique_ptr<Expr>> bindArgs;
                            // stateName.id → member access (dotted identifier)
                            bindArgs.push_back(makeVar(stateName + ".id"));
                            bindArgs.push_back(makeVar(tmpVar));
                            bindArgs.push_back(std::move(fmtLambda));
                            setupStmts.push_back(makeExprStmt(makeCall("bindText", std::move(bindArgs))));
                        }
                    }
                    parsedEventStmts_.push_back(std::move(stmts));
                } catch (const std::exception& e) {
                    std::cerr << "[STUI] Warning: failed to parse bindText lambda: " << e.what() << std::endl;
                }
            }
        }
    }

    return makeVar(tmpVar);
}

// ============================================================================
// Expression Conversion: STUI Expr → Stratos Expr
// ============================================================================

std::unique_ptr<Expr> STUITranspiler::transpileExpr(const STUIExpr& expr) {
    // Literal
    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(&expr)) {
        switch (lit->token.type) {
            case STUITokenType::NUMBER:
                return std::make_unique<SLiteralExpr>(lit->token.lexeme, TokenType::NUMBER);
            case STUITokenType::STRING:
                return std::make_unique<SLiteralExpr>(lit->token.lexeme, TokenType::STRING);
            case STUITokenType::TRUE:
                return std::make_unique<SLiteralExpr>("true", TokenType::TRUE);
            case STUITokenType::FALSE:
                return std::make_unique<SLiteralExpr>("false", TokenType::FALSE);
            case STUITokenType::NONE:
                return std::make_unique<SLiteralExpr>("none", TokenType::NONE);
            default:
                return std::make_unique<SLiteralExpr>(lit->token.lexeme, TokenType::STRING);
        }
    }

    // Identifier
    if (auto* id = dynamic_cast<const stui::IdentifierExpr*>(&expr)) {
        return std::make_unique<SVariableExpr>(makeIdentifier(id->name.lexeme));
    }

    // Member access: obj.member
    if (auto* mem = dynamic_cast<const stui::MemberAccessExpr*>(&expr)) {
        // Flatten to dotted name for simple cases
        // For complex cases, this would need proper member access AST
        if (auto* baseId = dynamic_cast<const stui::IdentifierExpr*>(mem->object.get())) {
            return std::make_unique<SVariableExpr>(
                makeIdentifier(baseId->name.lexeme + "." + mem->member.lexeme));
        }
        // Fallback: treat as variable with dotted name
        return std::make_unique<SVariableExpr>(makeIdentifier(mem->member.lexeme));
    }

    // Function call
    if (auto* call = dynamic_cast<const stui::CallExpr*>(&expr)) {
        auto callee = transpileExpr(*call->callee);
        std::vector<std::unique_ptr<stratos::Expr>> args;
        for (const auto& arg : call->arguments) {
            args.push_back(transpileExpr(*arg));
        }
        return std::make_unique<SCallExpr>(
            std::move(callee),
            makeToken(TokenType::LEFT_PAREN, "("),
            std::move(args));
    }

    // Binary expression
    if (auto* bin = dynamic_cast<const stui::BinaryExpr*>(&expr)) {
        auto left = transpileExpr(*bin->left);
        auto right = transpileExpr(*bin->right);
        // Map STUI operator token to Stratos token
        TokenType opType;
        switch (bin->op.type) {
            case STUITokenType::PLUS: opType = TokenType::PLUS; break;
            case STUITokenType::MINUS: opType = TokenType::MINUS; break;
            case STUITokenType::STAR: opType = TokenType::STAR; break;
            case STUITokenType::SLASH: opType = TokenType::SLASH; break;
            case STUITokenType::PERCENT: opType = TokenType::PERCENT; break;
            case STUITokenType::EQUAL_EQUAL: opType = TokenType::EQUAL_EQUAL; break;
            case STUITokenType::BANG_EQUAL: opType = TokenType::BANG_EQUAL; break;
            case STUITokenType::LESS: opType = TokenType::LESS; break;
            case STUITokenType::LESS_EQUAL: opType = TokenType::LESS_EQUAL; break;
            case STUITokenType::GREATER: opType = TokenType::GREATER; break;
            case STUITokenType::GREATER_EQUAL: opType = TokenType::GREATER_EQUAL; break;
            case STUITokenType::AMPERSAND: opType = TokenType::AND; break;
            case STUITokenType::PIPE_PIPE: opType = TokenType::OR; break;
            default: opType = TokenType::PLUS; break;
        }
        return std::make_unique<SBinaryExpr>(
            std::move(left),
            makeToken(opType, bin->op.lexeme),
            std::move(right));
    }

    // Unary expression
    if (auto* un = dynamic_cast<const stui::UnaryExpr*>(&expr)) {
        auto operand = transpileExpr(*un->operand);
        TokenType opType = (un->op.type == STUITokenType::MINUS) ? TokenType::MINUS : TokenType::BANG;
        return std::make_unique<SUnaryExpr>(
            makeToken(opType, un->op.lexeme),
            std::move(operand));
    }

    // Lambda
    if (auto* lam = dynamic_cast<const stui::LambdaExpr*>(&expr)) {
        std::vector<Token> params;
        for (const auto& p : lam->params) {
            params.push_back(makeIdentifier(p.lexeme));
        }

        // For lambdas with bodySource, we create a block with the raw content
        // passed through. The Stratos interpreter will need to re-lex/parse it.
        // For expression-body lambdas, we transpile the expression.
        std::unique_ptr<Stmt> body;
        if (!lam->bodyExprs.empty()) {
            // Expression body → return statement
            auto retExpr = transpileExpr(*lam->bodyExprs[0]);
            std::vector<std::unique_ptr<Stmt>> stmts;
            stmts.push_back(std::make_unique<ReturnStmt>(
                makeToken(TokenType::IDENTIFIER, "return"),
                std::move(retExpr)));
            body = std::make_unique<BlockStmt>(std::move(stmts));
        } else {
            // Block body — we need to pass the raw source to be re-lexed
            // For now, create an empty block (the interpreter handles raw source via NativeRegistry callbacks)
            std::vector<std::unique_ptr<Stmt>> stmts;
            // If there's body source, create a print statement as placeholder
            // The actual event binding happens through the GUI native callback system
            body = std::make_unique<BlockStmt>(std::move(stmts));
        }

        return std::make_unique<SLambdaExpr>(
            std::move(params), std::move(body), false);
    }

    // Interpolated string — parse into InterpolatedStringExpr parts
    if (auto* interp = dynamic_cast<const stui::InterpolatedStringExpr*>(&expr)) {
        std::vector<InterpolatedPart> parts;
        const std::string& raw = interp->raw;
        size_t pos = 0;
        while (pos < raw.size()) {
            // Look for ${...} interpolation
            auto dollarPos = raw.find("${", pos);
            if (dollarPos == std::string::npos) {
                // Rest is literal text
                parts.emplace_back(raw.substr(pos));
                break;
            }
            // Add literal text before the interpolation
            if (dollarPos > pos) {
                parts.emplace_back(raw.substr(pos, dollarPos - pos));
            }
            // Find matching closing brace
            auto endBrace = raw.find('}', dollarPos + 2);
            if (endBrace == std::string::npos) {
                parts.emplace_back(raw.substr(pos));
                break;
            }
            // Extract expression string and create appropriate expr
            std::string exprStr = raw.substr(dollarPos + 2, endBrace - dollarPos - 2);
            // If this is a state variable, generate stateName.get() call
            if (currentStateNames_.count(exprStr)) {
                auto objExpr = std::make_unique<SVariableExpr>(makeIdentifier(exprStr));
                auto methodExpr = std::make_unique<SVariableExpr>(makeIdentifier("get"));
                Token dotToken;
                dotToken.type = TokenType::DOT;
                dotToken.lexeme = ".";
                auto dotExpr = std::make_unique<SBinaryExpr>(
                    std::move(objExpr), dotToken, std::move(methodExpr));
                Token paren;
                paren.type = TokenType::LEFT_PAREN;
                paren.lexeme = "(";
                std::vector<std::unique_ptr<Expr>> noArgs;
                auto callExpr = std::make_unique<SCallExpr>(
                    std::move(dotExpr), paren, std::move(noArgs));
                parts.emplace_back(std::move(callExpr));
            } else {
                parts.emplace_back(
                    std::make_unique<SVariableExpr>(makeIdentifier(exprStr)));
            }
            pos = endBrace + 1;
        }
        return std::make_unique<stratos::InterpolatedStringExpr>(std::move(parts));
    }

    // Array literal
    if (auto* arr = dynamic_cast<const stui::ArrayExpr*>(&expr)) {
        std::vector<std::unique_ptr<stratos::Expr>> elements;
        for (const auto& el : arr->elements) {
            elements.push_back(transpileExpr(*el));
        }
        return std::make_unique<ArrayLiteralExpr>(std::move(elements));
    }

    // Map literal
    if (auto* map = dynamic_cast<const stui::MapExpr*>(&expr)) {
        std::vector<std::pair<std::string, std::unique_ptr<stratos::Expr>>> entries;
        for (const auto& entry : map->entries) {
            entries.push_back({entry.first, transpileExpr(*entry.second)});
        }
        return std::make_unique<MapLiteralExpr>(std::move(entries));
    }

    // Assignment
    if (auto* assign = dynamic_cast<const stui::AssignExpr*>(&expr)) {
        // For state assignments like count += 1, we transform to:
        // count.set(count.get() + 1)
        // But for simple cases, just emit as binary with assignment
        auto target = transpileExpr(*assign->target);
        auto value = transpileExpr(*assign->value);

        // Check if it's a compound assignment
        if (assign->op.type == STUITokenType::PLUS_EQUAL ||
            assign->op.type == STUITokenType::MINUS_EQUAL) {
            // target += value  →  target.set(target.get() + value)
            // This is a simplification; full state binding would need more work
            TokenType binOp = (assign->op.type == STUITokenType::PLUS_EQUAL) ?
                              TokenType::PLUS : TokenType::MINUS;
            auto getCall = transpileExpr(*assign->target);
            auto binExpr = std::make_unique<SBinaryExpr>(
                std::move(getCall),
                makeToken(binOp, assign->op.lexeme.substr(0, 1)),
                std::move(value));
            return std::make_unique<SBinaryExpr>(
                std::move(target),
                makeToken(TokenType::EQUAL, "="),
                std::move(binExpr));
        }

        return std::make_unique<SBinaryExpr>(
            std::move(target),
            makeToken(TokenType::EQUAL, "="),
            std::move(value));
    }

    // Fallback: return a "none" literal
    return std::make_unique<SLiteralExpr>("none", TokenType::NONE);
}

// ============================================================================
// Generate main() function
// ============================================================================

std::unique_ptr<Stmt> STUITranspiler::generateMain(const STUIFile& file) {
    // Find the App component to extract window config
    const ComponentDecl* appComp = nullptr;
    for (const auto& comp : file.components) {
        if (comp.name == "App") {
            appComp = &comp;
            break;
        }
    }

    std::vector<std::unique_ptr<Stmt>> mainBody;

    if (appComp && appComp->viewRoot) {
        // Look for a Window widget in the App's view
        const WidgetNode* windowWidget = nullptr;
        if (appComp->viewRoot->widgetType == "Window") {
            windowWidget = appComp->viewRoot.get();
        }

        // Extract window properties
        std::string title = "Stratos App";
        int width = 800, height = 600;

        if (windowWidget) {
            for (const auto& prop : windowWidget->properties) {
                if (prop.name == "title") {
                    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop.value.get())) {
                        title = lit->token.lexeme;
                    }
                } else if (prop.name == "width") {
                    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop.value.get())) {
                        width = std::stoi(lit->token.lexeme);
                    }
                } else if (prop.name == "height") {
                    if (auto* lit = dynamic_cast<const stui::LiteralExpr*>(prop.value.get())) {
                        height = std::stoi(lit->token.lexeme);
                    }
                }
            }
        }

        // val app = gui.App("Title", width, height);
        {
            std::vector<std::unique_ptr<Expr>> args;
            args.push_back(makeString(title, true));
            args.push_back(makeNumber(width));
            args.push_back(makeNumber(height));
            mainBody.push_back(makeVarDecl("app", makeCall("App", std::move(args))));
        }

        // Build the content widget tree (children of Window, or the App's view root)
        const WidgetNode* contentRoot = windowWidget;
        if (windowWidget && !windowWidget->children.empty()) {
            // If Window has children, wrap them
            if (windowWidget->children.size() == 1) {
                std::vector<std::unique_ptr<Stmt>> widgetSetup;
                auto widgetExpr = transpileWidget(*windowWidget->children[0], widgetSetup);
                for (auto& s : widgetSetup) mainBody.push_back(std::move(s));
                std::vector<std::unique_ptr<Expr>> rootArgs;
                rootArgs.push_back(std::move(widgetExpr));
                mainBody.push_back(makeExprStmt(
                    makeMemberCall("app", "root", std::move(rootArgs))));
            } else {
                // Multiple children → wrap in Column
                std::vector<std::unique_ptr<Stmt>> widgetSetup;
                std::vector<std::unique_ptr<Expr>> childExprs;
                for (const auto& child : windowWidget->children) {
                    childExprs.push_back(transpileWidget(*child, widgetSetup));
                }
                for (auto& s : widgetSetup) mainBody.push_back(std::move(s));
                std::vector<std::unique_ptr<Expr>> colArgs;
                colArgs.push_back(makeArrayLiteral(std::move(childExprs)));
                auto columnExpr = makeCall("Column", std::move(colArgs));

                std::vector<std::unique_ptr<Expr>> rootArgs;
                rootArgs.push_back(std::move(columnExpr));
                mainBody.push_back(makeExprStmt(
                    makeMemberCall("app", "root", std::move(rootArgs))));
            }
        } else if (!windowWidget && appComp->viewRoot) {
            // No Window widget, use the view root directly
            std::vector<std::unique_ptr<Stmt>> widgetSetup;
            auto widgetExpr = transpileWidget(*appComp->viewRoot, widgetSetup);
            for (auto& s : widgetSetup) mainBody.push_back(std::move(s));
            std::vector<std::unique_ptr<Expr>> rootArgs;
            rootArgs.push_back(std::move(widgetExpr));
            mainBody.push_back(makeExprStmt(
                makeMemberCall("app", "root", std::move(rootArgs))));
        }

        // For non-App components referenced in the tree, call their functions
        // This is already handled since component names become function calls
        // in transpileWidget (e.g., Counter() calls the Counter function)

        // app.run();
        {
            std::vector<std::unique_ptr<Expr>> runArgs;
            mainBody.push_back(makeExprStmt(
                makeMemberCall("app", "run", std::move(runArgs))));
        }
    }

    auto body = std::make_unique<std::vector<std::unique_ptr<Stmt>>>();
    for (auto& stmt : mainBody) {
        body->push_back(std::move(stmt));
    }

    return std::make_unique<FunctionDecl>(
        makeIdentifier("main"),
        std::vector<Parameter>{},
        "void",
        std::move(body),
        true,   // isPublic
        false   // isAsync
    );
}

} // namespace stui
} // namespace stratos
