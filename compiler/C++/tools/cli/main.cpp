// Stratos CLI Tool - Project Management
// Build this as a separate executable: stratoscli or stratos-cli

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <ctime>

namespace fs = std::filesystem;

// ============================================================================
// TEMPLATES
// ============================================================================

const std::string TEMPLATE_MAIN_ST = R"(// {PROJECT_NAME} - Main Entry Point

fn main() {
    print("Hello, Stratos!");
}
)";

const std::string TEMPLATE_TEST_ST = R"(// Tests for {PROJECT_NAME}

fn testExample() {
    val result = 2 + 2;
    // TODO: Add assertions when test framework is ready
    print("Test passed!");
}
)";

const std::string TEMPLATE_STRATOS_TOML = R"([package]
name = "{PROJECT_NAME}"
version = "0.1.0"
authors = ["Your Name <you@example.com>"]
edition = "2024"

[dependencies]
# Add dependencies here when package manager is available

[build]
optimization = "release"  # debug, release
target = "native"
)";

const std::string TEMPLATE_README = R"(# {PROJECT_NAME}

A Stratos project.

## Project Structure

```
{PROJECT_NAME}/
├── stratos.toml      # Project configuration
├── src/
│   └── main.st      # Main entry point
├── tests/
│   └── main_test.st # Test file
└── README.md
```

## Building

```bash
stratos compile src/main.st
```

## Running Tests

```bash
stratos test
```

## Development

Edit `src/main.st` to start coding!

## License

Add your license here.
)";

const std::string TEMPLATE_GITIGNORE = R"(# Build artifacts
*.ll
*.o
*.out
*.exe

# IDE
.vscode/
.idea/
*.swp
*.swo

# OS
.DS_Store
Thumbs.db
)";

// ============================================================================
// PROJECT CREATION
// ============================================================================

std::string replaceTemplate(const std::string& template_str, const std::string& projectName) {
    std::string result = template_str;
    size_t pos = 0;
    const std::string placeholder = "{PROJECT_NAME}";

    while ((pos = result.find(placeholder, pos)) != std::string::npos) {
        result.replace(pos, placeholder.length(), projectName);
        pos += projectName.length();
    }

    return result;
}

void createFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not create file: " << path << std::endl;
        return;
    }
    file << content;
    file.close();
}

bool createProject(const std::string& projectName, const std::string& basePath = ".") {
    std::string projectPath = (basePath == ".") ? projectName : basePath;

    std::cout << "Creating new Stratos project: " << projectName << "\n";

    try {
        // Create directory structure
        if (basePath != ".") {
            if (fs::exists(projectPath)) {
                std::cerr << "Error: Directory already exists: " << projectPath << std::endl;
                return false;
            }
            fs::create_directory(projectPath);
        }

        fs::create_directory(projectPath + "/src");
        fs::create_directory(projectPath + "/tests");

        std::cout << "  ✓ Created directory structure\n";

        // Create stratos.toml
        std::string tomlContent = replaceTemplate(TEMPLATE_STRATOS_TOML, projectName);
        createFile(projectPath + "/stratos.toml", tomlContent);
        std::cout << "  ✓ Created stratos.toml\n";

        // Create main.st
        std::string mainContent = replaceTemplate(TEMPLATE_MAIN_ST, projectName);
        createFile(projectPath + "/src/main.st", mainContent);
        std::cout << "  ✓ Created src/main.st\n";

        // Create test file
        std::string testContent = replaceTemplate(TEMPLATE_TEST_ST, projectName);
        createFile(projectPath + "/tests/main_test.st", testContent);
        std::cout << "  ✓ Created tests/main_test.st\n";

        // Create README
        std::string readmeContent = replaceTemplate(TEMPLATE_README, projectName);
        createFile(projectPath + "/README.md", readmeContent);
        std::cout << "  ✓ Created README.md\n";

        // Create .gitignore
        createFile(projectPath + "/.gitignore", TEMPLATE_GITIGNORE);
        std::cout << "  ✓ Created .gitignore\n";

        std::cout << "\n✓ Project created successfully!\n\n";
        std::cout << "Next steps:\n";
        if (basePath != ".") {
            std::cout << "  cd " << projectName << "\n";
        }
        std::cout << "  stratos compile src/main.st\n";
        std::cout << "  # Edit src/main.st to start coding!\n";

        return true;

    } catch (const std::exception& e) {
        std::cerr << "Error creating project: " << e.what() << std::endl;
        return false;
    }
}

// ============================================================================
// TEMPLATES
// ============================================================================

void showTemplates() {
    std::cout << "Available project templates:\n\n";
    std::cout << "  basic      Default project with main.st\n";
    std::cout << "  lib        Library project\n";
    std::cout << "  cli        Command-line application\n";
    std::cout << "  webapp     Web application (future)\n";
    std::cout << "\nUsage: stratoscli new <name> --template <template>\n";
}

// ============================================================================
// HELP & VERSION
// ============================================================================

void printHelp() {
    std::cout << "Stratos CLI Tool v0.1.0\n";
    std::cout << "Project management and scaffolding\n\n";
    std::cout << "Usage:\n";
    std::cout << "  stratoscli new <name>           Create a new project\n";
    std::cout << "  stratoscli init                 Initialize project in current directory\n";
    std::cout << "  stratoscli templates            List available templates\n";
    std::cout << "  stratoscli --help               Show this help\n";
    std::cout << "  stratoscli --version            Show version\n\n";
    std::cout << "Options:\n";
    std::cout << "  --template <name>               Use a specific template\n\n";
    std::cout << "Examples:\n";
    std::cout << "  stratoscli new my-app           Create new project 'my-app'\n";
    std::cout << "  stratoscli init                 Initialize in current directory\n";
}

void printVersion() {
    std::cout << "Stratos CLI Tool v0.1.0\n";
    std::cout << "Project scaffolding and management\n";
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    std::string command = argv[1];

    // Handle flags
    if (command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    if (command == "--version" || command == "-v") {
        printVersion();
        return 0;
    }

    // Handle commands
    if (command == "new") {
        if (argc < 3) {
            std::cerr << "Error: Project name required\n";
            std::cerr << "Usage: stratoscli new <name>\n";
            return 1;
        }

        std::string projectName = argv[2];

        // Validate project name
        if (projectName.empty() || projectName[0] == '-') {
            std::cerr << "Error: Invalid project name\n";
            return 1;
        }

        bool success = createProject(projectName);
        return success ? 0 : 1;
    }

    if (command == "init") {
        std::string currentDir = fs::current_path().filename().string();

        std::cout << "Initializing Stratos project in current directory...\n";
        std::cout << "Project name will be: " << currentDir << "\n\n";

        bool success = createProject(currentDir, ".");
        return success ? 0 : 1;
    }

    if (command == "templates") {
        showTemplates();
        return 0;
    }

    std::cerr << "Unknown command: " << command << "\n\n";
    printHelp();
    return 1;
}
