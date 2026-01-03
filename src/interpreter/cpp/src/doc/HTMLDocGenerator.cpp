#include "stratos/HTMLDocGenerator.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace stratos {

HTMLDocGenerator::HTMLDocGenerator() {}

void HTMLDocGenerator::generate(const Documentation& doc, const std::string& outputDir) {
    // Create output directory
    fs::create_directories(outputDir);

    // Generate index
    generateIndex(doc, outputDir);

    // Generate each package
    for (const auto& pkg : doc.packages) {
        generatePackage(*pkg, outputDir);
    }
}

void HTMLDocGenerator::generateIndex(const Documentation& doc, const std::string& outputDir) {
    html_.str("");
    html_.clear();

    writeHeader(doc.projectName.empty() ? "Stratos Documentation" : doc.projectName);

    html_ << "<div class='container'>\n";
    html_ << "<h1>" << escapeHTML(doc.projectName.empty() ? "Stratos Documentation" : doc.projectName) << "</h1>\n";

    if (!doc.version.empty()) {
        html_ << "<p><strong>Version:</strong> " << escapeHTML(doc.version) << "</p>\n";
    }

    html_ << "<h2>Packages</h2>\n";
    html_ << "<ul>\n";

    for (const auto& pkg : doc.packages) {
        html_ << "<li><a href='" << pkg->name << ".html'>" << escapeHTML(pkg->name) << "</a>";
        if (!pkg->doc.summary.empty()) {
            html_ << " - " << escapeHTML(pkg->doc.summary);
        }
        html_ << "</li>\n";
    }

    html_ << "</ul>\n";
    html_ << "</div>\n";

    writeFooter();

    // Write to file
    std::ofstream file(outputDir + "/index.html");
    file << html_.str();
    file.close();
}

void HTMLDocGenerator::generatePackage(const DocPackage& pkg, const std::string& outputDir) {
    html_.str("");
    html_.clear();

    writeHeader("Package: " + pkg.name);

    html_ << "<div class='container'>\n";
    html_ << "<h1>Package: " << escapeHTML(pkg.name) << "</h1>\n";

    // Package documentation
    if (!pkg.doc.summary.empty()) {
        html_ << "<p>" << escapeHTML(pkg.doc.description) << "</p>\n";
    }

    // Dependencies
    if (!pkg.dependencies.empty()) {
        html_ << "<h2>Dependencies</h2>\n<ul>\n";
        for (const auto& dep : pkg.dependencies) {
            html_ << "<li>" << escapeHTML(dep) << "</li>\n";
        }
        html_ << "</ul>\n";
    }

    // Functions
    if (!pkg.functions.empty()) {
        html_ << "<h2>Functions</h2>\n";
        for (const auto& func : pkg.functions) {
            writeFunction(*func);
        }
    }

    // Classes
    if (!pkg.classes.empty()) {
        html_ << "<h2>Classes</h2>\n";
        for (const auto& cls : pkg.classes) {
            writeClass(*cls);
        }
    }

    html_ << "</div>\n";

    writeFooter();

    // Write to file
    std::ofstream file(outputDir + "/" + pkg.name + ".html");
    file << html_.str();
    file.close();
}

void HTMLDocGenerator::writeHeader(const std::string& title) {
    html_ << "<!DOCTYPE html>\n";
    html_ << "<html lang='en'>\n";
    html_ << "<head>\n";
    html_ << "<meta charset='UTF-8'>\n";
    html_ << "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
    html_ << "<title>" << escapeHTML(title) << "</title>\n";
    html_ << "<style>\n";
    html_ << "body { font-family: Arial, sans-serif; line-height: 1.6; margin: 0; padding: 20px; background: #f4f4f4; }\n";
    html_ << ".container { max-width: 900px; margin: auto; background: white; padding: 30px; border-radius: 5px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }\n";
    html_ << "h1 { color: #333; border-bottom: 2px solid #007bff; padding-bottom: 10px; }\n";
    html_ << "h2 { color: #555; margin-top: 30px; }\n";
    html_ << "h3 { color: #007bff; }\n";
    html_ << "code { background: #f4f4f4; padding: 2px 6px; border-radius: 3px; font-family: 'Courier New', monospace; }\n";
    html_ << ".function, .class { border: 1px solid #ddd; padding: 15px; margin: 15px 0; border-radius: 5px; background: #fafafa; }\n";
    html_ << ".signature { font-family: 'Courier New', monospace; background: #e8e8e8; padding: 10px; border-radius: 3px; margin: 10px 0; }\n";
    html_ << "ul { line-height: 1.8; }\n";
    html_ << "a { color: #007bff; text-decoration: none; }\n";
    html_ << "a:hover { text-decoration: underline; }\n";
    html_ << "</style>\n";
    html_ << "</head>\n";
    html_ << "<body>\n";
}

