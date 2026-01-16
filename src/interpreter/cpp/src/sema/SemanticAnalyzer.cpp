#include "stratos/SemanticAnalyzer.h"
#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/NativeRegistry.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace stratos {

SemanticAnalyzer::SemanticAnalyzer(std::string root) : projectRoot(root) {
    defineNativeFunctions();
}

void SemanticAnalyzer::defineNativeFunctions() {
    // Don't pre-define 'print' or 'println' - they're defined in std/io/console.st
    // symbolTable.define(Symbol::Function("print", {"any"}, "void"));
    // symbolTable.define(Symbol::Function("println", {"any"}, "void"));

    // Define standard types/constructors
    symbolTable.define(Symbol::Variable("Some", "constructor", false, true)); // Mock for now
    symbolTable.define(Symbol::Variable("None", "Optional", false, true));
    symbolTable.define(Symbol::Function("Array", {}, "any", true)); // Generic array constructor

    // Internal intrinsics
    symbolTable.define(Symbol::Function("__if_expr", {"bool", "any", "any"}, "any", true));

    // Register Native/Built-in Types and their Methods
    
    // --- Array ---
    auto& arrayMethods = classMembers["Array"];
    // Note: We use "any" for generic return types for now
    arrayMethods["length"] = Symbol::Function("length", {}, "int");
    arrayMethods["add"] = Symbol::Function("add", {"any"}, "void");
    arrayMethods["isEmpty"] = Symbol::Function("isEmpty", {}, "bool");
    arrayMethods["first"] = Symbol::Function("first", {}, "any");
    arrayMethods["last"] = Symbol::Function("last", {}, "any");
    arrayMethods["join"] = Symbol::Function("join", {"string"}, "string");
    arrayMethods["contains"] = Symbol::Function("contains", {"any"}, "bool");
    arrayMethods["indexOf"] = Symbol::Function("indexOf", {"any"}, "int");
    arrayMethods["reverse"] = Symbol::Function("reverse", {}, "Array");
    arrayMethods["clear"] = Symbol::Function("clear", {}, "Array");
    
    // Alias "array" (lowercase) to "Array"
    classMembers["array"] = arrayMethods;

    // --- Map ---
    // Note: maps.create() returns "map<...>", so we register "map"
    auto& mapMethods = classMembers["map"];
    mapMethods["set"] = Symbol::Function("set", {"any", "any"}, "map");
    mapMethods["put"] = Symbol::Function("put", {"any", "any"}, "map"); // Alias for set
    mapMethods["get"] = Symbol::Function("get", {"any"}, "any");
    mapMethods["remove"] = Symbol::Function("remove", {"any"}, "map");
    mapMethods["has"] = Symbol::Function("has", {"any"}, "bool");
    mapMethods["size"] = Symbol::Function("size", {}, "int");
    mapMethods["length"] = Symbol::Function("length", {}, "int"); // Alias for size
    mapMethods["isEmpty"] = Symbol::Function("isEmpty", {}, "bool");
    mapMethods["keys"] = Symbol::Function("keys", {}, "Array");
    mapMethods["values"] = Symbol::Function("values", {}, "Array");
    mapMethods["clear"] = Symbol::Function("clear", {}, "map");
    mapMethods["first"] = Symbol::Function("first", {}, "any");
    mapMethods["last"] = Symbol::Function("last", {}, "any");
    mapMethods["firstValue"] = Symbol::Function("firstValue", {}, "any");
    mapMethods["lastValue"] = Symbol::Function("lastValue", {}, "any");
    mapMethods["merge"] = Symbol::Function("merge", {"map"}, "map");
    mapMethods["entries"] = Symbol::Function("entries", {}, "Array");
    mapMethods["containsValue"] = Symbol::Function("containsValue", {"any"}, "bool");
    mapMethods["fromEntries"] = Symbol::Function("fromEntries", {"Array"}, "map");

    // Alias "Map" (capitalized) to "map"
    classMembers["Map"] = mapMethods;

    // --- String ---
    auto& stringMethods = classMembers["string"];
    stringMethods["length"] = Symbol::Function("length", {}, "int");
    stringMethods["split"] = Symbol::Function("split", {"string"}, "Array");
    stringMethods["repeat"] = Symbol::Function("repeat", {"int"}, "string");
    stringMethods["trim"] = Symbol::Function("trim", {}, "string");
    stringMethods["toUpper"] = Symbol::Function("toUpper", {}, "string");
    stringMethods["toLower"] = Symbol::Function("toLower", {}, "string");
    stringMethods["startsWith"] = Symbol::Function("startsWith", {"string"}, "bool");
    stringMethods["endsWith"] = Symbol::Function("endsWith", {"string"}, "bool");
    stringMethods["replace"] = Symbol::Function("replace", {"string", "string"}, "string");

    // --- IO Module ---
    auto& fileMethods = classMembers["File"];
    fileMethods["readAll"] = Symbol::Function("readAll", {}, "string");
    fileMethods["readLine"] = Symbol::Function("readLine", {}, "string");
    fileMethods["write"] = Symbol::Function("write", {"string"}, "Result"); // Result<int, Error>
    fileMethods["close"] = Symbol::Function("close", {}, "void");
    // Fields
    fileMethods["path"] = Symbol::Variable("path", "string", false);
    fileMethods["handle"] = Symbol::Variable("handle", "int", false);

    auto& resultMethods = classMembers["Result"];
    resultMethods["ok"] = Symbol::Function("ok", {}, "bool");
    resultMethods["err"] = Symbol::Function("err", {}, "any"); // Generic E
    resultMethods["unwrap"] = Symbol::Function("unwrap", {}, "any"); // Generic T
    resultMethods["isOk"] = Symbol::Variable("isOk", "bool", false);
    resultMethods["value"] = Symbol::Variable("value", "any", false);
    resultMethods["error"] = Symbol::Variable("error", "any", false);

    auto& fileInfoMethods = classMembers["FileInfo"];
    fileInfoMethods["getName"] = Symbol::Function("getName", {}, "string", true);
    fileInfoMethods["getSize"] = Symbol::Function("getSize", {}, "int", true);
    fileInfoMethods["getIsDir"] = Symbol::Function("getIsDir", {}, "bool", true);
    fileInfoMethods["getModTime"] = Symbol::Function("getModTime", {}, "int", true);
    fileInfoMethods["fileName"] = Symbol::Variable("fileName", "string", false);
    fileInfoMethods["size"] = Symbol::Variable("size", "int", false);
    fileInfoMethods["isDir"] = Symbol::Variable("isDir", "bool", false);
    fileInfoMethods["modTime"] = Symbol::Variable("modTime", "int", false);

    auto& errorMethods = classMembers["Error"];
    errorMethods["message"] = Symbol::Variable("message", "string", false);

    // --- Net Module ---
    auto& connMethods = classMembers["Conn"];
    connMethods["read"] = Symbol::Function("read", {"Array"}, "int");
    connMethods["write"] = Symbol::Function("write", {"Array"}, "int");
    connMethods["readString"] = Symbol::Function("readString", {"int"}, "string");
    connMethods["writeString"] = Symbol::Function("writeString", {"string"}, "int");
    connMethods["close"] = Symbol::Function("close", {}, "void");
    connMethods["setTimeout"] = Symbol::Function("setTimeout", {"int"}, "void");

    auto& listenerMethods = classMembers["Listener"];
    listenerMethods["accept"] = Symbol::Function("accept", {}, "Result");
    listenerMethods["close"] = Symbol::Function("close", {}, "void");

    auto& udpConnMethods = classMembers["UDPConn"];
    udpConnMethods["readFrom"] = Symbol::Function("readFrom", {"Array"}, "Pair");
    udpConnMethods["writeTo"] = Symbol::Function("writeTo", {"Array", "string", "int"}, "int");
    udpConnMethods["close"] = Symbol::Function("close", {}, "void");

    auto& ipAddrMethods = classMembers["IPAddr"];
    ipAddrMethods["toString"] = Symbol::Function("toString", {}, "string");
    ipAddrMethods["isIPv4"] = Symbol::Function("isIPv4", {}, "bool");
    ipAddrMethods["isIPv6"] = Symbol::Function("isIPv6", {}, "bool");

    auto& tcpServerMethods = classMembers["TCPServer"];
    tcpServerMethods["start"] = Symbol::Function("start", {}, "void");
    tcpServerMethods["stop"] = Symbol::Function("stop", {}, "void");

    auto& udpServerMethods = classMembers["UDPServer"];
    udpServerMethods["start"] = Symbol::Function("start", {}, "void");
    udpServerMethods["stop"] = Symbol::Function("stop", {}, "void");

    // --- Regex Module ---
    auto& matchMethods = classMembers["Match"];
    matchMethods["text"] = Symbol::Variable("text", "string", false);
    matchMethods["start"] = Symbol::Variable("start", "int", false);
    matchMethods["end"] = Symbol::Variable("end", "int", false);
    matchMethods["groups"] = Symbol::Variable("groups", "Array", false);

    auto& matchInfoMethods = classMembers["MatchInfo"];
    matchInfoMethods["text"] = Symbol::Variable("text", "string", false);
    matchInfoMethods["start"] = Symbol::Variable("start", "int", false);
    matchInfoMethods["end"] = Symbol::Variable("end", "int", false);
    matchInfoMethods["length"] = Symbol::Variable("length", "int", false);

    auto& regexMethods = classMembers["Regex"];
    regexMethods["pattern"] = Symbol::Variable("pattern", "string", false);
    regexMethods["flags"] = Symbol::Variable("flags", "int", false);
    regexMethods["valid"] = Symbol::Variable("valid", "bool", false);

    // --- Time Module ---
    auto& timeMethods = classMembers["Time"];
    timeMethods["year"] = Symbol::Function("year", {}, "int");
    timeMethods["month"] = Symbol::Function("month", {}, "int");
    timeMethods["day"] = Symbol::Function("day", {}, "int");
    timeMethods["hour"] = Symbol::Function("hour", {}, "int");
    timeMethods["minute"] = Symbol::Function("minute", {}, "int");
    timeMethods["second"] = Symbol::Function("second", {}, "int");
    timeMethods["millisecond"] = Symbol::Function("millisecond", {}, "int");
    timeMethods["format"] = Symbol::Function("format", {"string"}, "string");
    timeMethods["add"] = Symbol::Function("add", {"Duration"}, "Time");
    timeMethods["sub"] = Symbol::Function("sub", {"Time"}, "Duration");
    timeMethods["before"] = Symbol::Function("before", {"Time"}, "bool");
    timeMethods["after"] = Symbol::Function("after", {"Time"}, "bool");
    timeMethods["equals"] = Symbol::Function("equals", {"Time"}, "bool");
    timeMethods["unix"] = Symbol::Function("unix", {}, "int");
    timeMethods["timestamp"] = Symbol::Variable("timestamp", "int", false);

    auto& durationMethods = classMembers["Duration"];
    durationMethods["getHours"] = Symbol::Function("getHours", {}, "double");
    durationMethods["getMinutes"] = Symbol::Function("getMinutes", {}, "double");
    durationMethods["getSeconds"] = Symbol::Function("getSeconds", {}, "double");
    durationMethods["getMilliseconds"] = Symbol::Function("getMilliseconds", {}, "int");
    durationMethods["add"] = Symbol::Function("add", {"Duration"}, "Duration");
    durationMethods["sub"] = Symbol::Function("sub", {"Duration"}, "Duration");
    durationMethods["value"] = Symbol::Variable("value", "int", false);

    auto& tickerMethods = classMembers["Ticker"];
    tickerMethods["stop"] = Symbol::Function("stop", {}, "void");

    auto& timerMethods = classMembers["Timer"];
    timerMethods["stop"] = Symbol::Function("stop", {}, "void");
    timerMethods["reset"] = Symbol::Function("reset", {"Duration"}, "void");
}

