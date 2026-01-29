#include "stratos/JSONDocGenerator.h"
#include <fstream>
#include <sstream>

namespace stratos {

void JSONDocGenerator::generate(const Documentation& doc, const std::string& outputPath) {
    std::string json = toJSON(doc);

    std::ofstream file(outputPath);
    file << json;
    file.close();
}

std::string JSONDocGenerator::toJSON(const Documentation& doc) {
    std::ostringstream json;
    json << "{\n";
    json << "  \"projectName\": \"" << escapeJSON(doc.projectName) << "\",\n";
    json << "  \"version\": \"" << escapeJSON(doc.version) << "\",\n";
    json << "  \"packages\": [\n";

    for (size_t i = 0; i < doc.packages.size(); i++) {
        if (i > 0) json << ",\n";
        json << toJSON(*doc.packages[i]);
    }

    json << "\n  ]\n";
    json << "}\n";
    return json.str();
}

std::string JSONDocGenerator::toJSON(const DocPackage& pkg) {
    std::ostringstream json;
    json << "    {\n";
    json << "      \"name\": \"" << escapeJSON(pkg.name) << "\",\n";
    json << "      \"sourceFile\": \"" << escapeJSON(pkg.sourceFile) << "\",\n";
    json << "      \"sourceLine\": " << pkg.sourceLine << ",\n";
    json << "      \"documentation\": " << toJSON(pkg.doc) << ",\n";

    json << "      \"dependencies\": [";
    for (size_t i = 0; i < pkg.dependencies.size(); i++) {
        if (i > 0) json << ", ";
        json << "\"" << escapeJSON(pkg.dependencies[i]) << "\"";
    }
    json << "],\n";

    json << "      \"functions\": [\n";
    for (size_t i = 0; i < pkg.functions.size(); i++) {
        if (i > 0) json << ",\n";
        json << "        " << toJSON(*pkg.functions[i]);
    }
    json << "\n      ],\n";

    json << "      \"classes\": [\n";
    for (size_t i = 0; i < pkg.classes.size(); i++) {
        if (i > 0) json << ",\n";
        json << "        " << toJSON(*pkg.classes[i]);
    }
    json << "\n      ]\n";

    json << "    }";
    return json.str();
}

std::string JSONDocGenerator::toJSON(const DocFunction& func) {
    std::ostringstream json;
    json << "{\n";
    json << "          \"name\": \"" << escapeJSON(func.name) << "\",\n";
    json << "          \"returnType\": \"" << escapeJSON(func.returnType) << "\",\n";
    json << "          \"isNative\": " << (func.isNative ? "true" : "false") << ",\n";
    json << "          \"sourceFile\": \"" << escapeJSON(func.sourceFile) << "\",\n";
    json << "          \"sourceLine\": " << func.sourceLine << ",\n";

    json << "          \"parameters\": [";
    for (size_t i = 0; i < func.paramNames.size(); i++) {
        if (i > 0) json << ", ";
        json << "{\"name\": \"" << escapeJSON(func.paramNames[i]) << "\", ";
        json << "\"type\": \"" << (i < func.paramTypes.size() ? escapeJSON(func.paramTypes[i]) : "any") << "\"}";
    }
    json << "],\n";

    json << "          \"documentation\": " << toJSON(func.doc) << "\n";
    json << "        }";
    return json.str();
}

std::string JSONDocGenerator::toJSON(const DocClass& cls) {
    std::ostringstream json;
    json << "{\n";
    json << "          \"name\": \"" << escapeJSON(cls.name) << "\",\n";
    json << "          \"superclass\": \"" << escapeJSON(cls.superclass) << "\",\n";
    json << "          \"sourceFile\": \"" << escapeJSON(cls.sourceFile) << "\",\n";
    json << "          \"sourceLine\": " << cls.sourceLine << ",\n";
    json << "          \"documentation\": " << toJSON(cls.doc) << "\n";
    json << "        }";
    return json.str();
}

std::string JSONDocGenerator::toJSON(const DocComment& doc) {
    std::ostringstream json;
    json << "{\n";
    json << "            \"summary\": \"" << escapeJSON(doc.summary) << "\",\n";
    json << "            \"description\": \"" << escapeJSON(doc.description) << "\",\n";
    json << "            \"returnDoc\": \"" << escapeJSON(doc.returnDoc) << "\"\n";
    json << "          }";
    return json.str();
}

std::string JSONDocGenerator::escapeJSON(const std::string& str) {
    std::ostringstream escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped << "\\\""; break;
            case '\\': escaped << "\\\\"; break;
            case '\n': escaped << "\\n"; break;
            case '\r': escaped << "\\r"; break;
            case '\t': escaped << "\\t"; break;
            default: escaped << c; break;
        }
    }
    return escaped.str();
}

} // namespace stratos
