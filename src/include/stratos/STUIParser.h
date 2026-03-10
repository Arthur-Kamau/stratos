#ifndef STRATOS_STUI_PARSER_H
#define STRATOS_STUI_PARSER_H

#include "STUIToken.h"
#include "STUIAST.h"
#include <vector>
#include <string>
#include <stdexcept>

namespace stratos {
namespace stui {

class STUIParseError : public std::runtime_error {
public:
    int line, column;
    STUIParseError(const std::string& message, int line, int column)
        : std::runtime_error(message), line(line), column(column) {}
};

class STUIParser {
public:
    STUIParser(const std::vector<STUIToken>& tokens, const std::string& sourceFile);
    STUIFile parse();

private:
    const std::vector<STUIToken>& tokens_;
    const std::string& sourceFile_;
    int current_ = 0;

    // Token navigation
    bool isAtEnd() const;
    const STUIToken& peek() const;
    const STUIToken& previous() const;
    const STUIToken& advance();
    bool check(STUITokenType type) const;
    bool match(STUITokenType type);
    bool match(std::initializer_list<STUITokenType> types);
    const STUIToken& consume(STUITokenType type, const std::string& message);

    // Error handling
    STUIParseError error(const STUIToken& token, const std::string& message);

    // Top-level parsing
    void parseImport(STUIFile& file);
    void parsePackage(STUIFile& file);
    ComponentDecl parseComponent();

    // Component members
    StateDecl parseStateDecl();
    PropDecl parsePropDecl();
    std::unique_ptr<WidgetNode> parseViewBlock();

    // Widget tree
    std::unique_ptr<WidgetNode> parseWidget();
    void parseWidgetBody(WidgetNode& widget);
    Property parseProperty();
    EventHandler parseEventHandler(const std::string& name, int line);

    // Expressions
    std::unique_ptr<STUIExpr> parseExpression();
    std::unique_ptr<STUIExpr> parseAssignment();
    std::unique_ptr<STUIExpr> parseOr();
    std::unique_ptr<STUIExpr> parseAnd();
    std::unique_ptr<STUIExpr> parseEquality();
    std::unique_ptr<STUIExpr> parseComparison();
    std::unique_ptr<STUIExpr> parseTerm();
    std::unique_ptr<STUIExpr> parseFactor();
    std::unique_ptr<STUIExpr> parseUnary();
    std::unique_ptr<STUIExpr> parseCall();
    std::unique_ptr<STUIExpr> parsePrimary();

    // Helpers
    std::string parseTypeName();
    bool isWidgetName() const;
    bool isEventName(const std::string& name) const;
};

} // namespace stui
} // namespace stratos

#endif // STRATOS_STUI_PARSER_H
