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
    bool isPublic;    // For visibility checks
    bool isAsync = false; // For async functions/lambdas

    // Location info
    int line = 0;
    int column = 0;
    std::string file = "";

    // For functions
    std::vector<std::string> paramTypes;
    std::string returnType;

    // Helper constructors
    static Symbol Variable(std::string name, std::string type, bool isMutable, bool isPublic = false, int line = 0, int column = 0, std::string file = "") {
        Symbol s;
        s.name = name;
        s.kind = SymbolKind::VARIABLE;
        s.type = type;
        s.isMutable = isMutable;
        s.isPublic = isPublic;
        s.line = line;
        s.column = column;
        s.file = file;
        return s;
    }

    static Symbol Function(std::string name, std::vector<std::string> params, std::string ret, bool isPublic = false, bool isAsync = false, int line = 0, int column = 0, std::string file = "") {
        Symbol s;
        s.name = name;
        s.kind = SymbolKind::FUNCTION;
        s.paramTypes = params;
        s.returnType = ret;
        s.isPublic = isPublic;
        s.isAsync = isAsync;
        s.line = line;
        s.column = column;
        s.file = file;
        // Construct a type signature string
        s.type = "fn(";
        for (size_t i = 0; i < params.size(); ++i) {
            s.type += params[i];
            if (i < params.size() - 1) s.type += ",";
        }
        s.type += ")->" + ret;
        return s;
    }

    static Symbol Class(std::string name, std::string type, bool isPublic = false, int line = 0, int column = 0, std::string file = "") {
        Symbol s;
        s.name = name;
        s.kind = SymbolKind::CLASS;
        s.type = type;
        s.isPublic = isPublic;
        s.line = line;
        s.column = column;
        s.file = file;
        s.isMutable = false; // Classes are not mutable variables
        return s;
    }
};

} // namespace stratos

#endif // STRATOS_SYMBOL_H