bool SemanticAnalyzer::analyze(const std::vector<std::unique_ptr<Stmt>>& statements) {
    hadError = false;

    // Zero-th pass: Register all packages present in the compilation unit as loaded
    for (const auto& stmt : statements) {
        if (auto* pkgDecl = dynamic_cast<PackageDecl*>(stmt.get())) {
            // Check if already in loadedModules to avoid duplicates (e.g. multiple files in same package)
            if (std::find(loadedModules.begin(), loadedModules.end(), pkgDecl->name.lexeme) == loadedModules.end()) {
                loadedModules.push_back(pkgDecl->name.lexeme);
            }
        }
    }

    // First pass: Collect all function, class, and enum declarations (including inside packages)
    for (size_t i = 0; i < statements.size(); ++i) {
        if (!statements[i]) continue;

        // Check for PackageDecl and process its contents
        if (auto* pkgDecl = dynamic_cast<PackageDecl*>(statements[i].get())) {
            std::cerr << "DEBUG: First pass - Processing PackageDecl '" << pkgDecl->name.lexeme << "' with " << pkgDecl->declarations.size() << " declarations" << std::endl;
            for (const auto& decl : pkgDecl->declarations) {
                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(decl.get())) {
                    std::vector<std::string> paramTypes;
                    for (const auto& param : funcDecl->parameters) {
                        paramTypes.push_back(param.type);
                    }
                    Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType, funcDecl->isPublic, funcDecl->isAsync, funcDecl->name.line, funcDecl->name.column, funcDecl->name.file);
                    if (!symbolTable.define(funcSymbol)) {
                        auto existing = symbolTable.resolve(funcDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined." + loc);
                    }
                } else if (auto* classDecl = dynamic_cast<ClassDecl*>(decl.get())) {
                    std::cerr << "DEBUG: First pass - Found ClassDecl '" << classDecl->name.lexeme << "' in package" << std::endl;
                    std::cerr << "DEBUG: Attempting to define class symbol..." << std::endl;
                    if (!symbolTable.define(Symbol::Class(classDecl->name.lexeme, classDecl->name.lexeme, false, classDecl->name.line, classDecl->name.column, classDecl->name.file))) {
                        auto existing = symbolTable.resolve(classDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined." + loc);
                    }
                    // Register members
                    // std::cout << "DEBUG: Registering class " << classDecl->name.lexeme << " with " << classDecl->methods.size() << " members" << std::endl;
                    for (const auto& member : classDecl->methods) {
                         if (auto* func = dynamic_cast<FunctionDecl*>(member.get())) {
                             std::vector<std::string> pTypes;
                             for (const auto& p : func->parameters) pTypes.push_back(p.type);
                             Symbol sym = Symbol::Function(func->name.lexeme, pTypes, func->returnType, func->isPublic);
                             classMembers[classDecl->name.lexeme][func->name.lexeme] = sym;
                         } else if (auto* var = dynamic_cast<VarDecl*>(member.get())) {
                             Symbol sym = Symbol::Variable(var->name.lexeme, var->typeName, var->isMutable, var->isPublic); 
                             classMembers[classDecl->name.lexeme][var->name.lexeme] = sym;
                         }
                    }
                } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(decl.get())) {
                    if (!symbolTable.define(Symbol::Class(enumDecl->name.lexeme, enumDecl->name.lexeme, false, enumDecl->name.line, enumDecl->name.column, enumDecl->name.file))) {
                        auto existing = symbolTable.resolve(enumDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined." + loc);
                    }
                    for (const auto& value : enumDecl->values) {
                        std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                        symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false));
                    }
                }
            }
            continue;
        }

        // Only process declarations, not their bodies (for top-level non-packaged code)
        if (auto* funcDecl = dynamic_cast<FunctionDecl*>(statements[i].get())) {
            std::vector<std::string> paramTypes;
            for (const auto& param : funcDecl->parameters) {
                paramTypes.push_back(param.type);
            }
            Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType, funcDecl->isPublic, funcDecl->isAsync, funcDecl->name.line, funcDecl->name.column, funcDecl->name.file);
            if (!symbolTable.define(funcSymbol)) {
                auto existing = symbolTable.resolve(funcDecl->name.lexeme);
                std::string loc = "";
                if (existing) {
                    if (existing->line > 0) {
                        loc = " (previously defined at line " + std::to_string(existing->line);
                        if (!existing->file.empty()) {
                            loc += " in " + existing->file;
                        }
                        loc += ")";
                    } else if (!existing->file.empty()) {
                        loc = " (previously defined in " + existing->file + ")";
                    }
                }
                error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined." + loc);
            }
        } else if (auto* classDecl = dynamic_cast<ClassDecl*>(statements[i].get())) {
            if (!symbolTable.define(Symbol::Class(classDecl->name.lexeme, classDecl->name.lexeme, false, classDecl->name.line, classDecl->name.column, classDecl->name.file))) {
                auto existing = symbolTable.resolve(classDecl->name.lexeme);
                std::string loc = "";
                if (existing) {
                    if (existing->line > 0) {
                        std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                        std::string fileName = p.filename().string();
                        loc = " (previously defined at ";
                        if (!fileName.empty()) {
                            loc += fileName + ":";
                        }
                        loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                    } else if (!existing->file.empty()) {
                        std::filesystem::path p(existing->file);
                        loc = " (previously defined in " + p.filename().string() + ")";
                    }
                }
                error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined." + loc);
            }
            // Register members
            for (const auto& member : classDecl->methods) {
                    if (auto* func = dynamic_cast<FunctionDecl*>(member.get())) {
                        std::vector<std::string> pTypes;
                        for (const auto& p : func->parameters) pTypes.push_back(p.type);
                        Symbol sym = Symbol::Function(func->name.lexeme, pTypes, func->returnType, func->isPublic, func->isAsync, func->name.line, func->name.column, func->name.file);
                        classMembers[classDecl->name.lexeme][func->name.lexeme] = sym;
                    } else if (auto* var = dynamic_cast<VarDecl*>(member.get())) {
                        Symbol sym = Symbol::Variable(var->name.lexeme, var->typeName, var->isMutable, var->isPublic, var->name.line, var->name.column, var->name.file);
                        classMembers[classDecl->name.lexeme][var->name.lexeme] = sym;
                    }
            }
        } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(statements[i].get())) {
            if (!symbolTable.define(Symbol::Class(enumDecl->name.lexeme, enumDecl->name.lexeme, false, enumDecl->name.line, enumDecl->name.column, enumDecl->name.file))) {
                auto existing = symbolTable.resolve(enumDecl->name.lexeme);
                std::string loc = "";
                if (existing) {
                    if (existing->line > 0) {
                        std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                        std::string fileName = p.filename().string();
                        loc = " (previously defined at ";
                        if (!fileName.empty()) {
                            loc += fileName + ":";
                        }
                        loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                    } else if (!existing->file.empty()) {
                        std::filesystem::path p(existing->file);
                        loc = " (previously defined in " + p.filename().string() + ")";
                    }
                }
                error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined." + loc);
            }
            // Also register enum values in first pass
            for (const auto& value : enumDecl->values) {
                std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false, false, value.line, value.column, value.file));
            }
        }
    }

    // Validate use statement order: use statements must come at the top (after package declaration)
    bool seenNonUseStatement = false;
    for (size_t i = 0; i < statements.size(); ++i) {
        if (!statements[i]) {
            continue;
        }

        // Skip package declarations (and reset non-use statement flag as it implies a new file context)
        if (dynamic_cast<PackageDecl*>(statements[i].get())) {
            seenNonUseStatement = false;
            continue;
        }

        // Check if this is a use statement
        if (auto* useStmt = dynamic_cast<UseStmt*>(statements[i].get())) {
            if (seenNonUseStatement) {
                error(useStmt->moduleName, "'use' statements must appear at the top of the file, after the package declaration and before any other statements.");
            }
        } else {
            // This is not a use statement, mark that we've seen non-use code
            seenNonUseStatement = true;
        }
    }

    // Second pass: Analyze all statements including bodies
    for (size_t i = 0; i < statements.size(); ++i) {
        if (!statements[i]) {
            continue;
        }
        statements[i]->accept(*this);
    }
    return !hadError;
}

