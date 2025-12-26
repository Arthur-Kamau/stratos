// Improved main.cpp with proper CLI handling
// Replace your current main.cpp with this version

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>

#include "stratos/Lexer.h"
#include "stratos/Parser.h"
#include "stratos/AST.h"
#include "stratos/SemanticAnalyzer.h"
#include "stratos/IRGenerator.h"
#include "stratos/Optimizer.h"
#include "stratos/ProjectConfig.h"

using namespace stratos;
namespace fs = std::filesystem;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

void printHelp() {
    std::cout << "Stratos Compiler v0.1.0\n\n";
    std::cout << "Usage:\n";
    std::cout << "  stratos <file.st>              Compile a single file\n";
    std::cout << "  stratos compile <file.st>      Compile a single file\n";
    std::cout << "  stratos compile <directory>    Compile all .st files in directory\n";
    std::cout << "  stratos build                  Build project (looks for stratos.conf)\n";
    std::cout << "  stratos build <project_dir>    Build project in specified directory\n";
    std::cout << "  stratos test                   Run test cases from cases/ directory\n";
    std::cout << "  stratos test --verbose         Run tests with detailed output\n";
    std::cout << "  stratos --help                 Show this help\n";
    std::cout << "  stratos --version              Show version\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>            Specify output file path\n";
    std::cout << "  -v, --verbose                  Enable verbose output\n";
}

void printVersion() {
    std::cout << "Stratos Compiler v0.1.0\n";
    std::cout << "LLVM Backend\n";
    std::cout << "Built: " << __DATE__ << " " << __TIME__ << "\n";
}

// ============================================================================
// COMPILATION FUNCTIONS
// ============================================================================

struct CompileResult {
    bool success;
    std::string errorMessage;
    double compilationTime;  // in milliseconds
};

CompileResult compileFile(const std::string& path, const std::string& outputPath = "", bool verbose = false) {
    CompileResult result;
    auto start = std::chrono::high_resolution_clock::now();

    if (verbose) {
        std::cout << "Compiling: " << path << std::endl;
    }

    // Read source file
    std::ifstream file(path);
    if (!file.is_open()) {
        result.success = false;
        result.errorMessage = "Could not open file: " + path;
        return result;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // Lexical Analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        if (verbose) std::cout << "  [Lexer]     OK (" << tokens.size() << " tokens)" << std::endl;

        // Parsing
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
        if (verbose) std::cout << "  [Parser]    OK (" << statements.size() << " statements)" << std::endl;

        // Semantic Analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(statements)) {
            result.success = false;
            result.errorMessage = "Semantic analysis failed";
            return result;
        }
        if (verbose) std::cout << "  [Semantics] OK" << std::endl;

        // Optimization
        Optimizer optimizer;
        optimizer.optimize(statements);
        if (verbose) std::cout << "  [Optimizer] Finished" << std::endl;

        // Code Generation
        std::string irPath = outputPath.empty() ? (path + ".ll") : outputPath;
        IRGenerator generator(irPath);
        generator.generate(statements);
        if (verbose) std::cout << "  [CodeGen]   Generated " << irPath << std::endl;

        result.success = true;

    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string(e.what());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    result.compilationTime = duration.count();

    return result;
}

