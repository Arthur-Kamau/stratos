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
#include "stratos/NativeRegistry.h"
#include "stratos/TypeSystem.h"
#include "stratos/AsyncRuntime.h"
#include "stratos/Interpreter.h"
#include "stratos/DependencyManager.h"
#include "stratos/LockFile.h"
#include "stratos/Formatter.h"

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
    std::cout << "  stratos run <file.st>          Execute a Stratos program directly\n";
    std::cout << "  stratos check <file.st>        Parse and analyze without code generation\n";
    std::cout << "  stratos check <directory>      Check all .st files in directory\n";
    std::cout << "  stratos fmt <file.st>          Format a Stratos source file\n";
    std::cout << "  stratos fmt <directory> -w     Format all .st files in directory\n";
    std::cout << "  stratos build                  Build project (looks for stratos.conf)\n";
    std::cout << "  stratos build <project_dir>    Build project in specified directory\n";
    std::cout << "  stratos new <project-name>     Create a new Stratos project\n";
    std::cout << "  stratos get                    Fetch all dependencies from stratos.conf\n";
    std::cout << "  stratos get <url>              Fetch a dependency from URL\n";
    std::cout << "  stratos get --update           Update all dependencies and regenerate lock file\n";
    std::cout << "  stratos get --verify           Verify lock file matches installed dependencies\n";
    std::cout << "  stratos test                   Run test cases from cases/ directory\n";
    std::cout << "  stratos test --verbose         Run tests with detailed output\n";
    std::cout << "  stratos --help                 Show this help\n";
    std::cout << "  stratos --version              Show version\n\n";
    std::cout << "Dependency URL formats:\n";
    std::cout << "  github.com/user/repo@v1.0.0    GitHub with version tag\n";
    std::cout << "  github.com/user/repo@main      GitHub with branch\n";
    std::cout << "  github.com/user/repo@abc123    GitHub with commit hash\n";
    std::cout << "  https://github.com/user/repo   Full GitHub URL\n";
    std::cout << "  path:../local-lib              Local directory\n\n";
    std::cout << "Options:\n";
    std::cout << "  -o, --output <file>            Specify output file path\n";
    std::cout << "  -v, --verbose                  Enable verbose output\n";
    std::cout << "  -r, --run                      Execute program instead of compiling\n";
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