void SemanticAnalyzer::error(const std::string& message) {
    std::cerr << "[Error] 0:0: " << message << std::endl; // Fallback
    hadError = true;
}

void SemanticAnalyzer::error(Token token, const std::string& message) {
    std::string location = "[Error] ";
    if (!token.file.empty()) {
        // Extract just the filename from the path for brevity
        std::filesystem::path p(token.file);
        location += p.filename().string() + ":";
    }
    location += std::to_string(token.line) + ":" + std::to_string(token.column) + ": ";
    std::cerr << location << message << std::endl;
    hadError = true;
}

// --- Expressions ---

void SemanticAnalyzer::visit(BinaryExpr& expr) {
    expr.left->accept(*this);
    std::string leftType = inferType(expr.left.get());
    if (expr.op.type == TokenType::DOT) {
        expr.left->accept(*this);
        std::string leftType = inferType(expr.left.get());
        
        // Helper to strip generics for member lookup: "List<T>" -> "List"
        std::string baseType = leftType;
        size_t paramStart = baseType.find('<');
        if (paramStart != std::string::npos) {
            baseType = baseType.substr(0, paramStart);
        }
        
        // Check for member access (method call or property get)
        // If the left type is a class/struct, check if the member exists
        if (classMembers.find(baseType) != classMembers.end()) {
             if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                 std::string methodName = rightVar->name.lexeme;
                 auto& members = classMembers[baseType];
                 
                 if (members.find(methodName) != members.end()) {
                     Symbol methodSym = members[methodName];
                     
                     // Check visibility (native types are implicitly public)
                     bool isNative = (baseType == "string" || baseType == "Array" || baseType == "array" || baseType == "map" || baseType == "Map" || baseType == "File" || baseType == "Result");
                     bool isAccessible = isNative || methodSym.isPublic || (currentClassName == baseType);
                     
                     if (!isAccessible) {
                         error(rightVar->name, "Member '" + methodName + "' is private and cannot be accessed from outside class '" + leftType + "'.");
                     }
                     return; // Resolved as valid member
                 }
             }
        }
        
        // Check if this is an enum value access (e.g., Color.RED)
        if (auto* leftVar = dynamic_cast<VariableExpr*>(expr.left.get())) {
            if (auto* rightVar = dynamic_cast<VariableExpr*>(expr.right.get())) {
                std::string enumName = leftVar->name.lexeme;
                std::string valueName = rightVar->name.lexeme;
                std::string fullName = enumName + "." + valueName;

                // Check if this is a registered enum value
                if (symbolTable.resolve(fullName)) {
                    // Valid enum value access
                    return;
                }

                // If full name resolution failed, check if the base (left side) is a valid enum/class
                // Only error if the base is actually a class/enum (static access attempt)
                // If it's a variable (instance), we fall through to regular member access handling
                auto resolvedBase = symbolTable.resolve(enumName);
                if (resolvedBase && resolvedBase->kind == SymbolKind::CLASS) {
                    // The enum/class exists, but the member does not
                    error(rightVar->name, "Member '" + valueName + "' not found in enum '" + enumName + "'.");
                    return;
                }
            }
        }
        
        // Check for class field access (e.g. file.name)
        // If the left side type is a known class, we assume the field exists for now
        // (Full field validation would require storing class structure in SymbolTable)
        // std::cerr << "[DEBUG] BinaryExpr DOT: leftType=" << leftType << std::endl;
        auto classSymbol = symbolTable.resolve(leftType);
        if (classSymbol) {
             // std::cerr << "[DEBUG] Resolved leftType to symbol kind=" << static_cast<int>(classSymbol->kind) << std::endl;
        } else {
             // std::cerr << "[DEBUG] Failed to resolve leftType=" << leftType << std::endl;
        }
        
        if (classSymbol && classSymbol->kind == SymbolKind::CLASS) {
            return; // Valid member access on object
        }

        // For other dot accesses (like module.function), just validate right side
        // The left side (module name) was already analyzed above
        expr.right->accept(*this);
        return;
    }

    // Default processing for non-dot binary expressions
    expr.left->accept(*this);
    expr.right->accept(*this);
    // TODO: Type checking (e.g., ensure left/right are numbers for +)
}

