#ifndef STRATOS_DOC_MODEL_H
#define STRATOS_DOC_MODEL_H

#include "DocComment.h"
#include <string>
#include <vector>
#include <memory>

namespace stratos {

/**
 * Base documentation node
 */
struct DocNode {
    std::string name;
    std::string kind; // "function", "class", "variable", "package"
    DocComment doc;
    std::string sourceFile;
    int sourceLine;

    virtual ~DocNode() = default;
};

/**
 * Function documentation
 */
struct DocFunction : public DocNode {
    std::vector<std::string> paramNames;
    std::vector<std::string> paramTypes;
    std::string returnType;
    bool isNative;

    DocFunction() { kind = "function"; isNative = false; }
};

/**
 * Class documentation
 */
struct DocClass : public DocNode {
    std::string superclass;
    std::vector<std::unique_ptr<DocFunction>> methods;
    std::vector<std::unique_ptr<DocNode>> properties;

    DocClass() { kind = "class"; }
};

/**
 * Variable documentation
 */
struct DocVariable : public DocNode {
    std::string type;
    bool isMutable;

    DocVariable() { kind = "variable"; isMutable = false; }
};

/**
 * Package/Module documentation
 */
struct DocPackage : public DocNode {
    std::vector<std::unique_ptr<DocFunction>> functions;
    std::vector<std::unique_ptr<DocClass>> classes;
    std::vector<std::unique_ptr<DocVariable>> variables;
    std::vector<std::string> dependencies; // use statements

    DocPackage() { kind = "package"; }
};

/**
 * Complete documentation for a project
 */
struct Documentation {
    std::vector<std::unique_ptr<DocPackage>> packages;
    std::string projectName;
    std::string version;
};

} // namespace stratos

#endif // STRATOS_DOC_MODEL_H