void HTMLDocGenerator::writeFooter() {
    html_ << "</body>\n";
    html_ << "</html>\n";
}

void HTMLDocGenerator::writeFunction(const DocFunction& func) {
    html_ << "<div class='function'>\n";
    html_ << "<h3>" << escapeHTML(func.name) << "</h3>\n";

    // Signature
    html_ << "<div class='signature'>";
    html_ << escapeHTML(func.name) << "(";
    for (size_t i = 0; i < func.paramNames.size(); i++) {
        if (i > 0) html_ << ", ";
        html_ << escapeHTML(func.paramNames[i]);
        if (i < func.paramTypes.size()) {
            html_ << ": " << escapeHTML(func.paramTypes[i]);
        }
    }
    html_ << ") " << escapeHTML(func.returnType);
    html_ << "</div>\n";

    // Documentation
    if (!func.doc.summary.empty()) {
        html_ << "<p>" << escapeHTML(func.doc.description) << "</p>\n";
    }

    // Parameters
    if (!func.doc.params.empty()) {
        html_ << "<p><strong>Parameters:</strong></p>\n<ul>\n";
        for (const auto& param : func.doc.params) {
            html_ << "<li><code>" << escapeHTML(param.name) << "</code>";
            if (!param.description.empty()) {
                html_ << " - " << escapeHTML(param.description);
            }
            html_ << "</li>\n";
        }
        html_ << "</ul>\n";
    }

    // Return value
    if (!func.doc.returnDoc.empty()) {
        html_ << "<p><strong>Returns:</strong> " << escapeHTML(func.doc.returnDoc) << "</p>\n";
    }

    // Source location
    html_ << "<p><small><a href='" << func.sourceFile << "#L" << func.sourceLine << "'>Source: "
          << escapeHTML(func.sourceFile) << ":" << func.sourceLine << "</a></small></p>\n";

    html_ << "</div>\n";
}

void HTMLDocGenerator::writeClass(const DocClass& cls) {
    html_ << "<div class='class'>\n";
    html_ << "<h3>Class: " << escapeHTML(cls.name) << "</h3>\n";

    if (!cls.superclass.empty()) {
        html_ << "<p><strong>Extends:</strong> <code>" << escapeHTML(cls.superclass) << "</code></p>\n";
    }

    if (!cls.doc.summary.empty()) {
        html_ << "<p>" << escapeHTML(cls.doc.description) << "</p>\n";
    }

    // Methods
    if (!cls.methods.empty()) {
        html_ << "<p><strong>Methods:</strong></p>\n<ul>\n";
        for (const auto& method : cls.methods) {
            html_ << "<li><code>" << escapeHTML(method->name) << "</code></li>\n";
        }
        html_ << "</ul>\n";
    }

    html_ << "</div>\n";
}

std::string HTMLDocGenerator::escapeHTML(const std::string& text) {
    std::ostringstream escaped;
    for (char c : text) {
        switch (c) {
            case '&': escaped << "&amp;"; break;
            case '<': escaped << "&lt;"; break;
            case '>': escaped << "&gt;"; break;
            case '"': escaped << "&quot;"; break;
            case '\'': escaped << "&#39;"; break;
            default: escaped << c; break;
        }
    }
    return escaped.str();
}

} // namespace stratos