void SemanticAnalyzer::visit(UnaryExpr& expr) {
    expr.right->accept(*this);
}

void SemanticAnalyzer::visit(LiteralExpr& expr) {
    // Literals are self-contained
}

void SemanticAnalyzer::visit(VariableExpr& expr) {
    // 1. Resolution: Variable must exist
    if (!symbolTable.resolve(expr.name.lexeme)) {
        error(expr.name, "Undefined variable '" + expr.name.lexeme + "'.");
    }
}

void SemanticAnalyzer::visit(CallExpr& expr) {
    // Check if this is a module function call (e.g., math.sqrt())
    if (auto* binExpr = dynamic_cast<BinaryExpr*>(expr.callee.get())) {
        if (binExpr->op.type == TokenType::DOT) {
            if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                    std::string moduleName = leftVar->name.lexeme;
                    std::string functionName = rightVar->name.lexeme;

                    // Check if this is a native function call via NativeRegistry
                    auto& registry = NativeRegistry::getInstance();
                    if (registry.isNative(moduleName, functionName)) {
                        // Check if the module was imported
                        bool isImported = std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end();

                        if (!isImported) {
                            error(leftVar->name, "Module '" + moduleName + "' is not imported. Add 'use " + moduleName + ";' at the top of the file.");
                            return;
                        }
                        // Check if we have type signature for this function
                        if (registry.hasSignature(moduleName, functionName)) {
                            auto signature = registry.getSignature(moduleName, functionName);

                            // Check argument count
                            if (expr.arguments.size() != signature.paramTypes.size()) {
                                error(rightVar->name, "Function '" + functionName + "' expects " +
                                     std::to_string(signature.paramTypes.size()) + " arguments, got " +
                                     std::to_string(expr.arguments.size()));
                                return;
                            }

                            // Check argument types
                            for (size_t i = 0; i < expr.arguments.size(); i++) {
                                // Visit the argument first to perform semantic analysis (e.g., for lambdas)
                                expr.arguments[i]->accept(*this);

                                std::string argType = inferType(expr.arguments[i].get());
                                std::string expectedType = signature.paramTypes[i];

                                // Allow any type if expectedType is "any", or if argType is unknown
                                bool typesMatch = (argType == expectedType) || (argType == "unknown") || (expectedType == "any");
                                
                                // Relaxed check for raw Array vs generic Array
                                if (!typesMatch) {
                                    if (argType == "Array" || argType == "array") {
                                        if (expectedType.find("Array<") == 0 || expectedType.find("array<") == 0) {
                                            typesMatch = true;
                                        }
                                    }
                                }

                                if (!typesMatch) {
                                    error(rightVar->name, "Function '" + moduleName + "." + functionName +
                                         "' expects argument " + std::to_string(i + 1) +
                                         " to be of type '" + expectedType + "', found '" + argType + "'");
                                }
                            }
                        } else {
                            // No signature available, just validate arguments exist
                            for (const auto& arg : expr.arguments) {
                                arg->accept(*this);
                            }
                        }
                        return;
                    }
                }
            }
        }
    }

    // Check if this is a prelude function call (auto-imported)
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.callee.get())) {
        std::string functionName = varExpr->name.lexeme;
        auto& registry = NativeRegistry::getInstance();

        // Check if this function exists in the prelude module
        if (registry.isNative("prelude", functionName)) {
            // Validate arguments exist
            for (const auto& arg : expr.arguments) {
                arg->accept(*this);
            }
            return;  // Valid prelude function call
        }
    }

    // Not a module call or prelude call, process as regular function call
    expr.callee->accept(*this); // Verify function exists (if it's a variable access)

    // TODO: Advanced check - ensure callee is actually a callable type

    for (const auto& arg : expr.arguments) {
        arg->accept(*this);
    }
}

void SemanticAnalyzer::visit(IndexExpr& expr) {
    // Validate object and index expressions
    expr.object->accept(*this);
    expr.index->accept(*this);
}

void SemanticAnalyzer::visit(GroupingExpr& expr) {
    expr.expression->accept(*this);
}

void SemanticAnalyzer::visit(CastExpr& expr) {
    expr.expression->accept(*this);
    // Future: Verify if cast is valid (e.g. primitive to primitive)
}

void SemanticAnalyzer::visit(AwaitExpr& expr) {
    if (!currentFunctionIsAsync) {
        error(expr.keyword, "Can only use 'await' inside an async function.");
    }

    expr.expression->accept(*this);
    std::string type = inferType(expr.expression.get());

    // Check if type is awaitable (Promise<T> or Future<T>)
    if (type.find("Promise<") == 0 || type.find("Future<") == 0) {
        // Valid
    } else if (type != "any" && type != "unknown") {
        // Warning or error? For now allow any for dynamic behavior, but warn/error on known non-awaitables
        // error(expr.keyword, "Expression of type '" + type + "' is not awaitable.");
    }

    lastExprType = "any"; // Ideally T from Promise<T>
    if (type.find("<") != std::string::npos && type.back() == '>') {
        size_t start = type.find("<") + 1;
        lastExprType = type.substr(start, type.length() - start - 1);
    }
}

void SemanticAnalyzer::visit(MapLiteralExpr& expr) {
    for (const auto& pair : expr.entries) {
        pair.second->accept(*this);
    }
}

void SemanticAnalyzer::visit(ArrayLiteralExpr& expr) {
    for (const auto& element : expr.elements) {
        element->accept(*this);
    }
}

void SemanticAnalyzer::visit(LambdaExpr& expr) {
    bool previousAsync = currentFunctionIsAsync;
    currentFunctionIsAsync = expr.isAsync;

    symbolTable.enterScope();
    for (const auto& param : expr.params) {
        symbolTable.define(Symbol::Variable(param.lexeme, "any", false, false, param.line, param.column, param.file));
    }
    if (expr.body) expr.body->accept(*this);
    symbolTable.exitScope();

    currentFunctionIsAsync = previousAsync;
}

// --- Statements ---