int handleCompile(int argc, char* argv[]) {
    // Parse arguments
    std::string inputPath;
    std::string outputPath;
    bool verbose = false;

    int argStart = (std::string(argv[1]) == "compile") ? 2 : 1;

    if (argc <= argStart) {
        std::cerr << "Error: No input file specified\n\n";
        printHelp();
        return 1;
    }

    inputPath = argv[argStart];

    // Parse options
    for (int i = argStart + 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputPath = argv[++i];
            }
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    // Check if input is a file or directory
    if (fs::is_directory(inputPath)) {
        // Compile all .st files in directory
        std::cout << "Compiling all .st files in: " << inputPath << "\n" << std::endl;

        int successCount = 0;
        int failCount = 0;

        for (const auto& entry : fs::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".st") {
                CompileResult result = compileFile(entry.path().string(), "", verbose);

                if (result.success) {
                    successCount++;
                    if (!verbose) std::cout << ".";
                } else {
                    failCount++;
                    std::cerr << "\nError in " << entry.path().filename() << ": "
                              << result.errorMessage << std::endl;
                }
            }
        }

        if (!verbose) std::cout << "\n";
        std::cout << "\n==========================================\n";
        std::cout << "Compilation Results: " << successCount << " succeeded, "
                  << failCount << " failed\n";
        std::cout << "==========================================\n";

        return (failCount > 0) ? 1 : 0;

    } else if (fs::is_regular_file(inputPath)) {
        // Compile single file
        CompileResult result = compileFile(inputPath, outputPath, verbose);

        if (result.success) {
            if (!verbose) {
                std::cout << "Successfully compiled " << inputPath << " in "
                          << result.compilationTime << "ms" << std::endl;
            }
            return 0;
        } else {
            std::cerr << "Compilation failed: " << result.errorMessage << std::endl;
            return 1;
        }

    } else {
        std::cerr << "Error: Input path not found: " << inputPath << std::endl;
        return 1;
    }
}

// ============================================================================
// TEST RUNNER
// ============================================================================

struct TestResult {
    std::string testName;
    bool passed;
    std::string errorMessage;
    double executionTime;
};

int handleTest(int argc, char* argv[]) {
    bool verbose = false;

    // Parse options
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    std::string casesDir = "../../cases";  // Relative to build directory

    // Try alternative paths if default doesn't exist
    if (!fs::exists(casesDir)) {
        casesDir = "../cases";
    }
    if (!fs::exists(casesDir)) {
        casesDir = "cases";
    }

    std::cout << "Running Stratos Test Suite...\n";
    std::cout << "Test directory: " << casesDir << "\n\n";

    if (!fs::exists(casesDir) || !fs::is_directory(casesDir)) {
        std::cerr << "Error: Test cases directory not found: " << casesDir << std::endl;
        return 1;
    }

    std::vector<TestResult> results;
    int passed = 0;
    int failed = 0;

    for (const auto& entry : fs::directory_iterator(casesDir)) {
        if (entry.path().extension() == ".st") {
            TestResult result;
            result.testName = entry.path().filename().string();

            if (verbose) {
                std::cout << "Running " << result.testName << "... ";
            }

            CompileResult compileResult = compileFile(entry.path().string(), "", false);

            result.passed = compileResult.success;
            result.errorMessage = compileResult.errorMessage;
            result.executionTime = compileResult.compilationTime;

            results.push_back(result);

            if (result.passed) {
                passed++;
                if (verbose) {
                    std::cout << "✓ PASS (" << result.executionTime << "ms)\n";
                } else {
                    std::cout << ".";
                }
            } else {
                failed++;
                if (verbose) {
                    std::cout << "✗ FAIL\n";
                    std::cout << "  Error: " << result.errorMessage << "\n";
                } else {
                    std::cout << "F";
                }
            }
        }
    }

    if (!verbose) std::cout << "\n";

    std::cout << "\n==========================================\n";
    std::cout << "Test Results: " << passed << " passed, " << failed << " failed\n";
    std::cout << "Total: " << (passed + failed) << " tests\n";
    std::cout << "==========================================\n";

    // Show failed tests summary
    if (failed > 0) {
        std::cout << "\nFailed Tests:\n";
        for (const auto& result : results) {
            if (!result.passed) {
                std::cout << "  ✗ " << result.testName << "\n";
                std::cout << "    " << result.errorMessage << "\n";
            }
        }
    }

    return (failed > 0) ? 1 : 0;
}

// ============================================================================
// PROJECT BUILD
// ============================================================================

