// JSON module - JSON encoding and decoding
// Inspired by Go's encoding/json package

package json;

// ============================================================================
// Types
// ============================================================================

enum JsonType {
    NULL,
    BOOLEAN,
    NUMBER,
    STRING,
    ARRAY,
    OBJECT
}

class JsonValue {
    var type: JsonType;
    var boolValue: bool;
    var numberValue: double;
    var stringValue: string;
    var arrayValue: Array<JsonValue>;
    var objectValue: Map<string, JsonValue>;

    fn isNull() bool;
    fn isBool() bool;
    fn isNumber() bool;
    fn isString() bool;
    fn isArray() bool;
    fn isObject() bool;

    fn asBool() bool;
    fn asNumber() double;
    fn asString() string;
    fn asArray() Array<JsonValue>;
    fn asObject() Map<string, JsonValue>;

    fn get(key: string) JsonValue?;
    fn getAt(index: int) JsonValue?;
}

class JsonEncoder {
    var indent: string;
    var sortKeys: bool;

    fn encode(value: JsonValue) string;
    fn encodeToFile(value: JsonValue, path: string) Result<void, Error>;
}

class JsonDecoder {
    var allowComments: bool;
    var allowTrailingCommas: bool;

    fn decode(json: string) Result<JsonValue, Error>;
    fn decodeFromFile(path: string) Result<JsonValue, Error>;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Native function declarations

// Parsing
fn jsonParse(json: string) Result<JsonValue, Error>;
fn parseFile(path: string) Result<JsonValue, Error>;

// Encoding
fn jsonStringify(value: any) string;
fn jsonStringifyPretty(value: any, indent: string) string;
fn jsonUnmarshal(json: string, prototype: any) any;
fn writeToFile(value: JsonValue, path: string) Result<void, Error>;

// Value constructors
fn jsonNull() JsonValue;
fn jsonBool(value: bool) JsonValue;
fn jsonNumber(value: double) JsonValue;
fn jsonString(value: string) JsonValue;
fn jsonArray(values: Array<JsonValue>) JsonValue;
fn jsonObject(entries: Map<string, JsonValue>) JsonValue;

// Encoder/Decoder creation
fn newEncoder(indent: string, sortKeys: bool) JsonEncoder;
fn newDecoder(allowComments: bool, allowTrailingCommas: bool) JsonDecoder;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Quick parse with default options
fn quickParse(json: string) Result<JsonValue, Error> {
    return jsonParse(json);
}

// Quick stringify
fn quickStringify(value: JsonValue) string {
    return jsonStringify(value);
}

// Marshal any value to JSON (simplified)
fn jsonMarshal(obj: any) string {
    // In real implementation, this would use reflection
    // For now, users need to manually construct JsonValue
    return jsonStringify(obj); // Native handles any
}



// Get nested value by path
// fn getPath(json: JsonValue, path: string) JsonValue? {
//     val parts = strings.split(path, ".");
//     var current = json;
// 
//     for val part in parts {
//         if current.isObject() {
//             val next = current.get(part);
//             if next == none {
//                 return none;
//             }
//             current = next;
//         } else {
//             return none;
//         }
//     }
// 
//     return current;
// }

// Commented out broken pure implementations that use closures/captures
// ... fromMap, fromList, merge, filterArray, mapArray ...

// Validate JSON structure
fn validate(json: string) bool {
    val result = jsonParse(json);
    return result.ok();
}

// Pretty print JSON
fn pretty(json: JsonValue) string {
    return jsonStringifyPretty(json, "  ");
}

// Minify JSON
fn minify(json: string) string {
    val parsed = jsonParse(json);
    if parsed.ok() {
        return jsonStringify(parsed.unwrap());
    }
    return json;
}