CompileResult compileFile(const std::string& path, const std::string& outputPath = "", bool verbose = false, bool run = false) {
    CompileResult result;
    auto start = std::chrono::high_resolution_clock::now();

    if (verbose) {
        if (run) {
            std::cout << "Executing: " << path << std::endl;
        } else {
            std::cout << "Compiling: " << path << std::endl;
        }
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

        if (run) {
            // Execute directly with interpreter
            if (verbose) std::cout << "  [Executing...]" << std::endl;

            Interpreter interpreter;

            // Process all declarations first (transfers ownership to interpreter)
            interpreter.execute(std::move(statements));

            // Call main function if it exists
            try {
                std::vector<RuntimeValue> emptyArgs;
                interpreter.callFunction("main", emptyArgs);
                if (verbose) std::cout << "  [Execution] Complete" << std::endl;
            } catch (const ReturnException& e) {
                // Return from main() is normal - not an error
                if (verbose) std::cout << "  [Execution] Complete" << std::endl;
            } catch (const std::runtime_error& e) {
                // If main doesn't exist, that's okay - declarations were already executed
                std::string err = e.what();
                if (err.find("Undefined function: main") == std::string::npos) {
                    // Only throw if it's not a "main not found" error
                    throw std::runtime_error("Error executing main(): " + err);
                }
                if (verbose) std::cout << "  [Execution] Complete (no main function)" << std::endl;
            }

        } else {
            // Optimization
            Optimizer optimizer;
            optimizer.optimize(statements);
            if (verbose) std::cout << "  [Optimizer] Finished" << std::endl;

            // Code Generation
            std::string irPath = outputPath.empty() ? (path + ".ll") : outputPath;
            IRGenerator generator(irPath);
            generator.generate(statements);
            if (verbose) std::cout << "  [CodeGen]   Generated " << irPath << std::endl;
        }

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
// RUN (INTERPRETER)
// ============================================================================

// Helper function to find stratos.conf in current or parent directories
std::optional<std::string> findProjectConfig(const std::string& startDir = ".") {
    fs::path currentPath = fs::absolute(startDir);

    // Search up to 5 levels up
    for (int i = 0; i < 5; i++) {
        fs::path configPath = currentPath / "stratos.conf";
        if (fs::exists(configPath)) {
            return configPath.string();
        }

        // Move to parent directory
        fs::path parent = currentPath.parent_path();
        if (parent == currentPath) {
            break; // Reached root
        }
        currentPath = parent;
    }

    return std::nullopt;
}

// Helper function to resolve entry point file
std::optional<std::string> resolveEntryPoint(const std::string& inputPath) {
    // If the input path exists as-is and is a file, use it
    if (fs::is_regular_file(inputPath)) {
        return inputPath;
    }

    // If the input path is a directory, look inside it
    if (fs::is_directory(inputPath)) {
        fs::path dirPath = inputPath;

        // Look for stratos.conf in the directory
        fs::path configPath = dirPath / "stratos.conf";
        if (fs::exists(configPath)) {
            auto configOpt = ProjectConfigParser::parse(configPath.string());
            if (configOpt && !configOpt->entry.empty()) {
                fs::path entryPath = dirPath / configOpt->entry;
                if (fs::is_regular_file(entryPath)) {
                    return entryPath.string();
                }
            }
        }

        // Try main.st in the directory
        fs::path mainPath = dirPath / "main.st";
        if (fs::is_regular_file(mainPath)) {
            return mainPath.string();
        }

        // Try src/main.st in the directory
        fs::path srcMainPath = dirPath / "src" / "main.st";
        if (fs::is_regular_file(srcMainPath)) {
            return srcMainPath.string();
        }
    }

    // Try adding .st extension
    std::string withExtension = inputPath + ".st";
    if (fs::is_regular_file(withExtension)) {
        return withExtension;
    }

    // Look for stratos.conf in current directory
    auto configPathOpt = findProjectConfig();
    if (configPathOpt) {
        auto configOpt = ProjectConfigParser::parse(*configPathOpt);
        if (configOpt) {
            // Check if entry is set
            if (!configOpt->entry.empty()) {
                fs::path entryPath = fs::path(*configPathOpt).parent_path() / configOpt->entry;
                if (fs::is_regular_file(entryPath)) {
                    return entryPath.string();
                }
            }
        }
    }

    // Try main.st in current directory
    if (fs::is_regular_file("main.st")) {
        return "main.st";
    }

    // Try main.st in src/ directory
    if (fs::is_regular_file("src/main.st")) {
        return "src/main.st";
    }

    return std::nullopt;
}

int handleRun(int argc, char* argv[]) {
    // Parse arguments
    std::string inputPath;
    bool verbose = false;

    if (argc < 3) {
        std::cerr << "Error: No input file specified\n\n";
        printHelp();
        return 1;
    }

    inputPath = argv[2];

    // Parse options
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    // Resolve the entry point file
    auto resolvedPathOpt = resolveEntryPoint(inputPath);
    if (!resolvedPathOpt) {
        std::cerr << "Error: Could not find entry point file.\n";
        std::cerr << "Searched for:\n";
        std::cerr << "  - " << inputPath << "\n";
        if (fs::is_directory(inputPath)) {
            std::cerr << "  - " << inputPath << "/stratos.conf entry\n";
            std::cerr << "  - " << inputPath << "/main.st\n";
            std::cerr << "  - " << inputPath << "/src/main.st\n";
        } else {
            std::cerr << "  - " << inputPath << ".st\n";
        }
        std::cerr << "  - Entry point from stratos.conf (current dir)\n";
        std::cerr << "  - main.st (current dir)\n";
        std::cerr << "  - src/main.st (current dir)\n";
        return 1;
    }

    std::string resolvedPath = *resolvedPathOpt;

    if (verbose) {
        std::cout << "Resolved entry point: " << resolvedPath << std::endl;
    }

    // Execute the file
    CompileResult result = compileFile(resolvedPath, "", verbose, true); // run=true

    if (!result.success) {
        std::cerr << "Execution failed: " << result.errorMessage << std::endl;
        return 1;
    }

    return 0;
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

// ============================================================================
// DEPENDENCY MANAGEMENT
// ============================================================================

int handleGet(int argc, char* argv[]) {
    bool verbose = false;
    bool update = false;
    bool verify = false;
    std::string url;

    // Parse options
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "--update") {
            update = true;
        } else if (arg == "--verify") {
            verify = true;
        } else if (!arg.starts_with("-")) {
            url = arg;
        }
    }

    // Get current directory as project root
    std::string projectRoot = fs::current_path().string();

    // Handle --verify flag
    if (verify) {
        bool success = LockFileManager::verify(projectRoot);
        return success ? 0 : 1;
    }

    DependencyManager depMgr(projectRoot);

    // Handle --update flag
    if (update) {
        std::cout << "Updating all dependencies...\n" << std::endl;
        bool success = depMgr.updateAllDependencies(verbose);

        if (success) {
            // Generate lock file after successful update
            std::cout << "\nGenerating lock file...\n";
            LockFileManager::generate(projectRoot);
        }

        return success ? 0 : 1;
    }

    // Handle normal get operations
    if (url.empty()) {
        // No URL provided - fetch all dependencies from stratos.conf
        std::cout << "Fetching all dependencies from stratos.conf...\n" << std::endl;
        bool success = depMgr.fetchAllDependencies(verbose);

        if (success) {
            // Generate lock file after successful fetch
            std::cout << "\nGenerating lock file...\n";
            LockFileManager::generate(projectRoot);
        }

        return success ? 0 : 1;
    } else {
        // Fetch specific dependency
        bool success = depMgr.fetchDependency(url, verbose);
        return success ? 0 : 1;
    }
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
// PROJECT CREATION - stratos new
// ============================================================================

int handleNew(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Error: No project name specified.\n";
        std::cerr << "Usage: stratos new <project-name>\n";
        std::cerr << "Example: stratos new my-app\n";
        return 1;
    }

    std::string projectName = argv[2];
    std::cout << "Creating new Stratos project: " << projectName << "\n";

    // Validate project name
    if (projectName.empty() || projectName[0] == '-') {
        std::cerr << "Error: Invalid project name\n";
        return 1;
    }

    // Check if directory already exists
    if (fs::exists(projectName)) {
        std::cerr << "Error: Directory '" << projectName << "' already exists\n";
        return 1;
    }

    // Create project structure
    try {
        fs::create_directory(projectName);
        fs::create_directory(projectName + "/src");
        fs::create_directory(projectName + "/build");
        fs::create_directory(projectName + "/libs");
        fs::create_directory(projectName + "/tests");

        std::cout << "Created directory structure:\n";
        std::cout << "  " << projectName << "/\n";
        std::cout << "  ├── src/\n";
        std::cout << "  ├── build/\n";
        std::cout << "  ├── libs/\n";
        std::cout << "  ├── tests/\n";
        std::cout << "  ├── stratos.conf\n";
        std::cout << "  ├── README.md\n";
        std::cout << "  └── .gitignore\n\n";

        // Create stratos.conf
        std::ofstream confFile(projectName + "/stratos.conf");
        confFile << "[project]\n";
        confFile << "name = " << projectName << "\n";
        confFile << "version = 1.0.0\n";
        confFile << "author = Your Name\n";
        confFile << "license = MIT\n\n";
        confFile << "[build]\n";
        confFile << "entry = src/main.st\n";
        confFile << "output = build/" << projectName << "\n\n";
        confFile << "[compile]\n";
        confFile << "sources = src/main.st\n\n";
        confFile << "[dependencies]\n";
        confFile << "# Add dependencies here\n";
        confFile << "# example = \"libs/example\"\n";
        confFile.close();

        // Create main.st
        std::ofstream mainFile(projectName + "/src/main.st");
        mainFile << "package main;\n\n";
        mainFile << "use log;\n";
        mainFile << "use math;\n\n";
        mainFile << "fn main() {\n";
        mainFile << "    log.info(\"Hello from " << projectName << "!\");\n";
        mainFile << "    \n";
        mainFile << "    val result = math.sqrt(16.0);\n";
        mainFile << "    log.info(\"Square root of 16 is: \" + result);\n";
        mainFile << "}\n";
        mainFile.close();

        // Create README.md
        std::ofstream readmeFile(projectName + "/README.md");
        readmeFile << "# " << projectName << "\n\n";
        readmeFile << "A Stratos project.\n\n";
        readmeFile << "## Building\n\n";
        readmeFile << "```bash\n";
        readmeFile << "cd " << projectName << "\n";
        readmeFile << "stratos build\n";
        readmeFile << "```\n\n";
        readmeFile << "## Running\n\n";
        readmeFile << "```bash\n";
        readmeFile << "./build/" << projectName << "\n";
        readmeFile << "```\n\n";
        readmeFile << "## Testing\n\n";
        readmeFile << "```bash\n";
        readmeFile << "stratos test\n";
        readmeFile << "```\n";
        readmeFile.close();

        // Create .gitignore
        std::ofstream gitignoreFile(projectName + "/.gitignore");
        gitignoreFile << "build/\n";
        gitignoreFile << "*.ll\n";
        gitignoreFile << "*.o\n";
        gitignoreFile << "*.exe\n";
        gitignoreFile << ".vscode/\n";
        gitignoreFile << ".idea/\n";
        gitignoreFile.close();

        std::cout << "✓ Project created successfully!\n\n";
        std::cout << "Next steps:\n";
        std::cout << "  cd " << projectName << "\n";
        std::cout << "  stratos build\n";
        std::cout << "  ./build/" << projectName << "\n\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error creating project: " << e.what() << std::endl;
        return 1;
    }
}

int handleFmt(int argc, char* argv[]) {
    // Parse arguments
    std::vector<std::string> inputPaths;
    bool writeToFile = false;
    bool checkOnly = false;
    bool verbose = false;

    if (argc < 3) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: stratos fmt <file.st> [options]\n";
        std::cerr << "       stratos fmt <directory> [options]\n";
        std::cerr << "Options:\n";
        std::cerr << "  -w, --write    Write formatted code back to file (in-place)\n";
        std::cerr << "  --check        Check if files are formatted (exit 1 if not)\n";
        std::cerr << "  -v, --verbose  Verbose output\n";
        return 1;
    }

    // Parse arguments
    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-w" || arg == "--write") {
            writeToFile = true;
        } else if (arg == "--check") {
            checkOnly = true;
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            inputPaths.push_back(arg);
        }
    }

    if (inputPaths.empty()) {
        std::cerr << "Error: No input file specified\n";
        return 1;
    }

    if (writeToFile && checkOnly) {
        std::cerr << "Error: Cannot use --write and --check together\n";
        return 1;
    }

    int filesFormatted = 0;
    int filesChecked = 0;
    int filesNeedFormatting = 0;

    auto formatFile = [&](const std::string& filePath) -> bool {
        if (verbose) {
            std::cout << "Processing: " << filePath << "\n";
        }

        // Read source file
        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::cerr << "✗ Could not open file: " << filePath << "\n";
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();
        file.close();

        try {
            // Lexical Analysis
            Lexer lexer(source);
            std::vector<Token> tokens = lexer.scanTokens();

            // Parsing
            Parser parser(tokens);
            std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

            // Format
            Formatter formatter;
            std::string formattedCode = formatter.format(statements);

            if (checkOnly) {
                // Check if formatted code differs from original
                if (formattedCode != source) {
                    std::cout << "✗ " << filePath << " is not formatted\n";
                    filesNeedFormatting++;
                    return false;
                } else {
                    if (verbose) {
                        std::cout << "✓ " << filePath << " is formatted\n";
                    }
                    filesChecked++;
                    return true;
                }
            } else if (writeToFile) {
                // Write formatted code back to file
                std::ofstream outFile(filePath);
                if (!outFile.is_open()) {
                    std::cerr << "✗ Could not write to file: " << filePath << "\n";
                    return false;
                }
                outFile << formattedCode;
                outFile.close();

                std::cout << "✓ Formatted: " << filePath << "\n";
                filesFormatted++;
                return true;
            } else {
                // Print formatted code to stdout
                std::cout << formattedCode;
                return true;
            }

        } catch (const std::exception& e) {
            std::cerr << "✗ Error formatting " << filePath << ": " << e.what() << "\n";
            return false;
        }
    };

    // Process each input path
    for (const auto& inputPath : inputPaths) {
        if (fs::is_directory(inputPath)) {
            if (!checkOnly && !writeToFile) {
                std::cerr << "Error: Cannot print multiple files to stdout\n";
                std::cerr << "Use --write or --check when formatting directories\n";
                return 1;
            }

            if (verbose) {
                std::cout << "Formatting all .st files in: " << inputPath << "\n";
            }

            for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
                if (entry.path().extension() == ".st") {
                    formatFile(entry.path().string());
                }
            }
        } else {
            // Single file
            formatFile(inputPath);
        }
    }

    // Print summary for --write or --check
    if (writeToFile && filesFormatted > 0) {
        std::cout << "\nFormatted " << filesFormatted << " file(s)\n";
    }

    if (checkOnly) {
        std::cout << "\n========================================\n";
        std::cout << "Format Check Summary\n";
        std::cout << "========================================\n";
        std::cout << "Files checked:       " << (filesChecked + filesNeedFormatting) << "\n";
        std::cout << "Already formatted:   " << filesChecked << "\n";
        std::cout << "Need formatting:     " << filesNeedFormatting << "\n";

        if (filesNeedFormatting > 0) {
            std::cout << "\nSome files are not formatted.\n";
            std::cout << "Run: stratos fmt <file> -w to format them\n";
            return 1;
        } else {
            std::cout << "\nAll files are formatted! ✓\n";
            return 0;
        }
    }

    return 0;
}

