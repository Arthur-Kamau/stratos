#include "stratos/MarkdownDocGenerator.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace stratos {

MarkdownDocGenerator::MarkdownDocGenerator() {}

void MarkdownDocGenerator::generate(const Documentation& doc, const std::string& outputDir) {
    // Create output directory
    fs::create_directories(outputDir);

    // Generate index
    generateIndex(doc, outputDir);

    // Generate each package
    for (const auto& pkg : doc.packages) {
        generatePackage(*pkg, outputDir);
    }
}

void MarkdownDocGenerator::generateIndex(const Documentation& doc, const std::string& outputDir) {
    md_.str("");
    md_.clear();

    md_ << "# " << (doc.projectName.empty() ? "Stratos Documentation" : doc.projectName) << "\n\n";

    if (!doc.version.empty()) {
        md_ << "**Version:** " << doc.version << "\n\n";
    }

    md_ << "## Packages\n\n";

    for (const auto& pkg : doc.packages) {
        md_ << "- [" << pkg->name << "](" << pkg->name << ".md)";
        if (!pkg->doc.summary.empty()) {
            md_ << " - " << pkg->doc.summary;
        }
        md_ << "\n";
    }

    // Write to file
    std::ofstream file(outputDir + "/README.md");
    file << md_.str();
    file.close();
}

void MarkdownDocGenerator::generatePackage(const DocPackage& pkg, const std::string& outputDir) {
    md_.str("");
    md_.clear();

    md_ << "# Package: " << pkg.name << "\n\n";

    // Package documentation
    if (!pkg.doc.summary.empty()) {
        writeDocComment(pkg.doc);
        md_ << "\n";
    }

    // Dependencies
    if (!pkg.dependencies.empty()) {
        md_ << "## Dependencies\n\n";
        for (const auto& dep : pkg.dependencies) {
            md_ << "- " << dep << "\n";
        }
        md_ << "\n";
    }

    // Functions
    if (!pkg.functions.empty()) {
        md_ << "## Functions\n\n";
        for (const auto& func : pkg.functions) {
            writeFunction(*func);
            md_ << "\n---\n\n";
        }
    }

    // Classes
    if (!pkg.classes.empty()) {
        md_ << "## Classes\n\n";
        for (const auto& cls : pkg.classes) {
            writeClass(*cls);
            md_ << "\n---\n\n";
        }
    }

    // Variables
    if (!pkg.variables.empty()) {
        md_ << "## Variables\n\n";
        for (const auto& var : pkg.variables) {
            writeVariable(*var);
            md_ << "\n";
        }
    }

    // Write to file
    std::ofstream file(outputDir + "/" + pkg.name + ".md");
    file << md_.str();
    file.close();
}

void MarkdownDocGenerator::writeFunction(const DocFunction& func) {
    // Function signature
    md_ << "### `" << func.name << "(";

    for (size_t i = 0; i < func.paramNames.size(); i++) {
        if (i > 0) md_ << ", ";
        md_ << func.paramNames[i];
        if (i < func.paramTypes.size()) {
            md_ << ": " << func.paramTypes[i];
        }
    }

    md_ << ") " << func.returnType << "`\n\n";

    // Native indicator
    if (func.isNative) {
        md_ << "*Native function*\n\n";
    }

    // Documentation
    if (!func.doc.summary.empty()) {
        md_ << func.doc.description << "\n\n";
    }

    // Parameters
    if (!func.doc.params.empty()) {
        md_ << "**Parameters:**\n\n";
        for (const auto& param : func.doc.params) {
            md_ << "- `" << param.name << "`";
            if (!param.type.empty()) {
                md_ << " (" << param.type << ")";
            }
            if (!param.description.empty()) {
                md_ << " - " << param.description;
            }
            md_ << "\n";
        }
        md_ << "\n";
    }

    // Return value
    if (!func.doc.returnDoc.empty()) {
        md_ << "**Returns:** `" << func.returnType << "` - " << func.doc.returnDoc << "\n\n";
    }

    // Examples
    if (!func.doc.examples.empty()) {
        md_ << "**Examples:**\n\n";
        for (const auto& example : func.doc.examples) {
            md_ << "```stratos\n" << example << "\n```\n\n";
        }
    }

    // Source location
    md_ << "**Source:** [" << func.sourceFile << ":" << func.sourceLine << "](" << func.sourceFile << "#L" << func.sourceLine << ")\n";
}

void MarkdownDocGenerator::writeClass(const DocClass& cls) {
    md_ << "### Class: `" << cls.name << "`\n\n";

    // Superclass
    if (!cls.superclass.empty()) {
        md_ << "**Extends:** `" << cls.superclass << "`\n\n";
    }

    // Documentation
    if (!cls.doc.summary.empty()) {
        md_ << cls.doc.description << "\n\n";
    }

    // Properties
    if (!cls.properties.empty()) {
        md_ << "**Properties:**\n\n";
        for (const auto& prop : cls.properties) {
            // Cast to DocVariable to access type field
            DocVariable* var = dynamic_cast<DocVariable*>(prop.get());
            if (var) {
                md_ << "- `" << var->name << ": " << var->type << "`";
                if (!var->doc.summary.empty()) {
                    md_ << " - " << var->doc.summary;
                }
                md_ << "\n";
            }
        }
        md_ << "\n";
    }

    // Methods
    if (!cls.methods.empty()) {
        md_ << "**Methods:**\n\n";
        for (const auto& method : cls.methods) {
            md_ << "#### `" << method->name << "(";
            for (size_t i = 0; i < method->paramNames.size(); i++) {
                if (i > 0) md_ << ", ";
                md_ << method->paramNames[i] << ": " << method->paramTypes[i];
            }
            md_ << ") " << method->returnType << "`\n\n";

            if (!method->doc.summary.empty()) {
                md_ << method->doc.summary << "\n\n";
            }
        }
    }

    // Source location
    md_ << "**Source:** [" << cls.sourceFile << ":" << cls.sourceLine << "](" << cls.sourceFile << "#L" << cls.sourceLine << ")\n";
}

void MarkdownDocGenerator::writeVariable(const DocVariable& var) {
    md_ << "- `" << var.name << ": " << var.type << "`";
    if (!var.doc.summary.empty()) {
        md_ << " - " << var.doc.summary;
    }
    md_ << "\n";
}

void MarkdownDocGenerator::writeDocComment(const DocComment& doc) {
    if (!doc.description.empty()) {
        md_ << doc.description << "\n";
    }
}

} // namespace stratos
