# XML Processing

Stratos provides XML parsing and generation through the `xml` module, supporting both DOM-style navigation and a fluent builder API.

## Parsing XML

Parse XML strings into navigable document trees:

```stratos
package main;

use encoding.xml;

fn main() {
    val xmlStr = "
        <catalog>
            <book id=\"1\">
                <title>The Stratos Guide</title>
                <author>Jane Doe</author>
                <price>29.99</price>
            </book>
            <book id=\"2\">
                <title>Advanced Programming</title>
                <author>John Smith</author>
                <price>49.99</price>
            </book>
        </catalog>
    ";

    val doc = xml.parse(xmlStr);
    println("Root element: " + doc.tag());  // "catalog"

    doc.free();  // Free resources when done
}
```

## Navigating Documents

### Accessing Elements

```stratos
val doc = xml.parse("<root><child>Hello</child></root>");

// Get tag name
println(doc.tag());  // "root"

// Get text content
val child = doc.child(0);
println(child.text());  // "Hello"

// Get child count
println("Children: " + doc.childCount());
```

### Working with Attributes

```stratos
val doc = xml.parse("<user id=\"123\" role=\"admin\">Alice</user>");

// Get attribute value
val id = doc.attr("id");
println("User ID: " + id);

// Check if attribute exists
if doc.hasAttr("role") {
    println("Role: " + doc.attr("role"));
}

// Get all attribute names
val attrs = doc.attrNames();
for name in attrs {
    println(name + " = " + doc.attr(name));
}
```

### Finding Elements

```stratos
val doc = xml.parse("
    <library>
        <section name=\"fiction\">
            <book>1984</book>
            <book>Brave New World</book>
        </section>
        <section name=\"tech\">
            <book>Clean Code</book>
        </section>
    </library>
");

// Find direct children by tag
val sections = doc.find("section");
println("Sections: " + sections.length());

// Find all descendants by tag (recursive)
val allBooks = doc.findAll("book");
println("Total books: " + allBooks.length());

for book in allBooks {
    println("  - " + book.text());
}

// Find first matching element
val firstSection = doc.findFirst("section");
if firstSection != null {
    println("First section: " + firstSection.attr("name"));
}
```

### Iterating Children

```stratos
val doc = xml.parse("<items><item>A</item><item>B</item><item>C</item></items>");

// Get all children
val children = doc.children();
for child in children {
    println(child.tag() + ": " + child.text());
}

// Or iterate by index
for i in 0..doc.childCount() {
    val child = doc.child(i);
    println(child.text());
}
```

## Creating XML

### Using create()

```stratos
// Create elements
val root = xml.create("catalog");
root.setAttr("version", "1.0");

val book = xml.create("book");
book.setAttr("id", "1");

val title = xml.create("title");
title.setText("Stratos Guide");

val author = xml.create("author");
author.setText("Jane Doe");

// Build tree
book.addChild(title);
book.addChild(author);
root.addChild(book);

// Convert to string
println(root.toString());
```

Output:
```xml
<catalog version="1.0">
  <book id="1">
    <title>Stratos Guide</title>
    <author>Jane Doe</author>
  </book>
</catalog>
```

### Using the Builder API

The builder provides a fluent interface:

```stratos
val xmlStr = xml.builder("catalog")
    .attr("version", "1.0")
    .element("book")
        .attr("id", "1")
        .textElement("title", "Stratos Guide")
        .textElement("author", "Jane Doe")
        .textElement("price", "29.99")
    .up()
    .element("book")
        .attr("id", "2")
        .textElement("title", "Advanced Programming")
        .textElement("author", "John Smith")
    .build();

println(xmlStr);
```

### Helper Functions

```stratos
// Create a simple text element
val title = xml.textElement("title", "My Book");
println(title.toString());  // <title>My Book</title>

// Escape special characters
val safe = xml.escapeText("<script>alert('xss')</script>");
println(safe);  // &lt;script&gt;alert('xss')&lt;/script&gt;

val safeAttr = xml.escapeAttr("value with \"quotes\"");
println(safeAttr);  // value with &quot;quotes&quot;
```

## Complete Example: RSS Feed Parser

