#ifndef STRATOS_HTML_DOC_GENERATOR_H
#define STRATOS_HTML_DOC_GENERATOR_H

#include "stratos/DocModel.h"
#include <string>
#include <sstream>

namespace stratos {

/**
 * Generates HTML documentation from documentation model
 */
class HTMLDocGenerator {
public:
    HTMLDocGenerator();

    /**
     * Generate HTML documentation
     * @param doc Documentation model
     * @param outputDir Output directory path
     */
    void generate(const Documentation& doc, const std::string& outputDir);

private:
    std::ostringstream html_;

    void generateIndex(const Documentation& doc, const std::string& outputDir);
    void generatePackage(const DocPackage& pkg, const std::string& outputDir);

    void writeHeader(const std::string& title);
    void writeFooter();
    void writeFunction(const DocFunction& func);
    void writeClass(const DocClass& cls);
    std::string escapeHTML(const std::string& text);
};

} // namespace stratos

#endif // STRATOS_HTML_DOC_GENERATOR_H
