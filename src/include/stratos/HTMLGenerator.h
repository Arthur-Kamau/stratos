#ifndef STRATOS_HTML_GENERATOR_H
#define STRATOS_HTML_GENERATOR_H

#include "STUIAST.h"
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>

namespace stratos {

struct HTMLGenerateResult {
    bool success;
    std::string errorMessage;
    std::string htmlPath;
    std::string cssPath;
    std::string jsPath;
};

class HTMLGenerator {
public:
    HTMLGenerator();

    // Generate HTML/CSS/JS from a parsed STUI file
    HTMLGenerateResult generate(const stui::STUIFile& file,
                                const std::string& outputDir,
                                const std::string& title = "Stratos App");

private:
    int idCounter_ = 0;
    std::stringstream html_;
    std::stringstream css_;
    std::stringstream js_;
    std::vector<std::string> stateVars_; // Track state variables for reactivity

    std::string nextId(const std::string& prefix = "el");

    // HTML generation
    void generateHTML(const stui::STUIFile& file, const std::string& title);
    void generateWidget(const stui::WidgetNode& widget, int indent);
    void generateWidgetOpen(const stui::WidgetNode& widget, const std::string& id, int indent);
    void generateWidgetClose(const stui::WidgetNode& widget, int indent);
    std::string getHtmlTag(const std::string& widgetType);
    std::string getWidgetClasses(const stui::WidgetNode& widget);

    // CSS generation
    void generateCSS(const stui::STUIFile& file);
    void generateBaseStyles();
    void generateWidgetCSS(const stui::WidgetNode& widget, const std::string& id);
    std::string propertyToCSS(const std::string& name, const std::string& value);

    // JS generation
    void generateJS(const stui::STUIFile& file);
    void generateStateJS(const stui::ComponentDecl& comp);
    void generateEventJS(const stui::WidgetNode& widget, const std::string& id);

    // Expression to string helpers
    std::string exprToString(const stui::STUIExpr& expr);
    std::string exprToJS(const stui::STUIExpr& expr);

    // Indentation helper
    std::string ind(int level);
};

} // namespace stratos

#endif // STRATOS_HTML_GENERATOR_H