int handleCheck(int argc, char* argv[]) {
    // Parse arguments
    std::string inputPath;
    bool verbose = false;

    if (argc < 3) {
        std::cerr << "Error: No input file specified\n";
        std::cerr << "Usage: stratos check <file.st>\n";
        std::cerr << "       stratos check <directory>\n";
        return 1;
    }

    inputPath = argv[2];

    // Parse options
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
    }

    // Check if input is a directory
    if (fs::is_directory(inputPath)) {
        std::cout << "Checking all .st files in: " << inputPath << "\n";

        int totalFiles = 0;
        int successCount = 0;
        int failCount = 0;

        for (const auto& entry : fs::recursive_directory_iterator(inputPath)) {
            if (entry.path().extension() == ".st") {
                totalFiles++;
                std::string filePath = entry.path().string();

                if (verbose) {
                    std::cout << "\n[" << totalFiles << "] Checking: " << filePath << "\n";
                }

                // Read source file
                std::ifstream file(filePath);
                if (!file.is_open()) {
                    std::cerr << "✗ Could not open file: " << filePath << "\n";
                    failCount++;
                    continue;
                }

                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string source = buffer.str();

                try {
                    // Lexical Analysis
                    Lexer lexer(source);
                    std::vector<Token> tokens = lexer.scanTokens();
                    if (verbose) std::cout << "  ✓ Lexer OK (" << tokens.size() << " tokens)\n";

                    // Parsing
                    Parser parser(tokens);
                    std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
                    if (verbose) std::cout << "  ✓ Parser OK (" << statements.size() << " statements)\n";

                    // Semantic Analysis
                    SemanticAnalyzer analyzer;
                    if (!analyzer.analyze(statements)) {
                        std::cerr << "✗ " << filePath << ": Semantic analysis failed\n";
                        failCount++;
                        continue;
                    }
                    if (verbose) std::cout << "  ✓ Semantics OK\n";

                    std::cout << "✓ " << filePath << "\n";
                    successCount++;

                } catch (const std::exception& e) {
                    std::cerr << "✗ " << filePath << ": " << e.what() << "\n";
                    failCount++;
                }
            }
        }

        std::cout << "\n========================================\n";
        std::cout << "Check Summary\n";
        std::cout << "========================================\n";
        std::cout << "Total files:    " << totalFiles << "\n";
        std::cout << "Passed:         " << successCount << "\n";
        std::cout << "Failed:         " << failCount << "\n";

        if (failCount == 0) {
            std::cout << "\nAll files passed! ✓\n";
            return 0;
        } else {
            std::cout << "\nSome files failed.\n";
            return 1;
        }
    }

    // Single file check
    if (verbose) {
        std::cout << "Checking: " << inputPath << "\n";
    }

    // Read source file
    std::ifstream file(inputPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << inputPath << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // Lexical Analysis
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.scanTokens();
        if (verbose) std::cout << "  ✓ Lexer OK (" << tokens.size() << " tokens)\n";

        // Parsing
        Parser parser(tokens);
        std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
        if (verbose) std::cout << "  ✓ Parser OK (" << statements.size() << " statements)\n";

        // Semantic Analysis
        SemanticAnalyzer analyzer;
        if (!analyzer.analyze(statements)) {
            std::cerr << "✗ Semantic analysis failed\n";
            return 1;
        }
        if (verbose) std::cout << "  ✓ Semantics OK\n";

        std::cout << "✓ " << inputPath << " is valid\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "✗ Error: " << e.what() << "\n";
        return 1;
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    // Initialize runtime systems
    NativeRegistry::getInstance().initializeStdlib();

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

    if (command == "run") {
        return handleRun(argc, argv);
    }

    if (command == "check") {
        return handleCheck(argc, argv);
    }

    if (command == "fmt") {
        return handleFmt(argc, argv);
    }

    if (command == "get") {
        return handleGet(argc, argv);
    }

    if (command == "new") {
        return handleNew(argc, argv);
    }

    if (command == "compile" || command.ends_with(".st")) {
        return handleCompile(argc, argv);
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printHelp();
    return 1;
}
