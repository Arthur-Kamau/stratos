#include "stratos/STUIFormatter.h"
#include <iostream>

namespace stratos {
namespace stui {

void STUIFormatter::indent() {
    for (int i = 0; i < indentLevel_; i++) out_ << "    ";
}

void STUIFormatter::newline() { out_ << "\n"; }
void STUIFormatter::write(const std::string& text) { out_ << text; }

void STUIFormatter::formatExpr(const STUIExpr& expr) {
    if (auto* lit = dynamic_cast<const LiteralExpr*>(&expr)) {
        if (lit->token.type == STUITokenType::STRING) {
            write("\"");
            write(lit->token.lexeme);
            write("\"");
        } else {
            write(lit->token.lexeme);
        }
    } else if (auto* id = dynamic_cast<const IdentifierExpr*>(&expr)) {
        write(id->name.lexeme);
    } else if (auto* mem = dynamic_cast<const MemberAccessExpr*>(&expr)) {
        formatExpr(*mem->object);
        write(".");
        write(mem->member.lexeme);
    } else if (auto* call = dynamic_cast<const CallExpr*>(&expr)) {
        formatExpr(*call->callee);
        write("(");
        for (size_t i = 0; i < call->arguments.size(); i++) {
            if (i > 0) write(", ");
            formatExpr(*call->arguments[i]);
        }
        write(")");
    } else if (auto* bin = dynamic_cast<const BinaryExpr*>(&expr)) {
        formatExpr(*bin->left);
        write(" ");
        write(bin->op.lexeme);
        write(" ");
        formatExpr(*bin->right);
    } else if (auto* un = dynamic_cast<const UnaryExpr*>(&expr)) {
        write(un->op.lexeme);
        formatExpr(*un->operand);
    } else if (auto* lam = dynamic_cast<const LambdaExpr*>(&expr)) {
        if (!lam->bodySource.empty()) {
            write(lam->bodySource);
        } else {
            write("{ ... }");
        }
    } else if (auto* interp = dynamic_cast<const InterpolatedStringExpr*>(&expr)) {
        write("\"");
        write(interp->raw);
        write("\"");
    } else if (auto* arr = dynamic_cast<const ArrayExpr*>(&expr)) {
        write("[");
        for (size_t i = 0; i < arr->elements.size(); i++) {
            if (i > 0) write(", ");
            formatExpr(*arr->elements[i]);
        }
        write("]");
    } else if (auto* mp = dynamic_cast<const MapExpr*>(&expr)) {
        write("{");
        for (size_t i = 0; i < mp->entries.size(); i++) {
            if (i > 0) write(", ");
            write(mp->entries[i].first);
            write(": ");
            formatExpr(*mp->entries[i].second);
        }
        write("}");
    } else if (auto* assign = dynamic_cast<const AssignExpr*>(&expr)) {
        formatExpr(*assign->target);
        write(" ");
        write(assign->op.lexeme);
        write(" ");
        formatExpr(*assign->value);
    }
}

void STUIFormatter::formatProperty(const Property& prop) {
    indent();
    write(prop.name);
    write(": ");
    formatExpr(*prop.value);
    write(";");
    newline();
}

void STUIFormatter::formatEvent(const EventHandler& event) {
    indent();
    write(event.eventName);
    write(": ");
    formatExpr(*event.handler);
    write(";");
    newline();
}

void STUIFormatter::formatWidget(const WidgetNode& widget) {
    indent();
    write(widget.widgetType);

    // Arguments
    if (!widget.arguments.empty()) {
        write("(");
        for (size_t i = 0; i < widget.arguments.size(); i++) {
            if (i > 0) write(", ");
            formatExpr(*widget.arguments[i]);
        }
        write(")");
    }

    // Body: properties, events, children
    bool hasBody = !widget.properties.empty() || !widget.events.empty() || !widget.children.empty();
    if (hasBody) {
        write(" {");
        newline();
        indentLevel_++;

        for (const auto& prop : widget.properties) {
            formatProperty(prop);
        }
        for (const auto& event : widget.events) {
            formatEvent(event);
        }
        for (const auto& child : widget.children) {
            formatWidget(*child);
        }

        indentLevel_--;
        indent();
        write("}");
    }
    newline();
}

void STUIFormatter::formatState(const StateDecl& state) {
    indent();
    write("state ");
    write(state.name);
    write(": ");
    write(state.typeName);
    if (state.initialValue) {
        write(" = ");
        formatExpr(*state.initialValue);
    }
    write(";");
    newline();
}

void STUIFormatter::formatComputed(const ComputedDecl& comp) {
    indent();
    write("computed ");
    write(comp.name);
    write(": ");
    write(comp.typeName);
    if (comp.expression) {
        write(" = ");
        formatExpr(*comp.expression);
    }
    write(";");
    newline();
}

void STUIFormatter::formatProp(const PropDecl& prop) {
    indent();
    write("prop ");
    write(prop.name);
    write(": ");
    write(prop.typeName);
    if (prop.defaultValue) {
        write(" = ");
        formatExpr(*prop.defaultValue);
    }
    write(";");
    newline();
}

void STUIFormatter::formatComponent(const ComponentDecl& comp) {
    write("component ");
    write(comp.name);
    write(" {");
    newline();
    indentLevel_++;

    // Props
    for (const auto& prop : comp.props) {
        formatProp(prop);
    }
    if (!comp.props.empty() && (!comp.states.empty() || !comp.computeds.empty())) {
        newline();
    }

    // States
    for (const auto& state : comp.states) {
        formatState(state);
    }

    // Computeds
    for (const auto& computed : comp.computeds) {
        formatComputed(computed);
    }
    if ((!comp.states.empty() || !comp.computeds.empty()) && comp.viewRoot) {
        newline();
    }

    // Functions
    for (const auto& [name, source] : comp.functions) {
        indent();
        write(source);
        newline();
        newline();
    }

    // View
    if (comp.viewRoot) {
        indent();
        write("view {");
        newline();
        indentLevel_++;
        formatWidget(*comp.viewRoot);
        indentLevel_--;
        indent();
        write("}");
        newline();
    }

    indentLevel_--;
    write("}");
    newline();
}

std::string STUIFormatter::format(const STUIFile& file) {
    out_.str("");
    out_.clear();
    indentLevel_ = 0;

    // Package
    if (!file.packageName.empty()) {
        write("package ");
        write(file.packageName);
        write(";");
        newline();
        newline();
    }

    // Imports
    for (const auto& imp : file.imports) {
        write("import \"");
        write(imp);
        write("\";");
        newline();
    }
    if (!file.imports.empty()) {
        newline();
    }

    // Components
    for (size_t i = 0; i < file.components.size(); i++) {
        if (i > 0) newline();
        formatComponent(file.components[i]);
    }

    return out_.str();
}

} // namespace stui
} // namespace stratos