```stratos
package main;

use encoding.xml;
use net.http;

class RssItem {
    var title: string;
    var link: string;
    var description: string;
    var pubDate: string;
}

fn parseRssFeed(xmlStr: string) array<RssItem> {
    var items: array<RssItem> = [];
    val doc = xml.parse(xmlStr);

    // RSS structure: <rss><channel><item>...</item></channel></rss>
    val channel = doc.findFirst("channel");
    if channel == null {
        return items;
    }

    val xmlItems = channel.find("item");
    for xmlItem in xmlItems {
        val item = RssItem();

        val title = xmlItem.findFirst("title");
        if title != null {
            item.title = title.text();
        }

        val link = xmlItem.findFirst("link");
        if link != null {
            item.link = link.text();
        }

        val desc = xmlItem.findFirst("description");
        if desc != null {
            item.description = desc.text();
        }

        val pubDate = xmlItem.findFirst("pubDate");
        if pubDate != null {
            item.pubDate = pubDate.text();
        }

        items.push(item);
    }

    doc.free();
    return items;
}

fn main() {
    val rssXml = "
        <rss version=\"2.0\">
            <channel>
                <title>Tech News</title>
                <item>
                    <title>New Stratos Release</title>
                    <link>https://example.com/stratos</link>
                    <description>Version 2.0 brings exciting features</description>
                    <pubDate>Mon, 01 Jan 2024 12:00:00 GMT</pubDate>
                </item>
                <item>
                    <title>Database Updates</title>
                    <link>https://example.com/db</link>
                    <description>PostgreSQL and Redis support added</description>
                    <pubDate>Tue, 02 Jan 2024 10:00:00 GMT</pubDate>
                </item>
            </channel>
        </rss>
    ";

    val items = parseRssFeed(rssXml);

    println("Found " + items.length() + " items:");
    for item in items {
        println("\n" + item.title);
        println("  Link: " + item.link);
        println("  Date: " + item.pubDate);
    }
}
```

## Complete Example: Configuration File

```stratos
package main;

use encoding.xml;
use io;

class ServerConfig {
    var host: string;
    var port: int;
    var maxConnections: int;
    var ssl: bool;
}

fn loadConfig(path: string) ServerConfig {
    val content = io.readFile(path);
    val doc = xml.parse(content);

    val config = ServerConfig();

    val server = doc.findFirst("server");
    if server != null {
        val host = server.findFirst("host");
        if host != null {
            config.host = host.text();
        }

        val port = server.findFirst("port");
        if port != null {
            config.port = port.text().toInt();
        }

        val maxConn = server.findFirst("maxConnections");
        if maxConn != null {
            config.maxConnections = maxConn.text().toInt();
        }

        val ssl = server.findFirst("ssl");
        if ssl != null {
            config.ssl = ssl.text() == "true";
        }
    }

    doc.free();
    return config;
}

fn saveConfig(path: string, config: ServerConfig) {
    val xmlStr = xml.builder("configuration")
        .element("server")
            .textElement("host", config.host)
            .textElement("port", config.port.toString())
            .textElement("maxConnections", config.maxConnections.toString())
            .textElement("ssl", config.ssl.toString())
        .build();

    io.writeFile(path, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + xmlStr);
}

fn main() {
    // Create and save config
    var config = ServerConfig();
    config.host = "localhost";
    config.port = 8080;
    config.maxConnections = 100;
    config.ssl = true;

    saveConfig("config.xml", config);
    println("Config saved");

    // Load it back
    val loaded = loadConfig("config.xml");
    println("Loaded: " + loaded.host + ":" + loaded.port);
}
```

## SOAP/Legacy API Integration

```stratos
package main;

use encoding.xml;
use net.http;

fn createSoapEnvelope(methodName: string, params: array<array<string>>) string {
    val builder = xml.builder("soap:Envelope")
        .attr("xmlns:soap", "http://schemas.xmlsoap.org/soap/envelope/")
        .element("soap:Body")
            .element(methodName);

    for param in params {
        builder.textElement(param[0], param[1]);
    }

    return "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" + builder.build();
}

fn parseSoapResponse(xmlStr: string, resultTag: string) string {
    val doc = xml.parse(xmlStr);
    val results = doc.findAll(resultTag);

    if results.length() > 0 {
        val result = results[0].text();
        doc.free();
        return result;
    }

    doc.free();
    return "";
}

fn main() {
    // Create SOAP request
    val soapRequest = createSoapEnvelope("GetWeather", [
        ["city", "New York"],
        ["country", "US"]
    ]);

    println(soapRequest);

    // Parse SOAP response
    val soapResponse = "
        <soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">
            <soap:Body>
                <GetWeatherResponse>
                    <Temperature>72</Temperature>
                    <Conditions>Sunny</Conditions>
                </GetWeatherResponse>
            </soap:Body>
        </soap:Envelope>
    ";

    val temp = parseSoapResponse(soapResponse, "Temperature");
    println("Temperature: " + temp);
}
```

## Best Practices

1. **Always free parsed documents** - Call `doc.free()` when done
2. **Check for null** - `findFirst()` returns null if element not found
3. **Escape user content** - Use `xml.escapeText()` and `xml.escapeAttr()`
4. **Use builder for creation** - More readable than manual element construction
5. **Handle missing elements gracefully** - XML documents may vary