void SemanticAnalyzer::visit(VarDecl& stmt) {
    // 1. Analyze initializer first (so it can't refer to the variable being declared)
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }

    // 2. Define variable in current scope
    std::string type = stmt.typeName;
    if (type.empty()) {
        if (stmt.initializer) {
            type = inferType(stmt.initializer.get());
        } else {
            type = "any";
        }
    }
    Symbol symbol = Symbol::Variable(stmt.name.lexeme, type, stmt.isMutable, stmt.isPublic, stmt.name.line, stmt.name.column, stmt.name.file);

    if (!symbolTable.define(symbol)) {
        error(stmt.name, "Variable '" + stmt.name.lexeme + "' is already defined in this scope.");
    }
}

void SemanticAnalyzer::visit(FunctionDecl& stmt) {
    // Try to define function name in current scope (may already be defined from first pass for top-level functions)
    std::vector<std::string> paramTypes;
    for (const auto& param : stmt.parameters) {
        paramTypes.push_back(param.type);
    }
    
    Symbol funcSymbol = Symbol::Function(stmt.name.lexeme, paramTypes, stmt.returnType, stmt.isPublic, stmt.isAsync);
    if (!symbolTable.define(funcSymbol)) {
        // Only error if we're in a class (class methods should be defined fresh)
        // Top-level functions are expected to fail here due to first pass registration
        if (!currentClassName.empty()) {
            error(stmt.name, "Function '" + stmt.name.lexeme + "' is already defined.");
        }
    }

    // Only analyze body if it exists (skip function declarations without bodies)
    if (!stmt.body) {
        return;
    }

    // Enter new scope for function body
    std::cerr << "DEBUG: Visiting FunctionDecl '" << stmt.name.lexeme << "', currentClassName='" << currentClassName << "'" << std::endl;
    symbolTable.enterScope();

    // If we're in a class, define 'this' in the method scope
    if (!currentClassName.empty()) {
        std::cerr << "DEBUG: Defining 'this' in FunctionDecl scope with type '" << currentClassName << "'" << std::endl;
        symbolTable.define(Symbol::Variable("this", currentClassName, false));
    }

    // Define parameters as variables
    for (const auto& param : stmt.parameters) {
        Symbol paramSym = Symbol::Variable(param.name.lexeme, param.type, false, false, param.name.line, param.name.column, param.name.file);
        symbolTable.define(paramSym);

        // Analyze default value if present
        if (param.defaultValue) {
            param.defaultValue->accept(*this);
        }
    }

    // Analyze body
    bool previousAsync = currentFunctionIsAsync;
    currentFunctionIsAsync = stmt.isAsync;

    for (const auto& s : *stmt.body) {
        if (s) s->accept(*this);
    }

    currentFunctionIsAsync = previousAsync;

    // Exit scope
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ClassDecl& stmt) {
    // Try to define class (may already be defined from first pass)
    symbolTable.define(Symbol{stmt.name.lexeme, SymbolKind::CLASS, stmt.name.lexeme, false});

    // Set current class context
    std::string previousClassName = currentClassName;
    currentClassName = stmt.name.lexeme;
    std::cerr << "DEBUG: Visiting ClassDecl for '" << stmt.name.lexeme << "', setting currentClassName" << std::endl;

    symbolTable.enterScope();
    // Define 'this'
    std::cerr << "DEBUG: Defining 'this' in ClassDecl scope for '" << stmt.name.lexeme << "'" << std::endl;
    symbolTable.define(Symbol::Variable("this", stmt.name.lexeme, false, false, stmt.name.line, stmt.name.column, stmt.name.file));

    for (const auto& member : stmt.methods) {
        if (member) {
            member->accept(*this);
        }
    }

    symbolTable.exitScope();

    // Restore previous class context
    currentClassName = previousClassName;
}

void SemanticAnalyzer::visit(EnumDecl& stmt) {
    // Register the enum type itself (may already be defined from first pass)
    symbolTable.define(Symbol{stmt.name.lexeme, SymbolKind::CLASS, stmt.name.lexeme, false});

    // Register each enum value as a constant (may already be defined from first pass)
    // Enum values are accessed as EnumName.VALUE, so we store them with dotted names
    for (const auto& value : stmt.values) {
        std::string fullName = stmt.name.lexeme + "." + value.lexeme;
        // Register as a variable (constant) of the enum type
        symbolTable.define(Symbol::Variable(fullName, stmt.name.lexeme, false));
    }
}

void SemanticAnalyzer::visit(PackageDecl& stmt) {
    std::cerr << "DEBUG: Visiting PackageDecl '" << stmt.name.lexeme << "' with " << stmt.declarations.size() << " declarations" << std::endl;
    // Package declaration (like "package main;") just declares what package
    // this file belongs to - it doesn't create a symbol in the namespace.
    // Don't define the package name as a symbol to avoid conflicts.
    // NOTE: Package imports (via 'use' keyword) would be different.

    // Process declarations within the package without creating a new scope
    // or defining the package name as a symbol
    for (size_t i = 0; i < stmt.declarations.size(); ++i) {
        if (stmt.declarations[i]) {
            if (auto* classDecl = dynamic_cast<ClassDecl*>(stmt.declarations[i].get())) {
                std::cerr << "DEBUG:   - Declaration " << i << ": ClassDecl '" << classDecl->name.lexeme << "'" << std::endl;
            } else if (auto* funcDecl = dynamic_cast<FunctionDecl*>(stmt.declarations[i].get())) {
                std::cerr << "DEBUG:   - Declaration " << i << ": FunctionDecl '" << funcDecl->name.lexeme << "'" << std::endl;
            }
            stmt.declarations[i]->accept(*this);
        }
    }
}

void SemanticAnalyzer::visit(UseStmt& stmt) {
    // Load the module if not already loaded
    std::string moduleName = stmt.moduleName.lexeme;

    std::cerr << "DEBUG: UseStmt processing module '" << moduleName << "'" << std::endl;

    // Check if already loaded
    if (std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end()) {
        std::cerr << "DEBUG: Module '" << moduleName << "' already loaded" << std::endl;
        return; // Already loaded
    }

    // Check if this is a native module (implemented in C++ via NativeRegistry)
    auto& registry = NativeRegistry::getInstance();
    bool isNativeModule = registry.hasModule(moduleName);

    // Try to load as a source file module (even for native modules - they can have .st implementations)
    loadModule(moduleName);

    // Register as loaded (loadModule already adds to loadedModules)
    if (std::find(loadedModules.begin(), loadedModules.end(), moduleName) == loadedModules.end()) {
        loadedModules.push_back(moduleName);
    }

    // Define the module name as a variable in the current scope
    symbolTable.define(Symbol::Variable(moduleName, "module", false, false, stmt.moduleName.line, stmt.moduleName.column, stmt.moduleName.file));
    std::cerr << "DEBUG: UseStmt completed for module '" << moduleName << "'" << std::endl;
}

void SemanticAnalyzer::visit(BlockStmt& stmt) {
    symbolTable.enterScope();
    for (size_t i = 0; i < stmt.statements.size(); ++i) {
        if (stmt.statements[i]) {
            stmt.statements[i]->accept(*this);
        }
    }
    symbolTable.exitScope();
}

void SemanticAnalyzer::visit(ExpressionStmt& stmt) {
    stmt.expression->accept(*this);
}

void SemanticAnalyzer::visit(PrintStmt& stmt) {
    stmt.expression->accept(*this);
}

void SemanticAnalyzer::visit(IfStmt& stmt) {
    stmt.condition->accept(*this);
    stmt.thenBranch->accept(*this);
    if (stmt.elseBranch) stmt.elseBranch->accept(*this);
}

