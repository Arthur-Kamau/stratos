#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/AST.h"
#include "stratos/SemanticAnalyzer.h"
#include "stratos/Interpreter.h"
#include "stratos/NativeRegistry.h"

using namespace stratos;

int main(int argc, char** argv) {
    // Initialize the standard library (prelude, math, etc.)
    NativeRegistry::getInstance().initializeStdlib();

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file.st>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filePath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // Lexical analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();

        // Parsing
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

        // Semantic analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(statements)) {
            std::cerr << "Semantic analysis failed" << std::endl;
            return 1;
        }

        // Interpretation
        Interpreter interpreter;
        interpreter.execute(statements);

        // Call main function
        std::vector<stratos::RuntimeValue> args;
        interpreter.callFunction("main", args);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
