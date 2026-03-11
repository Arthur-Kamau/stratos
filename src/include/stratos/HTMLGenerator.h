#ifndef STRATOS_HTML_GENERATOR_H
#define STRATOS_HTML_GENERATOR_H

#include "STUIAST.h"
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

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

    // Component expansion: map component names to their declarations
    std::unordered_map<std::string, const stui::ComponentDecl*> componentMap_;

    // Collected during HTML generation, used in JS generation
    struct EventBinding {
        std::string elementId;
        std::string domEvent;
        std::string handlerBody;
    };
    struct TextBinding {
        std::string elementId;
        std::string templateStr; // e.g., "Count: ${count}"
    };
    std::vector<EventBinding> eventBindings_;
    std::vector<TextBinding> textBindings_;

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
    void generateSignalRuntime();
    void generateStateJS(const stui::ComponentDecl& comp);
    void generateEventBindingsJS();
    void generateTextBindingsJS();

    // Event/state transformation helpers
    void collectEvents(const stui::WidgetNode& widget, const std::string& id);
    void collectTextBinding(const stui::WidgetNode& widget, const std::string& id);
    std::string transformHandlerBody(const std::string& body);
    std::string transformInterpolatedString(const std::string& raw);
    std::string capitalizeFirst(const std::string& s);
    bool isStateVar(const std::string& name) const;

    // Component expansion
    void buildComponentMap(const stui::STUIFile& file);
    void collectComponentStates(const stui::ComponentDecl& comp);

    // Expression to string helpers
    std::string exprToString(const stui::STUIExpr& expr);
    std::string exprToJS(const stui::STUIExpr& expr);

    // Indentation helper
    std::string ind(int level);
};

} // namespace stratos

#endif // STRATOS_HTML_GENERATOR_H