void SemanticAnalyzer::visit(WhileStmt& stmt) {
    stmt.condition->accept(*this);
    loopDepth++;
    stmt.body->accept(*this);
    loopDepth--;
}

void SemanticAnalyzer::visit(ForStmt& stmt) {
    // Analyze the iterable expression first (in outer scope)
    stmt.iterable->accept(*this);
    
    loopDepth++;

    // Enter new scope for loop variable and body
    symbolTable.enterScope();

    // Define the loop variable in this scope
    // For now, use the provided type or infer from iterable if possible
    std::string loopVarType = stmt.varType;
    if (loopVarType.empty()) {
        std::string iterableType = inferType(stmt.iterable.get());
        if (iterableType.find("Array<") == 0 && iterableType.back() == '>') {
             loopVarType = iterableType.substr(6, iterableType.length() - 7);
        } else if (iterableType == "string") {
             loopVarType = "char";
        } else {
             loopVarType = "any";
        }
    }
    
    Symbol loopVar = Symbol::Variable(stmt.variable.lexeme, loopVarType, stmt.isMutable);
    if (!symbolTable.define(loopVar)) {
        error(stmt.variable, "Loop variable '" + stmt.variable.lexeme + "' is already defined in this scope.");
    }

    // Analyze loop body
    stmt.body->accept(*this);

    // Exit loop scope
    symbolTable.exitScope();
    
    loopDepth--;
}

void SemanticAnalyzer::visit(ReturnStmt& stmt) {
    if (stmt.value) {
        stmt.value->accept(*this);
    }
    // TODO: Check if return type matches function signature
}

void SemanticAnalyzer::visit(BreakStmt& stmt) {
    if (loopDepth == 0) {
        error(stmt.keyword, "'break' cannot be used outside of a loop.");
    }
}

void SemanticAnalyzer::visit(ContinueStmt& stmt) {
    if (loopDepth == 0) {
        error(stmt.keyword, "'continue' cannot be used outside of a loop.");
    }
}

