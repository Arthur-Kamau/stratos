#pragma once

#include "stratos/STUIAST.h"
#include <string>
#include <sstream>

namespace stratos {
namespace stui {

/// Formats STUI AST back into well-indented .stui source code.
class STUIFormatter {
public:
    STUIFormatter() = default;

    std::string format(const STUIFile& file);

private:
    void formatComponent(const ComponentDecl& comp);
    void formatState(const StateDecl& state);
    void formatComputed(const ComputedDecl& comp);
    void formatProp(const PropDecl& prop);
    void formatWidget(const WidgetNode& widget);
    void formatProperty(const Property& prop);
    void formatEvent(const EventHandler& event);
    void formatExpr(const STUIExpr& expr);

    void indent();
    void newline();
    void write(const std::string& text);

    std::ostringstream out_;
    int indentLevel_ = 0;
};

} // namespace stui
} // namespace stratos
