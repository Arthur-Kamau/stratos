# HTML Templating

Stratos provides a lightweight templating engine through the `template` module, supporting variable interpolation, conditionals, and loops with automatic HTML escaping for security.

## Getting Started

```stratos
package main;

use template;

fn main() {
    // Create a context with data
    val ctx = template.context()
        .set("name", "Alice")
        .setInt("age", 25)
        .setBool("isAdmin", true);

    // Render a template
    val html = template.render("Hello, {{ name }}! You are {{ age }} years old.", ctx);
    println(html);  // "Hello, Alice! You are 25 years old."

    ctx.free();  // Free resources
}
```

## Template Syntax

### Variable Interpolation

Use `{{ variable }}` to output values (HTML-escaped by default):

```stratos
val ctx = template.context()
    .set("title", "My Page")
    .set("content", "<script>alert('xss')</script>");

val html = template.render("
    <h1>{{ title }}</h1>
    <div>{{ content }}</div>
", ctx);

// Output (content is escaped):
// <h1>My Page</h1>
// <div>&lt;script&gt;alert('xss')&lt;/script&gt;</div>
```

### Raw Output (No Escaping)

For pre-sanitized HTML, use triple braces or the `|raw` filter:

```stratos
val ctx = template.context()
    .set("html", "<strong>Bold text</strong>");

// Using triple braces
val result1 = template.render("{{{ html }}}", ctx);

// Using |raw filter
val result2 = template.render("{{ html|raw }}", ctx);

// Both output: <strong>Bold text</strong>
```

::: warning Security
Only use raw output for content you trust. Never use it for user-provided input.
:::

### Conditionals

Use `{% if %}` for conditional rendering:

```stratos
val ctx = template.context()
    .setBool("isLoggedIn", true)
    .setBool("isAdmin", false)
    .set("username", "Alice");

val html = template.render("
    {% if isLoggedIn %}
        <p>Welcome, {{ username }}!</p>
        {% if isAdmin %}
            <a href=\"/admin\">Admin Panel</a>
        {% else %}
            <a href=\"/profile\">Profile</a>
        {% endif %}
    {% else %}
        <a href=\"/login\">Login</a>
    {% endif %}
", ctx);
```

### Negated Conditions

Use `{% if not %}` for negated conditions:

```stratos
val ctx = template.context()
    .setBool("hasErrors", false);

val html = template.render("
    {% if not hasErrors %}
        <p class=\"success\">Operation completed successfully!</p>
    {% endif %}
", ctx);
```

### Loops

Use `{% for %}` to iterate over arrays:

```stratos
val ctx = template.context()
    .setArray("items", ["Apple", "Banana", "Cherry"]);

val html = template.render("
    <ul>
    {% for item in items %}
        <li>{{ item }}</li>
    {% endfor %}
    </ul>
", ctx);

// Output:
// <ul>
//     <li>Apple</li>
//     <li>Banana</li>
//     <li>Cherry</li>
// </ul>
```

### Loop Variables

Inside loops, special variables are available:

| Variable | Description |
|----------|-------------|
| `loop.index` | 0-based index |
| `loop.index1` | 1-based index |
| `loop.first` | true on first iteration |
| `loop.last` | true on last iteration |

```stratos
val ctx = template.context()
    .setArray("colors", ["Red", "Green", "Blue"]);

val html = template.render("
    {% for color in colors %}
        <span class=\"{% if loop.first %}first{% endif %} {% if loop.last %}last{% endif %}\">
            {{ loop.index1 }}. {{ color }}
        </span>
    {% endfor %}
", ctx);
```

## Template Objects

### Context

The context holds all template data:

```stratos
val ctx = template.context()
    .set("name", "Alice")           // String
    .setInt("count", 42)            // Integer
    .setDouble("price", 19.99)      // Double
    .setBool("active", true)        // Boolean
    .setArray("items", ["a", "b"]); // Array of strings

// Don't forget to free when done
ctx.free();
```

### Template

Create reusable templates:

```stratos
// From string
val tmpl = template.fromString("<h1>{{ title }}</h1>");

// From file
val tmpl = template.fromFile("templates/page.html");

// Render
val ctx = template.context().set("title", "Hello");
val html = tmpl.render(ctx);
```

## Quick Rendering

For simple cases, use the quick render function:

```stratos
// Pairs: [key1, value1, key2, value2, ...]
val html = template.quick(
    "<h1>{{ title }}</h1><p>By {{ author }}</p>",
    ["title", "My Article", "author", "Jane Doe"]
);
```

## HTML Helpers

### Page Structure

```stratos
// Simple HTML page
val html = template.htmlPage("My Title", "<p>Content here</p>");

// With custom head content
val html = template.htmlPageWithHead(
    "My Title",
    "<link rel=\"stylesheet\" href=\"style.css\">",
    "<p>Content here</p>"
);
```

### Lists