void SemanticAnalyzer::loadModule(const std::string& moduleName) {
    namespace fs = std::filesystem;

    // Possible module file locations
    std::vector<std::string> searchPaths = {
        // Internal project packages (highest priority)
        projectRoot + "/src/" + moduleName + "/init.st",
        projectRoot + "/src/" + moduleName + "/" + moduleName + ".st",

        // Dependencies directory
        projectRoot + "/deps/" + moduleName + "/src/init.st",
        projectRoot + "/deps/" + moduleName + "/src/" + moduleName + ".st",
        projectRoot + "/../deps/" + moduleName + "/src/init.st",
        projectRoot + "/../deps/" + moduleName + "/src/" + moduleName + ".st",

        // Current directory (fallback)
        "std/" + moduleName + "/init.st",
        "std/encoding/" + moduleName + "/init.st",

        // Build directory
        "build/std/" + moduleName + "/init.st",
        "build/std/encoding/" + moduleName + "/init.st",

        // One level up (from samples/ or similar)
        "../std/" + moduleName + "/init.st",
        "../std/encoding/" + moduleName + "/init.st",
        "../build/std/" + moduleName + "/init.st",
        "../build/std/encoding/" + moduleName + "/init.st",
        "../interpreter/C++/build/std/" + moduleName + "/init.st",
        "../interpreter/C++/build/std/encoding/" + moduleName + "/init.st",

        // Two levels up
        "../../std/" + moduleName + "/init.st",
        "../../std/encoding/" + moduleName + "/init.st",
        "../../interpreter/C++/build/std/" + moduleName + "/init.st",
        "../../interpreter/C++/build/std/encoding/" + moduleName + "/init.st",

        // Three levels up
        "../../../std/" + moduleName + "/init.st",
        "../../../std/encoding/" + moduleName + "/init.st"
    };

    std::string moduleFilePath;
    bool isUserModule = false;

    for (const auto& path : searchPaths) {
        if (fs::exists(path)) {
            moduleFilePath = path;
            // Check if this is a user-defined module (in src/ directory)
            isUserModule = path.starts_with("src/");
            break;
        }
    }

    // If no specific module file was found, check if module directory exists
    if (moduleFilePath.empty()) {
        std::string moduleDir = projectRoot + "/src/" + moduleName;
        if (fs::exists(moduleDir) && fs::is_directory(moduleDir)) {
            moduleFilePath = moduleDir;
            isUserModule = true;
        }
    }

    if (moduleFilePath.empty()) {
        std::cerr << "DEBUG: Module '" << moduleName << "' not found. Searched paths:" << std::endl;
        for (const auto& path : searchPaths) {
            std::cerr << "  - " << path << std::endl;
        }
        error("Could not find module '" + moduleName + "'. Searched in std directories.");
        return;
    }

    std::cerr << "DEBUG: Found module '" << moduleName << "' at: " << moduleFilePath << std::endl;

    // UPDATED: Now we parse ALL modules (std and user-defined) to support pure Stratos implementations
    // /* OLD BEHAVIOR - commented out to preserve history
    // // For built-in/native modules, just register the module name
    // if (!isUserModule) {
    //     // Module file exists - just register it
    //     // We don't parse the module file because:
    //     // 1. Native functions are implemented in C++ (NativeRegistry)
    //     // 2. Module files may use advanced syntax not yet fully supported
    //     // 3. We only need to know the module exists and is available
    //     // 4. Actual function validation happens during CallExpr analysis via NativeRegistry
    //
    //     // Register the module in the symbol table
    //     symbolTable.define(Symbol::Variable(moduleName, "module", false));
    //     return;
    // }
    // */

    // Register the module in the symbol table
    symbolTable.define(Symbol::Variable(moduleName, "module", false));

    // Parse all files in the module directory (both std and user-defined)
    // This enables pure Stratos implementations alongside native C++ functions
    std::vector<std::string> moduleFiles;

    // If moduleFilePath is a directory, scan for all .st files
    // Otherwise, it's a file and we also need to scan its directory
    fs::path modulePath(moduleFilePath);

    if (fs::is_directory(modulePath)) {
        // moduleFilePath is already the directory, scan it
        for (const auto& entry : fs::directory_iterator(modulePath)) {
            if (entry.path().extension() == ".st") {
                moduleFiles.push_back(entry.path().string());
            }
        }
    } else if (fs::is_regular_file(modulePath)) {
        // moduleFilePath is a specific file, scan its parent directory
        fs::path moduleDir = modulePath.parent_path();
        for (const auto& entry : fs::directory_iterator(moduleDir)) {
            if (entry.path().extension() == ".st") {
                moduleFiles.push_back(entry.path().string());
            }
        }
    } else {
        // Fallback: just use the file itself if it exists
        moduleFiles.push_back(moduleFilePath);
    }

    // Sort files to ensure consistent processing order (e.g., console.st before init.st)
    std::sort(moduleFiles.begin(), moduleFiles.end());

    // Parse and process each file in the module
    for (const auto& filePath : moduleFiles) {
        std::cerr << "DEBUG: Processing module file: " << filePath << std::endl;
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "DEBUG: Failed to open file: " << filePath << std::endl;
            continue; // Skip files we can't open
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        try {
            // Lex and parse the module file
            Lexer lexer(source, filePath);
            std::vector<Token> tokens = lexer.scanTokens();

            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
            std::cerr << "DEBUG: Parsed " << statements.size() << " statements from " << filePath << std::endl;

            // Debug: print what types of statements we got
            for (size_t i = 0; i < statements.size(); ++i) {
                if (!statements[i]) continue;
                if (dynamic_cast<PackageDecl*>(statements[i].get())) {
                    std::cerr << "  Statement " << i << ": PackageDecl" << std::endl;
                } else if (auto* classDecl = dynamic_cast<ClassDecl*>(statements[i].get())) {
                    std::cerr << "  Statement " << i << ": ClassDecl '" << classDecl->name.lexeme << "'" << std::endl;
                } else if (auto* funcDecl = dynamic_cast<FunctionDecl*>(statements[i].get())) {
                    std::cerr << "  Statement " << i << ": FunctionDecl '" << funcDecl->name.lexeme << "'" << std::endl;
                } else if (dynamic_cast<UseStmt*>(statements[i].get())) {
                    std::cerr << "  Statement " << i << ": UseStmt" << std::endl;
                } else {
                    std::cerr << "  Statement " << i << ": Other type" << std::endl;
                }
            }

            // Two-pass processing like analyze() does:
            // First pass: Collect all function, class, and enum declarations
            for (size_t i = 0; i < statements.size(); ++i) {
                if (!statements[i]) continue;

                // Check for PackageDecl and process its contents
                if (auto* pkgDecl = dynamic_cast<PackageDecl*>(statements[i].get())) {
                    std::cerr << "DEBUG: First pass - Processing PackageDecl '" << pkgDecl->name.lexeme << "' with " << pkgDecl->declarations.size() << " declarations" << std::endl;
                    for (const auto& decl : pkgDecl->declarations) {
                        if (auto* funcDecl = dynamic_cast<FunctionDecl*>(decl.get())) {
                            std::vector<std::string> paramTypes;
                            for (const auto& param : funcDecl->parameters) {
                                paramTypes.push_back(param.type);
                            }
                            Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType, funcDecl->isPublic, funcDecl->isAsync, funcDecl->name.line, funcDecl->name.column, funcDecl->name.file);
                            if (!symbolTable.define(funcSymbol)) {
                                // Ignore redefinitions for now or handle gracefully
                            }
                        } else if (auto* classDecl = dynamic_cast<ClassDecl*>(decl.get())) {
                            std::cerr << "DEBUG: Registering class '" << classDecl->name.lexeme << "' from file: " << filePath << std::endl;
                            if (symbolTable.define(Symbol::Class(classDecl->name.lexeme, classDecl->name.lexeme, false, classDecl->name.line, classDecl->name.column, classDecl->name.file))) {
                                std::cerr << "DEBUG: Successfully registered class '" << classDecl->name.lexeme << "'" << std::endl;
                            }
                            // Register members
                            for (const auto& member : classDecl->methods) {
                                if (auto* func = dynamic_cast<FunctionDecl*>(member.get())) {
                                    std::vector<std::string> pTypes;
                                    for (const auto& p : func->parameters) pTypes.push_back(p.type);
                                    Symbol sym = Symbol::Function(func->name.lexeme, pTypes, func->returnType, func->isPublic, func->isAsync, func->name.line, func->name.column, func->name.file);
                                    classMembers[classDecl->name.lexeme][func->name.lexeme] = sym;
                                } else if (auto* var = dynamic_cast<VarDecl*>(member.get())) {
                                    Symbol sym = Symbol::Variable(var->name.lexeme, var->typeName, var->isMutable, var->isPublic, var->name.line, var->name.column, var->name.file);
                                    classMembers[classDecl->name.lexeme][var->name.lexeme] = sym;
                                }
                            }
                        } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(decl.get())) {
                            if (symbolTable.define(Symbol::Class(enumDecl->name.lexeme, enumDecl->name.lexeme, false, enumDecl->name.line, enumDecl->name.column, enumDecl->name.file))) {
                                // Success
                            }
                            for (const auto& value : enumDecl->values) {
                                std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                                symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false, false, value.line, value.column, filePath));
                            }
                        }
                    }
                    continue;
                }

                // Only process declarations, not their bodies
                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(statements[i].get())) {
                    std::vector<std::string> paramTypes;
                    for (const auto& param : funcDecl->parameters) {
                        paramTypes.push_back(param.type);
                    }
                    Symbol funcSymbol = Symbol::Function(funcDecl->name.lexeme, paramTypes, funcDecl->returnType, funcDecl->isPublic, funcDecl->isAsync, funcDecl->name.line, funcDecl->name.column, funcDecl->name.file);
                    if (!symbolTable.define(funcSymbol)) {
                        auto existing = symbolTable.resolve(funcDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(funcDecl->name, "Function '" + funcDecl->name.lexeme + "' is already defined." + loc);
                    }
                } else if (auto* classDecl = dynamic_cast<ClassDecl*>(statements[i].get())) {
                    std::cerr << "DEBUG: Registering class '" << classDecl->name.lexeme << "' from file: " << filePath << std::endl;
                    if (!symbolTable.define(Symbol::Class(classDecl->name.lexeme, classDecl->name.lexeme, false, classDecl->name.line, classDecl->name.column, classDecl->name.file))) {
                        auto existing = symbolTable.resolve(classDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(classDecl->name, "Class '" + classDecl->name.lexeme + "' is already defined." + loc);
                    } else {
                        std::cerr << "DEBUG: Successfully registered class '" << classDecl->name.lexeme << "'" << std::endl;
                    }
                    // Register members
                    for (const auto& member : classDecl->methods) {
                        if (auto* func = dynamic_cast<FunctionDecl*>(member.get())) {
                            std::vector<std::string> pTypes;
                            for (const auto& p : func->parameters) pTypes.push_back(p.type);
                            Symbol sym = Symbol::Function(func->name.lexeme, pTypes, func->returnType, func->isPublic, func->isAsync, func->name.line, func->name.column, func->name.file);
                            classMembers[classDecl->name.lexeme][func->name.lexeme] = sym;
                            // std::cout << "DEBUG: Registered method " << classDecl->name.lexeme << "." << func->name.lexeme << " return=" << func->returnType << std::endl;
                        } else if (auto* var = dynamic_cast<VarDecl*>(member.get())) {
                            Symbol sym = Symbol::Variable(var->name.lexeme, var->typeName, var->isMutable, var->isPublic, var->name.line, var->name.column, var->name.file);
                            classMembers[classDecl->name.lexeme][var->name.lexeme] = sym;
                            // std::cout << "DEBUG: Registered field " << classDecl->name.lexeme << "." << var->name.lexeme << " type=" << var->typeName << std::endl;
                        }
                    }
                } else if (auto* enumDecl = dynamic_cast<EnumDecl*>(statements[i].get())) {
                    if (!symbolTable.define(Symbol::Class(enumDecl->name.lexeme, enumDecl->name.lexeme, false, enumDecl->name.line, enumDecl->name.column, enumDecl->name.file))) {
                        auto existing = symbolTable.resolve(enumDecl->name.lexeme);
                        std::string loc = "";
                        if (existing) {
                            if (existing->line > 0) {
                                std::filesystem::path p(existing->file.empty() ? "" : existing->file);
                                std::string fileName = p.filename().string();
                                loc = " (previously defined at ";
                                if (!fileName.empty()) {
                                    loc += fileName + ":";
                                }
                                loc += std::to_string(existing->line) + ":" + std::to_string(existing->column) + ")";
                            } else if (!existing->file.empty()) {
                                std::filesystem::path p(existing->file);
                                loc = " (previously defined in " + p.filename().string() + ")";
                            }
                        }
                        error(enumDecl->name, "Enum '" + enumDecl->name.lexeme + "' is already defined." + loc);
                    }
                    // Also register enum values in first pass
                    for (const auto& value : enumDecl->values) {
                        std::string fullName = enumDecl->name.lexeme + "." + value.lexeme;
                        symbolTable.define(Symbol::Variable(fullName, enumDecl->name.lexeme, false, false, value.line, value.column, filePath));
                    }
                }
            }

            // Second pass: Process all statements including bodies
            for (const auto& stmt : statements) {
                if (stmt) {
                    stmt->accept(*this);
                }
            }
        } catch (const std::exception& e) {
            error("Error loading module '" + moduleName + "' from " + filePath + ": " + e.what());
        }
    }
}

