// XML Parsing and Generation for Stratos
// Inspired by Go's encoding/xml and Python's ElementTree
package xml;

// ============================================================================
// Native function declarations (implemented in C++)
// ============================================================================

fn __xml_parse(xmlString: string) int;
fn __xml_stringify(nodeId: int) string;
fn __xml_get_tag(nodeId: int) string;
fn __xml_get_text(nodeId: int) string;
fn __xml_get_attr(nodeId: int, name: string) string;
fn __xml_has_attr(nodeId: int, name: string) bool;
fn __xml_get_attr_names(nodeId: int) array<string>;
fn __xml_child_count(nodeId: int) int;
fn __xml_get_child(nodeId: int, index: int) int;
fn __xml_find_by_tag(nodeId: int, tag: string) array<int>;
fn __xml_find_all_by_tag(nodeId: int, tag: string) array<int>;
fn __xml_free(nodeId: int) void;
fn __xml_create(tag: string) int;
fn __xml_set_text(nodeId: int, text: string) void;
fn __xml_set_attr(nodeId: int, name: string, value: string) void;
fn __xml_add_child(parentId: int, childId: int) void;

// ============================================================================
// Class Definitions
// ============================================================================

// Node represents an XML element
class Node {
    var nodeId: int;

    constructor(id: int) {
        this.nodeId = id;
    }

    // Get the tag name of this element
    pub fn tag() string {
        return __xml_get_tag(this.nodeId);
    }

    // Get the text content of this element
    pub fn text() string {
        return __xml_get_text(this.nodeId);
    }

    // Set the text content of this element
    pub fn setText(text: string) {
        __xml_set_text(this.nodeId, text);
    }

    // Get an attribute value
    //
    // Example:
    //   val id = node.attr("id");
    pub fn attr(name: string) string {
        return __xml_get_attr(this.nodeId, name);
    }

    // Check if an attribute exists
    pub fn hasAttr(name: string) bool {
        return __xml_has_attr(this.nodeId, name);
    }

    // Set an attribute value
    //
    // Example:
    //   node.setAttr("class", "container");
    pub fn setAttr(name: string, value: string) {
        __xml_set_attr(this.nodeId, name, value);
    }

    // Get all attribute names
    pub fn attrNames() array<string> {
        return __xml_get_attr_names(this.nodeId);
    }

    // Get the number of child elements
    pub fn childCount() int {
        return __xml_child_count(this.nodeId);
    }

    // Get a child element by index
    //
    // Example:
    //   val firstChild = node.child(0);
    pub fn child(index: int) Node {
        val childId = __xml_get_child(this.nodeId, index);
        return Node(childId);
    }

    // Get all children as an array of Nodes
    pub fn children() array<Node> {
        var result: array<Node> = [];
        val count = this.childCount();
        var i = 0;
        while i < count {
            result.push(this.child(i));
            i = i + 1;
        }
        return result;
    }

    // Find direct children with a specific tag
    //
    // Example:
    //   val items = root.find("item");
    pub fn find(tag: string) array<Node> {
        val ids = __xml_find_by_tag(this.nodeId, tag);
        var result: array<Node> = [];
        for id in ids {
            result.push(Node(id));
        }
        return result;
    }

    // Find all descendant elements with a specific tag (recursive)
    //
    // Example:
    //   val allLinks = root.findAll("a");
    pub fn findAll(tag: string) array<Node> {
        val ids = __xml_find_all_by_tag(this.nodeId, tag);
        var result: array<Node> = [];
        for id in ids {
            result.push(Node(id));
        }
        return result;
    }

    // Find the first child element with a specific tag
    // Returns null if not found
    pub fn findFirst(tag: string) Node? {
        val found = this.find(tag);
        if found.length() > 0 {
            return found[0];
        }
        return null;
    }

    // Add a child element
    pub fn addChild(child: Node) {
        __xml_add_child(this.nodeId, child.nodeId);
    }

    // Convert this element and its descendants to XML string
    pub fn toString() string {
        return __xml_stringify(this.nodeId);
    }

    // Free the resources associated with this node
    // Only needed for root nodes, not children
    pub fn free() {
        __xml_free(this.nodeId);
    }
}

// ============================================================================
// Public API Functions
// ============================================================================

// Parse an XML string and return the root element
//
// Example:
//   val doc = xml.parse("<root><item>Hello</item></root>");
//   val items = doc.find("item");
//   println(items[0].text()); // "Hello"
fn parse(xmlString: string) Node {
    val nodeId = __xml_parse(xmlString);
    return Node(nodeId);
}

// Create a new XML element
//
// Example:
//   val root = xml.create("root");
//   root.setAttr("version", "1.0");
//   val child = xml.create("item");
//   child.setText("Hello");
//   root.addChild(child);
//   println(root.toString());
fn create(tag: string) Node {
    val nodeId = __xml_create(tag);
    return Node(nodeId);
}

// Convert an XML node to a formatted string
fn stringify(node: Node) string {
    return node.toString();
}

// ============================================================================
// Builder Pattern for XML Construction
// ============================================================================

// Builder provides a fluent interface for constructing XML documents
class Builder {
    var root: Node;
    var current: Node;

    constructor(rootTag: string) {
        this.root = create(rootTag);
        this.current = this.root;
    }

    // Add an attribute to the current element
    pub fn attr(name: string, value: string) Builder {
        this.current.setAttr(name, value);
        return this;
    }

    // Set the text content of the current element
    pub fn text(content: string) Builder {
        this.current.setText(content);
        return this;
    }

    // Add a child element and make it current
    pub fn element(tag: string) Builder {
        val child = create(tag);
        this.current.addChild(child);
        this.current = child;
        return this;
    }

    // Add a simple element with text content (stays at current level)
    pub fn textElement(tag: string, content: string) Builder {
        val child = create(tag);
        child.setText(content);
        this.current.addChild(child);
        return this;
    }

    // Move back to the parent element
    // Note: This is a simplified version - parent tracking would need to be added
    pub fn up() Builder {
        this.current = this.root;
        return this;
    }

    // Build and return the XML string
    pub fn build() string {
        return this.root.toString();
    }

    // Get the root node
    pub fn getRoot() Node {
        return this.root;
    }
}

// Create a new XML builder
//
// Example:
//   val xml = xml.builder("catalog")
//       .element("book")
//           .attr("id", "1")
//           .textElement("title", "XML Guide")
//           .textElement("author", "John Doe")
//       .up()
//       .build();
fn builder(rootTag: string) Builder {
    return Builder(rootTag);
}

// ============================================================================
// Helper Functions
// ============================================================================

// Escape special XML characters in text
fn escapeText(text: string) string {
    var result = text;
    result = result.replace("&", "&amp;");
    result = result.replace("<", "&lt;");
    result = result.replace(">", "&gt;");
    return result;
}

// Escape special XML characters in attribute values
fn escapeAttr(value: string) string {
    var result = escapeText(value);
    result = result.replace("\"", "&quot;");
    result = result.replace("'", "&apos;");
    return result;
}

// Create a simple text element
fn textElement(tag: string, content: string) Node {
    val node = create(tag);
    node.setText(content);
    return node;
}
