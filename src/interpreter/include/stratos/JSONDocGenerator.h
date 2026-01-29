#ifndef STRATOS_JSON_DOC_GENERATOR_H
#define STRATOS_JSON_DOC_GENERATOR_H

#include "stratos/DocModel.h"
#include <string>

namespace stratos {

/**
 * Generates JSON documentation from documentation model
 */
class JSONDocGenerator {
public:
    /**
     * Generate JSON documentation
     * @param doc Documentation model
     * @param outputPath Output file path
     */
    void generate(const Documentation& doc, const std::string& outputPath);

private:
    std::string toJSON(const Documentation& doc);
    std::string toJSON(const DocPackage& pkg);
    std::string toJSON(const DocFunction& func);
    std::string toJSON(const DocClass& cls);
    std::string toJSON(const DocComment& doc);
    std::string escapeJSON(const std::string& str);
};

} // namespace stratos

#endif // STRATOS_JSON_DOC_GENERATOR_H
