#ifndef STRATOS_DOC_COMMENT_H
#define STRATOS_DOC_COMMENT_H

#include <string>
#include <vector>
#include <unordered_map>

namespace stratos {

/**
 * Represents a single parameter documentation
 */
struct ParamDoc {
    std::string name;
    std::string type;
    std::string description;
};

/**
 * Represents parsed documentation from doc comments
 */
struct DocComment {
    std::string summary;              // First paragraph
    std::string description;          // Full description (markdown)
    std::vector<ParamDoc> params;     // @param tags
    std::string returnDoc;            // @return tag
    std::vector<std::string> throws;  // @throws tags
    std::vector<std::string> examples; // @example tags
    std::string since;                // @since tag
    std::string deprecated;           // @deprecated tag
    std::unordered_map<std::string, std::string> customTags; // Other tags

    int line;                         // Source location
    std::string rawText;              // Unparsed comment text

    DocComment() : line(0) {}

    bool isEmpty() const {
        return summary.empty() && description.empty();
    }
};

} // namespace stratos

#endif // STRATOS_DOC_COMMENT_H