CompileResult compileMultipleFiles(const std::vector<std::string>& files, const std::string& outputPath, bool verbose) {
    CompileResult result;
    auto start = std::chrono::high_resolution_clock::now();

    // Collect all statements from all files
    std::vector<std::unique_ptr<Stmt>> allStatements;

    for (const auto& file : files) {
        if (verbose) std::cout << "  Processing: " << file << std::endl;

        std::ifstream inFile(file);
        if (!inFile.is_open()) {
            result.success = false;
            result.errorMessage = "Could not open file: " + file;
            return result;
        }

        std::stringstream buffer;
        buffer << inFile.rdbuf();
        std::string source = buffer.str();

        try {
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();

            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

            // Move statements to combined list
            for (auto& stmt : statements) {
                allStatements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            result.success = false;
            result.errorMessage = "Error in " + file + ": " + e.what();
            return result;
        }
    }

    try {
        // Semantic Analysis on all statements
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(allStatements)) {
            result.success = false;
            result.errorMessage = "Semantic analysis failed";
            return result;
        }

        // Optimization
        Optimizer optimizer;
        optimizer.optimize(allStatements);

        // Code Generation
        IRGenerator generator(outputPath);
        generator.generate(allStatements);

        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.errorMessage = std::string(e.what());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    result.compilationTime = duration.count();

    return result;
}

int handleBuild(int argc, char* argv[]) {
    std::string projectDir = ".";
    bool verbose = false;

    // Parse options
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (!arg.starts_with("-")) {
            projectDir = arg;
        }
    }

    // Look for stratos.conf
    std::string configPath = projectDir + "/stratos.conf";
    if (!fs::exists(configPath)) {
        std::cerr << "Error: No stratos.conf found in " << projectDir << std::endl;
        return 1;
    }

    // Parse config
    auto configOpt = ProjectConfigParser::parse(configPath);
    if (!configOpt) {
        std::cerr << "Error: Failed to parse stratos.conf" << std::endl;
        return 1;
    }

    ProjectConfig config = *configOpt;
    std::string projectRoot = ProjectConfigParser::getProjectRoot(configPath);

    std::cout << "Building project: " << config.name << " v" << config.version << "\n";
    if (verbose) {
        std::cout << "Project root: " << projectRoot << "\n";
        std::cout << "Project type: " << config.type << "\n";
    }

    // Determine source files
    std::vector<std::string> sourceFiles;

    if (!config.sources.empty()) {
        // Use explicit source list from config
        for (const auto& src : config.sources) {
            std::string fullPath = projectRoot + "/" + src;
            if (fs::exists(fullPath)) {
                sourceFiles.push_back(fullPath);
            } else {
                std::cerr << "Warning: Source file not found: " << fullPath << std::endl;
            }
        }
    } else if (!config.entry.empty()) {
        // Use entry point
        sourceFiles.push_back(projectRoot + "/" + config.entry);
    } else {
        std::cerr << "Error: No entry point or source files specified in config" << std::endl;
        return 1;
    }

    if (sourceFiles.empty()) {
        std::cerr << "Error: No source files to compile" << std::endl;
        return 1;
    }

    // Determine output path
    std::string outputPath;
    if (!config.output.empty()) {
        outputPath = projectRoot + "/" + config.output + ".ll";
    } else {
        outputPath = projectRoot + "/build/" + config.name + ".ll";
    }

    // Ensure output directory exists
    fs::path outPath(outputPath);
    fs::create_directories(outPath.parent_path());

    // Compile
    std::cout << "Compiling " << sourceFiles.size() << " file(s)...\n";
    CompileResult result = compileMultipleFiles(sourceFiles, outputPath, verbose);

    if (result.success) {
        std::cout << "✓ Build successful in " << result.compilationTime << "ms\n";
        std::cout << "Output: " << outputPath << "\n";
        return 0;
    } else {
        std::cerr << "✗ Build failed: " << result.errorMessage << std::endl;
        return 1;
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error: No input file or command specified.\n\n";
        printHelp();
        return 1;
    }

    std::string command = argv[1];

    // Handle flags
    if (command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    if (command == "--version") {
        printVersion();
        return 0;
    }

    // Handle subcommands
    if (command == "test") {
        return handleTest(argc, argv);
    }

    if (command == "build") {
        return handleBuild(argc, argv);
    }

    if (command == "compile" || command.ends_with(".st")) {
        return handleCompile(argc, argv);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printHelp();
    return 1;
}
