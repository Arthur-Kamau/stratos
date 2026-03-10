#ifndef STRATOS_STUI_AST_H
#define STRATOS_STUI_AST_H

#include "STUIToken.h"
#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace stratos {
namespace stui {

// Forward declarations
struct STUIExpr;
struct WidgetNode;
struct ComponentDecl;
struct StateDecl;
struct PropDecl;
struct StyleBlock;
struct EventHandler;

// ============================================================================
// EXPRESSIONS (reused for property values, conditions, etc.)
// ============================================================================

struct STUIExpr {
    virtual ~STUIExpr() = default;
};

struct LiteralExpr : STUIExpr {
    STUIToken token;
    LiteralExpr(STUIToken token) : token(std::move(token)) {}
};

struct IdentifierExpr : STUIExpr {
    STUIToken name;
    IdentifierExpr(STUIToken name) : name(std::move(name)) {}
};

struct MemberAccessExpr : STUIExpr {
    std::unique_ptr<STUIExpr> object;
    STUIToken member;
    MemberAccessExpr(std::unique_ptr<STUIExpr> object, STUIToken member)
        : object(std::move(object)), member(std::move(member)) {}
};

struct CallExpr : STUIExpr {
    std::unique_ptr<STUIExpr> callee;
    std::vector<std::unique_ptr<STUIExpr>> arguments;
    CallExpr(std::unique_ptr<STUIExpr> callee, std::vector<std::unique_ptr<STUIExpr>> args)
        : callee(std::move(callee)), arguments(std::move(args)) {}
};

struct BinaryExpr : STUIExpr {
    std::unique_ptr<STUIExpr> left;
    STUIToken op;
    std::unique_ptr<STUIExpr> right;
    BinaryExpr(std::unique_ptr<STUIExpr> left, STUIToken op, std::unique_ptr<STUIExpr> right)
        : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}
};

struct UnaryExpr : STUIExpr {
    STUIToken op;
    std::unique_ptr<STUIExpr> operand;
    UnaryExpr(STUIToken op, std::unique_ptr<STUIExpr> operand)
        : op(std::move(op)), operand(std::move(operand)) {}
};

struct LambdaExpr : STUIExpr {
    std::vector<STUIToken> params;
    std::vector<std::unique_ptr<STUIExpr>> bodyExprs; // simplified: sequence of expressions
    std::string bodySource; // raw source for pass-through to Stratos
    LambdaExpr() = default;
};

struct InterpolatedStringExpr : STUIExpr {
    std::string raw; // The full interpolated string content
    InterpolatedStringExpr(std::string raw) : raw(std::move(raw)) {}
};

struct ArrayExpr : STUIExpr {
    std::vector<std::unique_ptr<STUIExpr>> elements;
    ArrayExpr(std::vector<std::unique_ptr<STUIExpr>> elements)
        : elements(std::move(elements)) {}
};

struct MapExpr : STUIExpr {
    std::vector<std::pair<std::string, std::unique_ptr<STUIExpr>>> entries;
    MapExpr(std::vector<std::pair<std::string, std::unique_ptr<STUIExpr>>> entries)
        : entries(std::move(entries)) {}
};

struct AssignExpr : STUIExpr {
    std::unique_ptr<STUIExpr> target;
    STUIToken op; // =, +=, -=, etc.
    std::unique_ptr<STUIExpr> value;
    AssignExpr(std::unique_ptr<STUIExpr> target, STUIToken op, std::unique_ptr<STUIExpr> value)
        : target(std::move(target)), op(std::move(op)), value(std::move(value)) {}
};

// ============================================================================
// STUI-SPECIFIC NODES
// ============================================================================

// Property: key: value inside a widget or style block
struct Property {
    std::string name;
    std::unique_ptr<STUIExpr> value;
    int line;

    Property(std::string name, std::unique_ptr<STUIExpr> value, int line)
        : name(std::move(name)), value(std::move(value)), line(line) {}
};

// Event handler: onClick: { ... } or onSubmit: fn(e) { ... }
struct EventHandler {
    std::string eventName; // "onClick", "onChange", etc.
    std::unique_ptr<STUIExpr> handler; // Lambda or identifier
    int line;

    EventHandler(std::string eventName, std::unique_ptr<STUIExpr> handler, int line)
        : eventName(std::move(eventName)), handler(std::move(handler)), line(line) {}
};

// Widget instantiation in a view block
// e.g., Text("Hello") { fontSize: 24; color: "red"; }
//   or  Column(spacing: 8) { Text("a"); Text("b"); }
struct WidgetNode {
    std::string widgetType; // "Text", "Button", "Column", etc.
    std::vector<std::unique_ptr<STUIExpr>> arguments; // positional args
    std::vector<Property> properties; // named properties in { }
    std::vector<EventHandler> events; // event handlers in { }
    std::vector<std::unique_ptr<WidgetNode>> children; // child widgets in { }
    int line;

    WidgetNode(std::string widgetType, int line)
        : widgetType(std::move(widgetType)), line(line) {}
};

// Conditional widget: if/else inside view
struct ConditionalWidget {
    std::unique_ptr<STUIExpr> condition;
    std::vector<std::unique_ptr<WidgetNode>> thenWidgets;
    std::vector<std::unique_ptr<WidgetNode>> elseWidgets;
    int line;
};

// For-loop widget: for item in list { Widget(item) }
struct ForLoopWidget {
    std::string variableName;
    std::unique_ptr<STUIExpr> iterable;
    std::vector<std::unique_ptr<WidgetNode>> bodyWidgets;
    int line;
};

// State declaration inside a component
// state count: int = 0;
struct StateDecl {
    std::string name;
    std::string typeName;
    std::unique_ptr<STUIExpr> initialValue;
    int line;

    StateDecl(std::string name, std::string typeName,
              std::unique_ptr<STUIExpr> initialValue, int line)
        : name(std::move(name)), typeName(std::move(typeName)),
          initialValue(std::move(initialValue)), line(line) {}
};

// Prop declaration (input to component)
// prop title: string;
// prop count: int = 0;
struct PropDecl {
    std::string name;
    std::string typeName;
    std::unique_ptr<STUIExpr> defaultValue; // optional
    int line;

    PropDecl(std::string name, std::string typeName,
             std::unique_ptr<STUIExpr> defaultValue, int line)
        : name(std::move(name)), typeName(std::move(typeName)),
          defaultValue(std::move(defaultValue)), line(line) {}
};

// Component declaration
// component Counter { state ...; prop ...; view { ... } }
struct ComponentDecl {
    std::string name;
    std::vector<StateDecl> states;
    std::vector<PropDecl> props;
    std::unique_ptr<WidgetNode> viewRoot; // The root widget of the view block
    std::vector<std::pair<std::string, std::string>> functions; // helper fns (name -> source)
    int line;

    ComponentDecl(std::string name, int line)
        : name(std::move(name)), line(line) {}
};

// Top-level STUI file
struct STUIFile {
    std::string packageName;
    std::vector<std::string> imports; // import paths
    std::vector<ComponentDecl> components;
};

} // namespace stui
} // namespace stratos

#endif // STRATOS_STUI_AST_H
