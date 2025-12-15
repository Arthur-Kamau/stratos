#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/AST.h"
#include "stratos/SemanticAnalyzer.h"
#include "stratos/IRGenerator.h" // CodeGen

using namespace stratos;
namespace fs = std::filesystem;

// ASTPrinter omitted for brevity (but ideally kept in a util file)
// ...

void runTest(const std::string& path) {
    std::cout << "Processing: " << path << std::endl;
    std::ifstream file(path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    
    Parser parser(tokens);
    try {
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
        std::cout << "  [Parser]    OK." << std::endl;
        
        // Semantic Analysis
        SemanticAnalyzer analyzer;
        if (analyzer.analyze(statements)) {
            std::cout << "  [Semantics] OK." << std::endl;
            
            // Code Generation
            std::string irPath = path + ".ll";
            IRGenerator generator(irPath);
            generator.generate(statements);
            std::cout << "  [CodeGen]   Generated " << irPath << std::endl;
            
        } else {
            std::cout << "  [Semantics] FAIL (Skipping CodeGen)." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "  [FAIL] " << e.what() << std::endl;
    }
    std::cout << "-----------------------------------" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string casesDir = "../../cases"; // Relative to build directory
    
    // Check if path provided
    if (argc > 1) {
        casesDir = argv[1];
    }

    std::cout << "Searching for test cases in: " << casesDir << "\n" << std::endl;

    if (fs::exists(casesDir) && fs::is_directory(casesDir)) {
        for (const auto& entry : fs::directory_iterator(casesDir)) {
            if (entry.path().extension() == ".st") {
                runTest(entry.path().string());
            }
        }
    } else {
        std::cerr << "Directory not found: " << casesDir << std::endl;
        std::cout << "Trying absolute path..." << std::endl;
        runTest("C:\\Users\\ADMIN\\Desktop\\Development\\Projects\\stratos\\compiler\\cases\\01_basics.st");
    }

    return 0;
}
