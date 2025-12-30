#ifndef STRATOS_SYMBOL_H
#define STRATOS_SYMBOL_H

#include <string>
#include <vector>

namespace stratos {

enum class SymbolKind {
    VARIABLE,
    FUNCTION,
    CLASS,
    INTERFACE,
    PACKAGE
};

struct Symbol {
    std::string name;
    SymbolKind kind;
    std::string type; // Simple string representation for now (e.g., "int", "fn(int,int)->void")
    bool isMutable;   // For variables (val vs var)

    // For functions
    std::vector<std::string> paramTypes;
    std::string returnType;

    // Helper constructors
    static Symbol Variable(std::string name, std::string type, bool isMutable) {
        Symbol s;
        s.name = name;
        s.kind = SymbolKind::VARIABLE;
        s.type = type;
        s.isMutable = isMutable;
        return s;
    }

    static Symbol Function(std::string name, std::vector<std::string> params, std::string ret) {
        Symbol s;
        s.name = name;
        s.kind = SymbolKind::FUNCTION;
        s.paramTypes = params;
        s.returnType = ret;
        // Construct a type signature string
        s.type = "fn(";
        for (size_t i = 0; i < params.size(); ++i) {
            s.type += params[i];
            if (i < params.size() - 1) s.type += ",";
        }
        s.type += ")->" + ret;
        return s;
    }
};

} // namespace stratos

#endif // STRATOS_SYMBOL_H