```stratos
// Unordered list
val ul = template.htmlList(["Item 1", "Item 2", "Item 3"], false);
// <ul><li>Item 1</li><li>Item 2</li><li>Item 3</li></ul>

// Ordered list
val ol = template.htmlList(["First", "Second", "Third"], true);
// <ol><li>First</li><li>Second</li><li>Third</li></ol>
```

### Tables

```stratos
val headers = ["Name", "Age", "City"];
val rows = [
    ["Alice", "25", "New York"],
    ["Bob", "30", "London"],
    ["Charlie", "35", "Tokyo"]
];

val table = template.htmlTable(headers, rows);
```

### Form Elements

```stratos
val input = template.htmlInput("text", "username", "Enter username");
// <input type="text" name="username" placeholder="Enter username">

val button = template.htmlButton("Submit", "submit");
// <button type="submit">Submit</button>
```

## Complete Example: Blog Template

```stratos
package main;

use template;

class BlogPost {
    var title: string;
    var author: string;
    var content: string;
    var tags: array<string>;
    var published: bool;
}

fn renderPost(post: BlogPost) string {
    val ctx = template.context()
        .set("title", post.title)
        .set("author", post.author)
        .set("content", post.content)
        .setArray("tags", post.tags)
        .setBool("published", post.published);

    val tmpl = template.fromString("
        <article>
            <header>
                <h1>{{ title }}</h1>
                <p class=\"meta\">By {{ author }}</p>
                {% if not published %}
                    <span class=\"draft\">DRAFT</span>
                {% endif %}
            </header>

            <div class=\"content\">
                {{{ content }}}
            </div>

            {% if tags %}
            <footer class=\"tags\">
                {% for tag in tags %}
                    <a href=\"/tag/{{ tag }}\" class=\"tag\">{{ tag }}</a>
                {% endfor %}
            </footer>
            {% endif %}
        </article>
    ");

    val html = tmpl.render(ctx);
    ctx.free();
    return html;
}

fn main() {
    var post = BlogPost();
    post.title = "Introduction to Stratos";
    post.author = "Jane Doe";
    post.content = "<p>Stratos is a modern programming language...</p>";
    post.tags = ["programming", "stratos", "tutorial"];
    post.published = true;

    val html = renderPost(post);
    println(html);
}
```

## Complete Example: HTTP Response Templates

```stratos
package main;

use template;
use net.http;

fn renderUserList(users: array<map<string, string>>) string {
    val ctx = template.context();

    // Convert users to template-friendly format
    var names: array<string> = [];
    for user in users {
        names.push(user["name"]);
    }
    ctx.setArray("users", names);
    ctx.setInt("count", users.length());

    val html = template.render("
        <h1>User List ({{ count }} users)</h1>
        <ul>
        {% for user in users %}
            <li>{{ user }}</li>
        {% endfor %}
        </ul>
    ", ctx);

    ctx.free();
    return template.htmlPage("Users", html);
}

fn handleUsers(req: http.Request, res: http.Response) {
    val users = [
        {"name": "Alice", "email": "alice@example.com"},
        {"name": "Bob", "email": "bob@example.com"}
    ];

    val html = renderUserList(users);
    res.header("Content-Type", "text/html");
    res.send(html);
}

fn main() {
    val router = http.newRouter();
    router.get("/users", handleUsers);

    val server = http.newServer(router);
    println("Server starting on :8080");
    server.listen(8080);
}
```

## File-Based Templates

Store templates in files for easier maintenance:

**templates/layout.html:**
```html
<!DOCTYPE html>
<html>
<head>
    <title>{{ title }}</title>
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>
    <nav>
        {% if isLoggedIn %}
            <a href="/profile">{{ username }}</a>
            <a href="/logout">Logout</a>
        {% else %}
            <a href="/login">Login</a>
        {% endif %}
    </nav>

    <main>
        {{{ content }}}
    </main>

    <footer>
        &copy; 2024 My App
    </footer>
</body>
</html>
```

**Using the template:**
```stratos
fn renderPage(title: string, content: string, user: User?) string {
    val ctx = template.context()
        .set("title", title)
        .set("content", content)
        .setBool("isLoggedIn", user != null);

    if user != null {
        ctx.set("username", user.name);
    }

    val tmpl = template.fromFile("templates/layout.html");
    val html = tmpl.render(ctx);
    ctx.free();
    return html;
}
```

## Escaping HTML

Manual escaping for dynamic content:

```stratos
val userInput = "<script>alert('xss')</script>";
val safe = template.escapeHtml(userInput);
println(safe);  // &lt;script&gt;alert('xss')&lt;/script&gt;
```

## Best Practices

1. **Always escape user input** - Use `{{ }}` (not `{{{ }}}`) for untrusted content
2. **Free contexts** - Call `ctx.free()` when done to prevent memory leaks
3. **Use file templates** - Keep templates in separate files for maintainability
4. **Organize templates** - Use a `templates/` directory with subdirectories
5. **Validate data** - Ensure required context values are set before rendering
6. **Use helpers** - Leverage `htmlPage()`, `htmlList()`, etc. for common patterns
