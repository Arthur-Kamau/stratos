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
            // std::cerr << "[DEBUG] Symbol '" << symbol.name << "' already defined in current scope (depth " << scopes.size() << ")" << std::endl;
            return false; // Already defined in this scope
        }
        currentScope[symbol.name] = symbol;
        // std::cerr << "[DEBUG] Defined Symbol '" << symbol.name << "' type='" << symbol.type << "' (depth " << scopes.size() << ")" << std::endl;
        return true;
    }

    // Look up a symbol by name, searching from current scope outwards
    std::optional<Symbol> resolve(const std::string& name) {
        for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
            auto found = it->find(name);
            if (found != it->end()) {
                // std::cerr << "[DEBUG] Resolved Symbol '" << name << "' (depth " << (scopes.size() - (it - scopes.rbegin())) << ")" << std::endl;
                return found->second;
            }
        }
        // std::cerr << "[DEBUG] Failed to resolve Symbol '" << name << "'" << std::endl;
        return std::nullopt;
    }

    // Check if symbol exists in the IMMEDIATE (current) scope only
    bool isDefinedInCurrentScope(const std::string& name) {
        if (scopes.empty()) return false;
        return scopes.back().count(name) > 0;
    }

    // Get current scope depth (for scope guards)
    size_t depth() const { return scopes.size(); }

    // Restore to a specific depth (emergency scope repair)
    void restoreDepth(size_t targetDepth) {
        while (scopes.size() > targetDepth) {
            scopes.pop_back();
        }
    }

private:
    // A stack of scopes, where each scope is a map of name -> Symbol
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
};

} // namespace stratos

#endif // STRATOS_SYMBOL_TABLE_H