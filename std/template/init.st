// HTML Templating for Stratos
// Inspired by Jinja2, Go templates, and Mustache
package template;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

fn __template_create_context() int;
fn __template_set_string(ctxId: int, key: string, value: string) void;
fn __template_set_int(ctxId: int, key: string, value: int) void;
fn __template_set_double(ctxId: int, key: string, value: double) void;
fn __template_set_bool(ctxId: int, key: string, value: bool) void;
fn __template_set_array(ctxId: int, key: string, values: array<string>) void;
fn __template_render(templateString: string, ctxId: int) string;
fn __template_render_file(filePath: string, ctxId: int) string;
fn __template_free_context(ctxId: int) void;
fn __template_escape_html(str: string) string;

// ============================================================================
// Class Definitions
// ============================================================================

// Context holds template data for rendering
class Context {
    var ctxId: int;

    constructor() {
        this.ctxId = __template_create_context();
    }

    // Set a string value
    //
    // Example:
    //   ctx.set("name", "Alice");
    pub fn set(key: string, value: string) Context {
        __template_set_string(this.ctxId, key, value);
        return this;
    }

    // Set an integer value
    //
    // Example:
    //   ctx.setInt("count", 42);
    pub fn setInt(key: string, value: int) Context {
        __template_set_int(this.ctxId, key, value);
        return this;
    }

    // Set a double value
    pub fn setDouble(key: string, value: double) Context {
        __template_set_double(this.ctxId, key, value);
        return this;
    }

    // Set a boolean value
    //
    // Example:
    //   ctx.setBool("isLoggedIn", true);
    pub fn setBool(key: string, value: bool) Context {
        __template_set_bool(this.ctxId, key, value);
        return this;
    }

    // Set an array of strings for iteration
    //
    // Example:
    //   ctx.setArray("items", ["Apple", "Banana", "Cherry"]);
    pub fn setArray(key: string, values: array<string>) Context {
        __template_set_array(this.ctxId, key, values);
        return this;
    }

    // Free the context resources
    pub fn free() {
        __template_free_context(this.ctxId);
    }
}

// Template represents a compiled template
class Template {
    var content: string;
    var isFile: bool;

    constructor(templateContent: string, fromFile: bool) {
        this.content = templateContent;
        this.isFile = fromFile;
    }

    // Render the template with the given context
    //
    // Example:
    //   val ctx = template.context()
    //       .set("name", "Alice")
    //       .setBool("isAdmin", true);
    //   val html = tmpl.render(ctx);
    pub fn render(ctx: Context) string {
        if this.isFile {
            return __template_render_file(this.content, ctx.ctxId);
        }
        return __template_render(this.content, ctx.ctxId);
    }

    // Render with a fresh context using a builder function
    pub fn renderWith(builder: fn(Context) void) string {
        val ctx = Context();
        builder(ctx);
        val result = this.render(ctx);
        ctx.free();
        return result;
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Create a new template context
//
// Example:
//   val ctx = template.context();
//   ctx.set("title", "My Page");
fn context() Context {
    return Context();
}

// Create a template from a string
//
// Template syntax:
//   {{ variable }}           - Output variable (HTML escaped)
//   {{{ variable }}}         - Output variable (raw, no escaping)
//   {{ variable|raw }}       - Output variable (raw, no escaping)
//   {% if condition %}...{% endif %}
//   {% if condition %}...{% else %}...{% endif %}
//   {% if not condition %}...{% endif %}
//   {% for item in list %}...{% endfor %}
//
// Loop variables:
//   {{ loop.index }}   - 0-based index
//   {{ loop.index1 }}  - 1-based index
//   {{ loop.first }}   - true if first iteration
//   {{ loop.last }}    - true if last iteration
//
// Example:
//   val tmpl = template.fromString("<h1>{{ title }}</h1>");
fn fromString(content: string) Template {
    return Template(content, false);
}

// Create a template from a file
//
// Example:
//   val tmpl = template.fromFile("templates/index.html");
fn fromFile(path: string) Template {
    return Template(path, true);
}

// Render a template string directly with a context
//
// Example:
//   val ctx = template.context().set("name", "World");
//   val result = template.render("Hello, {{ name }}!", ctx);
fn render(templateString: string, ctx: Context) string {
    return __template_render(templateString, ctx.ctxId);
}

// Render a template file directly with a context
fn renderFile(path: string, ctx: Context) string {
    return __template_render_file(path, ctx.ctxId);
}

// Escape HTML special characters
//
// Example:
//   val safe = template.escapeHtml("<script>alert('xss')</script>");
//   // Returns: "&lt;script&gt;alert('xss')&lt;/script&gt;"
fn escapeHtml(str: string) string {
    return __template_escape_html(str);
}

// ============================================================================
// Quick Render Functions
// ============================================================================

// Quickly render a template with key-value pairs
//
// Example:
//   val html = template.quick("<h1>{{ title }}</h1><p>{{ body }}</p>",
//       ["title", "Welcome", "body", "Hello World"]);
fn quick(templateString: string, pairs: array<string>) string {
    val ctx = Context();
    var i = 0;
    while i < pairs.length() - 1 {
        ctx.set(pairs[i], pairs[i + 1]);
        i = i + 2;
    }
    val result = render(templateString, ctx);
    ctx.free();
    return result;
}

// ============================================================================
// HTML Response Helpers (for HTTP handlers)
// ============================================================================

// Render an HTML page with common structure
//
// Example:
//   val html = template.htmlPage("My Title", "<p>Content here</p>");
fn htmlPage(title: string, body: string) string {
    return "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>" + escapeHtml(title) + "</title>\n</head>\n<body>\n" + body + "\n</body>\n</html>";
}

// Render an HTML page with custom head content
fn htmlPageWithHead(title: string, headContent: string, body: string) string {
    return "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>" + escapeHtml(title) + "</title>\n" + headContent + "\n</head>\n<body>\n" + body + "\n</body>\n</html>";
}

// ============================================================================
// Common Template Snippets
// ============================================================================

// Generate an HTML list from an array
fn htmlList(items: array<string>, ordered: bool) string {
    val tag = ordered ? "ol" : "ul";
    var html = "<" + tag + ">\n";
    for item in items {
        html = html + "  <li>" + escapeHtml(item) + "</li>\n";
    }
    html = html + "</" + tag + ">";
    return html;
}

// Generate an HTML table from rows of data
fn htmlTable(headers: array<string>, rows: array<array<string>>) string {
    var html = "<table>\n<thead>\n<tr>\n";
    for header in headers {
        html = html + "  <th>" + escapeHtml(header) + "</th>\n";
    }
    html = html + "</tr>\n</thead>\n<tbody>\n";
    for row in rows {
        html = html + "<tr>\n";
        for cell in row {
            html = html + "  <td>" + escapeHtml(cell) + "</td>\n";
        }
        html = html + "</tr>\n";
    }
    html = html + "</tbody>\n</table>";
    return html;
}

// Generate a simple form input
fn htmlInput(inputType: string, name: string, placeholder: string) string {
    return "<input type=\"" + escapeHtml(inputType) + "\" name=\"" + escapeHtml(name) + "\" placeholder=\"" + escapeHtml(placeholder) + "\">";
}

// Generate a submit button
fn htmlButton(text: string, btnType: string) string {
    return "<button type=\"" + escapeHtml(btnType) + "\">" + escapeHtml(text) + "</button>";
}
