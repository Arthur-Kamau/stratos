#ifndef STRATOS_SYMBOL_TABLE_H
#define STRATOS_SYMBOL_TABLE_H

#include "Symbol.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <optional>
#include <iostream>

namespace stratos {

class SymbolTable {
public:
    SymbolTable() {
        // Start with a global scope
        enterScope();
    }

    // Enter a new nested scope (e.g., entering a block or function)
    void enterScope() {
        scopes.push_back({});
        // std::cout << "  [Scope] Enter (Depth: " << scopes.size() << ")" << std::endl;
    }

    // Exit the current scope
    void exitScope() {
        if (!scopes.empty()) {
            scopes.pop_back();
            // std::cout << "  [Scope] Exit (Depth: " << scopes.size() << ")" << std::endl;
        }
    }

    // Define a symbol in the CURRENT scope
    // Returns false if symbol already exists in this specific scope
    bool define(Symbol symbol) {
        if (scopes.empty()) return false;
        
        auto& currentScope = scopes.back();
        if (currentScope.find(symbol.name) != currentScope.end()) {
            // std::cout << "  [Define] FAILED: " << symbol.name << " already exists in scope " << scopes.size() << std::endl;
            return false; // Already defined in this scope
        }

        currentScope[symbol.name] = symbol;
        // std::cout << "  [Define] " << symbol.name << " (Scope " << scopes.size() << ")" << std::endl;
        return true;
    }

    // Look up a symbol by name, searching from current scope outwards
    std::optional<Symbol> resolve(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                // std::cout << "  [Resolve] Found " << name << std::endl;
                return found->second;
            }
        }
        // std::cout << "  [Resolve] FAILED " << name << std::endl;
        return std::nullopt;
    }

    // Check if symbol exists in the IMMEDIATE (current) scope only
    bool isDefinedInCurrentScope(const std::string& name) {
        if (scopes.empty()) return false;
        return scopes.back().count(name) > 0;
    }

private:
    // A stack of scopes, where each scope is a map of name -> Symbol
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

} // namespace stratos

#endif // STRATOS_SYMBOL_TABLE_H