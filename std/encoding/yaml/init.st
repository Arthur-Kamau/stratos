// YAML module - YAML encoding and decoding
package yaml;

// ============================================================================
// Native function declarations
// ============================================================================

// Serialize object to YAML string
fn yamlStringify(value: any) string;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Marshal any value to YAML
fn yamlMarshal(obj: any) string {
    return yamlStringify(obj);
}