std::string SemanticAnalyzer::inferType(Expr* expr) {
    // Infer the type of an expression
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (literal->type == TokenType::NUMBER) {
            // Check if it's a double (has decimal point) or int
            if (literal->value.find('.') != std::string::npos) {
                return "double";
            } else {
                return "int";
            }
        } else if (literal->type == TokenType::STRING) {
            return "string";
        } else if (literal->type == TokenType::TRUE || literal->type == TokenType::FALSE) {
            return "bool";
        }
    } else if (auto* varExpr = dynamic_cast<VariableExpr*>(expr)) {
        // Look up variable type from symbol table
        auto symbolOpt = symbolTable.resolve(varExpr->name.lexeme);
        if (symbolOpt) {
            return symbolOpt->type;
        }
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        std::string leftType = inferType(binary->left.get());
        
        if (binary->op.type == TokenType::DOT) {
            // Check lookup for fields
             if (classMembers.find(leftType) != classMembers.end()) {
                 if (auto* rightVar = dynamic_cast<VariableExpr*>(binary->right.get())) {
                      std::string name = rightVar->name.lexeme;
                      if (classMembers[leftType].count(name)) {
                          // std::cout << "DEBUG: Inferred member " << leftType << "." << name << " -> " << classMembers[leftType][name].type << std::endl;
                          return classMembers[leftType][name].type; // For fields and methods (return type)
                      }
                 }
            }

            if (leftType.find("Array<") == 0 || leftType == "Array") {
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binary->right.get())) {
                    std::string memberName = rightVar->name.lexeme;
                    if (memberName == "length") return "int";
                    if (memberName == "add") return "void"; // Add typically returns void or the element
                }
            }
        }

        // Infer type from binary operation
        std::string rightType = inferType(binary->right.get());

        // If either side is double, result is double
        if (leftType == "double" || rightType == "double") {
            return "double";
        }
        // If both are int, result is int
        if (leftType == "int" && rightType == "int") {
            return "int";
        }
        // String concatenation
        if (leftType == "string" || rightType == "string") {
            return "string";
        }
    } else if (auto* callExpr = dynamic_cast<CallExpr*>(expr)) {
        // Check for Array constructor
        if (auto* varExpr = dynamic_cast<VariableExpr*>(callExpr->callee.get())) {
            if (varExpr->name.lexeme == "Array") {
                 return "Array";
            }
            
            // Handle simple function call (local or prelude)
            auto symbolOpt = symbolTable.resolve(varExpr->name.lexeme);
            if (symbolOpt) {
                if (symbolOpt->kind == SymbolKind::FUNCTION) {
                    return symbolOpt->returnType;
                }
                // If it's a variable (like a callback), check if it's a generic Function type
                if (symbolOpt->type.find("Function<") == 0 && symbolOpt->type.back() == '>') {
                     return symbolOpt->type.substr(9, symbolOpt->type.length() - 10);
                }
                return symbolOpt->type;
            }
        }

        // Try to infer return type from function signature
        if (auto* binExpr = dynamic_cast<BinaryExpr*>(callExpr->callee.get())) {
            if (binExpr->op.type == TokenType::DOT) {
                // Check if it's a method call on an object
                std::string leftType = inferType(binExpr->left.get());
                if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                    std::string methodName = rightVar->name.lexeme;

                    // Strip generics
                    std::string baseType = leftType;
                    size_t paramStart = baseType.find('<');
                    if (paramStart != std::string::npos) {
                        baseType = baseType.substr(0, paramStart);
                    }

                    // Check class members
                    if (classMembers.find(baseType) != classMembers.end()) {
                        auto& members = classMembers[baseType];
                        if (members.find(methodName) != members.end()) {
                            // std::cout << "DEBUG: Inferred method type " << baseType << "." << methodName << " -> " << members[methodName].returnType << std::endl;
                            return members[methodName].returnType;
                        } else {
                            // std::cout << "DEBUG: Method " << methodName << " not found in class " << baseType << ". Available: ";
                            // for (const auto& kv : members) std::cout << kv.first << " ";
                            // std::cout << std::endl;
                        }
                    } else {
                         // std::cout << "DEBUG: Class " << baseType << " not found. Available: ";
                         // for (const auto& kv : classMembers) std::cout << kv.first << " ";
                         // std::cout << std::endl;
                    }

                    // Check generic symbols (e.g. module functions)
                    if (leftType == "module") {
                         auto sym = symbolTable.resolve(methodName);
                         if (sym && sym->kind == SymbolKind::FUNCTION) {
                             return sym->returnType;
                         }
                    }


                     
                     if (leftType == "string") {
                         if (methodName == "split") return "Array<string>";
                         if (methodName == "length") return "int";
                         if (methodName == "repeat") return "string";
                         if (methodName == "trim" || methodName == "trimLeft" || methodName == "trimRight" || 
                             methodName == "trimPrefix" || methodName == "trimSuffix") return "string";
                         if (methodName == "toUpper" || methodName == "toLower" || methodName == "toTitle") return "string";
                         if (methodName == "startsWith" || methodName == "endsWith") return "bool";
                     }
                     if (leftType.find("Array") == 0 || leftType == "array") {
                         if (methodName == "length") return "int";
                         if (methodName == "join") return "string";
                         if (methodName == "isEmpty") return "bool";
                         if (methodName == "contains") return "bool";
                         if (methodName == "indexOf") return "int";
                         // For first/last/get, we should return the element type, but we don't track generics well yet.
                         // For Array<string>, element is string.
                         if (methodName == "first" || methodName == "last" || methodName == "get") {
                             if (leftType.find("Array<") == 0 && leftType.back() == '>') {
                                 return leftType.substr(6, leftType.length() - 7);
                             }
                             return "any";
                         }
                         if (methodName == "reverse" || methodName == "clear") return leftType;
                     }
                }

                if (auto* leftVar = dynamic_cast<VariableExpr*>(binExpr->left.get())) {
                    if (auto* rightVar = dynamic_cast<VariableExpr*>(binExpr->right.get())) {
                        std::string moduleName = leftVar->name.lexeme;
                        std::string functionName = rightVar->name.lexeme;

                        auto& registry = NativeRegistry::getInstance();
                        if (registry.hasSignature(moduleName, functionName)) {
                            auto signature = registry.getSignature(moduleName, functionName);
                            return signature.returnType;
                        }
                    }
                }

            }
        }
    } 
    
    if (auto* castExpr = dynamic_cast<CastExpr*>(expr)) {
        return castExpr->typeToken.lexeme;
    }

    if (auto* structExpr = dynamic_cast<StructInitExpr*>(expr)) {
        return structExpr->name.lexeme;
    }

    return "unknown";
}

void SemanticAnalyzer::visit(StructInitExpr& expr) {
    // 1. Verify struct existence
    // Since we don't have full type checking on structs yet, we just check if it's available in symbol table if possible
    // But struct definitions are treated as ClassDecl which might be in classes map in Interpreter, 
    // Here in SemanticAnalyzer we track symbols.
    
    // 2. Analyze fields
    for (const auto& field : expr.fields) {
        field.second->accept(*this);
        // lastExprType is set by visit
    }
    
    lastExprType = expr.name.lexeme; // Tentative type name
}

} // namespace stratos
