#ifndef STRATOS_MARKDOWN_DOC_GENERATOR_H
#define STRATOS_MARKDOWN_DOC_GENERATOR_H

#include "stratos/DocModel.h"
#include <string>
#include <sstream>
#include <fstream>

namespace stratos {

/**
 * Generates Markdown documentation from documentation model
 */
class MarkdownDocGenerator {
public:
    MarkdownDocGenerator();

    /**
     * Generate Markdown documentation
     * @param doc Documentation model
     * @param outputDir Output directory path
     */
    void generate(const Documentation& doc, const std::string& outputDir);

private:
    std::ostringstream md_;

    void generateIndex(const Documentation& doc, const std::string& outputDir);
    void generatePackage(const DocPackage& pkg, const std::string& outputDir);

    void writeFunction(const DocFunction& func);
    void writeClass(const DocClass& cls);
    void writeVariable(const DocVariable& var);
    void writeDocComment(const DocComment& doc);
};

} // namespace stratos

#endif // STRATOS_MARKDOWN_DOC_GENERATOR_H
