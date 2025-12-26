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

// Parsing
fn parse(json: string) Result<JsonValue, Error>;
fn parseFile(path: string) Result<JsonValue, Error>;

// Encoding
fn stringify(value: JsonValue) string;
fn stringifyPretty(value: JsonValue, indent: string) string;
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
    return parse(json);
}

// Quick stringify
fn quickStringify(value: JsonValue) string {
    return stringify(value);
}

// Marshal any value to JSON (simplified)
fn marshal(obj: any) string {
    // In real implementation, this would use reflection
    // For now, users need to manually construct JsonValue
    return stringify(obj);
}

// Unmarshal JSON to value
fn unmarshal(json: string) Result<JsonValue, Error> {
    return parse(json);
}

// Get nested value by path
fn getPath(json: JsonValue, path: string) JsonValue? {
    val parts = strings.split(path, ".");
    var current = json;

    for val part in parts {
        if current.isObject() {
            val next = current.get(part);
            if next == null {
                return null;
            }
            current = next;
        } else {
            return null;
        }
    }

    return current;
}

// Get string value by path
fn getStringPath(json: JsonValue, path: string) string {
    val value = getPath(json, path);
    if value != null && value.isString() {
        return value.asString();
    }
    return "";
}

// Get number value by path
fn getNumberPath(json: JsonValue, path: string) double {
    val value = getPath(json, path);
    if value != null && value.isNumber() {
        return value.asNumber();
    }
    return 0.0;
}

// Get bool value by path
fn getBoolPath(json: JsonValue, path: string) bool {
    val value = getPath(json, path);
    if value != null && value.isBool() {
        return value.asBool();
    }
    return false;
}

// Create JSON from map
fn fromMap(map: Map<string, any>) JsonValue {
    val obj = collections.newMap<string, JsonValue>();

    map.keys().forEach((key) => {
        val value = map.get(key);
        // Convert value to JsonValue based on type
        // This is simplified - real implementation would check types
        obj.put(key, value);
    });

    return jsonObject(obj);
}

// Create JSON array from list
fn fromList(list: Array<any>) JsonValue {
    val arr = collections.newList<JsonValue>();

    for val item in list {
        // Convert each item to JsonValue
        arr.add(item);
    }

    return jsonArray(arr.items);
}

// Merge two JSON objects
fn merge(a: JsonValue, b: JsonValue) JsonValue {
    if !a.isObject() || !b.isObject() {
        return b;
    }

    val result = collections.newMap<string, JsonValue>();

    // Copy all from a
    val aObj = a.asObject();
    aObj.keys().forEach((key) => {
        result.put(key, aObj.get(key));
    });

    // Merge from b
    val bObj = b.asObject();
    bObj.keys().forEach((key) => {
        result.put(key, bObj.get(key));
    });

    return jsonObject(result);
}

// Filter JSON array
fn filterArray(json: JsonValue, predicate: Function<JsonValue, bool>) JsonValue {
    if !json.isArray() {
        return json;
    }

    val arr = json.asArray();
    var filtered = Array<JsonValue>();

    for val item in arr {
        if predicate(item) {
            filtered.add(item);
        }
    }

    return jsonArray(filtered);
}

// Map over JSON array
fn mapArray(json: JsonValue, fn: Function<JsonValue, JsonValue>) JsonValue {
    if !json.isArray() {
        return json;
    }

    val arr = json.asArray();
    var mapped = Array<JsonValue>();

    for val item in arr {
        mapped.add(fn(item));
    }

    return jsonArray(mapped);
}

// Validate JSON structure
fn validate(json: string) bool {
    val result = parse(json);
    return result.ok();
}

// Pretty print JSON
fn pretty(json: JsonValue) string {
    return stringifyPretty(json, "  ");
}

// Minify JSON
fn minify(json: string) string {
    val parsed = parse(json);
    if parsed.ok() {
        return stringify(parsed.unwrap());
    }
    return json;
}
