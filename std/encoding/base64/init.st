// Base64 module - Base64 encoding and decoding
// Inspired by Go's encoding/base64 package

package base64;

// ============================================================================
// Types
// ============================================================================

enum Encoding {
    STANDARD,     // Standard base64 encoding (RFC 4648)
    URL_SAFE,     // URL-safe base64 encoding
    NO_PADDING    // Standard encoding without padding
}

// ============================================================================
// Native function declarations
// ============================================================================

// Standard encoding
fn encode(data: Array<byte>) string;
fn decode(encoded: string) Result<Array<byte>, Error>;

// String encoding/decoding
fn encodeString(s: string) string;
fn decodeString(encoded: string) Result<string, Error>;

// Custom encoding
fn encodeWith(data: Array<byte>, encoding: Encoding) string;
fn decodeWith(encoded: string, encoding: Encoding) Result<Array<byte>, Error>;

// URL-safe encoding
fn encodeURL(data: Array<byte>) string;
fn decodeURL(encoded: string) Result<Array<byte>, Error>;

// Encoding without padding
fn encodeNoPad(data: Array<byte>) string;
fn decodeNoPad(encoded: string) Result<Array<byte>, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Quick encode string
fn quickEncode(s: string) string {
    return encodeString(s);
}

// Quick decode to string
fn quickDecode(encoded: string) string {
    val result = decodeString(encoded);
    if result.ok() {
        return result.unwrap();
    }
    return "";
}

// Encode file
fn encodeFile(path: string) Result<string, Error> {
    val content = io.readBytes(path);

    if !content.ok() {
        return io.Result<string, Error> {
            value: "",
            error: content.err(),
            isOk: false
        };
    }

    val encoded = encode(content.unwrap());

    return io.Result<string, Error> {
        value: encoded,
        error: null,
        isOk: true
    };
}

// Decode to file
fn decodeToFile(encoded: string, path: string) Result<void, Error> {
    val decoded = decode(encoded);

    if !decoded.ok() {
        return io.Result<void, Error> {
            value: null,
            error: decoded.err(),
            isOk: false
        };
    }

    return io.writeBytes(path, decoded.unwrap());
}

// Encode with line breaks (for readability)
fn encodeMultiline(data: Array<byte>, lineLength: int) string {
    val encoded = encode(data);
    var result = "";
    var pos = 0;

    while pos < strings.length(encoded) {
        val remaining = strings.length(encoded) - pos;
        val chunkSize = math.min(lineLength, remaining);
        val chunk = strings.substring(encoded, pos, pos + chunkSize);

        result = result + chunk;
        pos = pos + chunkSize;

        if pos < strings.length(encoded) {
            result = result + "\n";
        }
    }

    return result;
}

// Decode multiline (ignore whitespace)
fn decodeMultiline(encoded: string) Result<Array<byte>, Error> {
    val cleaned = strings.removeWhitespace(encoded);
    return decode(cleaned);
}

// Validate base64 string
fn isValid(encoded: string) bool {
    val result = decode(encoded);
    return result.ok();
}

// Get decoded size
fn decodedSize(encoded: string) int {
    val len = strings.length(encoded);
    var padding = 0;

    if len > 0 && strings.charAt(encoded, len - 1) == '=' {
        padding = padding + 1;
    }
    if len > 1 && strings.charAt(encoded, len - 2) == '=' {
        padding = padding + 1;
    }

    return (len * 3) / 4 - padding;
}

// Get encoded size
fn encodedSize(dataLength: int) int {
    return ((dataLength + 2) / 3) * 4;
}
