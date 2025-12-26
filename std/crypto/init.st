// Crypto module - Cryptographic functions
// Inspired by Go's crypto package and Node.js crypto module

package crypto;

// ============================================================================
// Types
// ============================================================================

class Hash {
    var algorithm: string;
    var data: Array<byte>;

    fn update(data: Array<byte>) Hash;
    fn updateString(s: string) Hash;
    fn digest() Array<byte>;
    fn digestHex() string;
    fn digestBase64() string;
}

class Cipher {
    var algorithm: string;
    var key: Array<byte>;
    var iv: Array<byte>;

    fn encrypt(plaintext: Array<byte>) Array<byte>;
    fn decrypt(ciphertext: Array<byte>) Array<byte>;
    fn encryptString(plaintext: string) string;
    fn decryptString(ciphertext: string) string;
}

class HMAC {
    var algorithm: string;
    var key: Array<byte>;

    fn update(data: Array<byte>) HMAC;
    fn digest() Array<byte>;
    fn digestHex() string;
}

class KeyPair {
    var publicKey: Array<byte>;
    var privateKey: Array<byte>;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Hashing
fn md5(data: Array<byte>) Array<byte>;
fn sha1(data: Array<byte>) Array<byte>;
fn sha256(data: Array<byte>) Array<byte>;
fn sha512(data: Array<byte>) Array<byte>;

// String hashing (returns hex)
fn md5String(s: string) string;
fn sha1String(s: string) string;
fn sha256String(s: string) string;
fn sha512String(s: string) string;

// Hash creation
fn newHash(algorithm: string) Hash;
fn newHMAC(algorithm: string, key: Array<byte>) HMAC;

// Encryption/Decryption
fn aesEncrypt(plaintext: Array<byte>, key: Array<byte>, iv: Array<byte>) Array<byte>;
fn aesDecrypt(ciphertext: Array<byte>, key: Array<byte>, iv: Array<byte>) Array<byte>;

// Cipher creation
fn newCipher(algorithm: string, key: Array<byte>, iv: Array<byte>) Cipher;

// Random generation
fn randomBytes(length: int) Array<byte>;
fn randomHex(length: int) string;
fn randomBase64(length: int) string;

// Key derivation
fn pbkdf2(password: string, salt: Array<byte>, iterations: int, keyLength: int, algorithm: string) Array<byte>;
fn bcrypt(password: string, rounds: int) string;
fn bcryptVerify(password: string, hash: string) bool;

// RSA
fn generateRSAKeyPair(bits: int) KeyPair;
fn rsaEncrypt(plaintext: Array<byte>, publicKey: Array<byte>) Array<byte>;
fn rsaDecrypt(ciphertext: Array<byte>, privateKey: Array<byte>) Array<byte>;
fn rsaSign(data: Array<byte>, privateKey: Array<byte>) Array<byte>;
fn rsaVerify(data: Array<byte>, signature: Array<byte>, publicKey: Array<byte>) bool;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Quick hash functions
fn quickMD5(s: string) string {
    return md5String(s);
}

fn quickSHA256(s: string) string {
    return sha256String(s);
}

// Hash file
fn hashFile(path: string, algorithm: string) Result<string, Error> {
    val content = io.readBytes(path);

    if !content.ok() {
        return io.Result<string, Error> {
            value: "",
            error: content.err(),
            isOk: false
        };
    }

    val hash = newHash(algorithm);
    hash.update(content.unwrap());

    return io.Result<string, Error> {
        value: hash.digestHex(),
        error: null,
        isOk: true
    };
}

// Checksum verification
fn verifyChecksum(path: string, expectedHash: string, algorithm: string) Result<bool, Error> {
    val computed = hashFile(path, algorithm);

    if !computed.ok() {
        return io.Result<bool, Error> {
            value: false,
            error: computed.err(),
            isOk: false
        };
    }

    val match = computed.unwrap() == expectedHash;

    return io.Result<bool, Error> {
        value: match,
        error: null,
        isOk: true
    };
}

// Password hashing
fn hashPassword(password: string) string {
    return bcrypt(password, 10);
}

fn verifyPassword(password: string, hash: string) bool {
    return bcryptVerify(password, hash);
}

// Generate secure token
fn generateToken(length: int) string {
    return randomHex(length);
}

// Generate UUID v4
fn uuid() string {
    val bytes = randomBytes(16);

    // Set version to 4
    bytes[6] = (bytes[6] & 0x0f) | 0x40;

    // Set variant to RFC 4122
    bytes[8] = (bytes[8] & 0x3f) | 0x80;

    // Format as UUID string
    var result = "";
    for i in 0..bytes.length() {
        if i == 4 || i == 6 || i == 8 || i == 10 {
            result = result + "-";
        }
        // Convert byte to hex
        result = result + "00"; // Simplified - need proper hex conversion
    }

    return result;
}

// Constant-time comparison
fn secureCompare(a: string, b: string) bool {
    if strings.length(a) != strings.length(b) {
        return false;
    }

    var result = 0;
    for i in 0..strings.length(a) {
        result = result | (strings.charAt(a, i) ^ strings.charAt(b, i));
    }

    return result == 0;
}

// HMAC-based authentication
class AuthToken {
    var secret: string;
    var algorithm: string;

    constructor(secretKey: string) {
        this.secret = secretKey;
        this.algorithm = "sha256";
    }

    fn sign(message: string) string {
        val hmac = newHMAC(this.algorithm, this.secret.bytes());
        hmac.update(message.bytes());
        return hmac.digestHex();
    }

    fn verify(message: string, signature: string) bool {
        val expected = this.sign(message);
        return secureCompare(expected, signature);
    }
}

fn newAuthToken(secret: string) AuthToken {
    return AuthToken(secret);
}

// JWT-like token generation (simplified)
class JWTToken {
    var secret: string;

    constructor(secretKey: string) {
        this.secret = secretKey;
    }

    fn encode(payload: json.JsonValue) string {
        // Header
        val header = json.jsonObject({
            typ: json.jsonString("JWT"),
            alg: json.jsonString("HS256")
        });

        val headerJson = json.stringify(header);
        val payloadJson = json.stringify(payload);

        // Base64 encode
        val headerB64 = base64.encodeString(headerJson);
        val payloadB64 = base64.encodeString(payloadJson);

        // Create signature
        val message = headerB64 + "." + payloadB64;
        val hmac = newHMAC("sha256", this.secret.bytes());
        hmac.update(message.bytes());
        val signature = base64.encode(hmac.digest());

        return message + "." + signature;
    }

    fn decode(token: string) Result<json.JsonValue, Error> {
        val parts = strings.split(token, ".");

        if parts.length() != 3 {
            return io.Result<json.JsonValue, Error> {
                value: null,
                error: io.Error("Invalid token format"),
                isOk: false
            };
        }

        // Verify signature
        val message = parts[0] + "." + parts[1];
        val hmac = newHMAC("sha256", this.secret.bytes());
        hmac.update(message.bytes());
        val expectedSig = base64.encode(hmac.digest());

        if !secureCompare(expectedSig, parts[2]) {
            return io.Result<json.JsonValue, Error> {
                value: null,
                error: io.Error("Invalid signature"),
                isOk: false
            };
        }

        // Decode payload
        val payloadJson = base64.decodeString(parts[1]);
        if !payloadJson.ok() {
            return io.Result<json.JsonValue, Error> {
                value: null,
                error: payloadJson.err(),
                isOk: false
            };
        }

        return json.parse(payloadJson.unwrap());
    }
}

fn newJWT(secret: string) JWTToken {
    return JWTToken(secret);
}

// Encryption helpers
fn encryptString(plaintext: string, password: string) string {
    // Generate salt and IV
    val salt = randomBytes(16);
    val iv = randomBytes(16);

    // Derive key from password
    val key = pbkdf2(password, salt, 10000, 32, "sha256");

    // Encrypt
    val cipher = newCipher("aes-256-cbc", key, iv);
    val ciphertext = cipher.encryptString(plaintext);

    // Combine salt + iv + ciphertext
    return base64.encode(salt) + ":" + base64.encode(iv) + ":" + ciphertext;
}

fn decryptString(encrypted: string, password: string) Result<string, Error> {
    val parts = strings.split(encrypted, ":");

    if parts.length() != 3 {
        return io.Result<string, Error> {
            value: "",
            error: io.Error("Invalid encrypted format"),
            isOk: false
        };
    }

    // Extract salt, IV, and ciphertext
    val saltResult = base64.decode(parts[0]);
    val ivResult = base64.decode(parts[1]);

    if !saltResult.ok() || !ivResult.ok() {
        return io.Result<string, Error> {
            value: "",
            error: io.Error("Invalid base64 encoding"),
            isOk: false
        };
    }

    // Derive key
    val key = pbkdf2(password, saltResult.unwrap(), 10000, 32, "sha256");

    // Decrypt
    val cipher = newCipher("aes-256-cbc", key, ivResult.unwrap());
    val plaintext = cipher.decryptString(parts[2]);

    return io.Result<string, Error> {
        value: plaintext,
        error: null,
        isOk: true
    };
}

// File encryption
fn encryptFile(inputPath: string, outputPath: string, password: string) Result<void, Error> {
    val content = io.readFile(inputPath);

    if !content.ok() {
        return io.Result<void, Error> {
            value: null,
            error: content.err(),
            isOk: false
        };
    }

    val encrypted = encryptString(content.unwrap(), password);
    return io.writeFile(outputPath, encrypted);
}

fn decryptFile(inputPath: string, outputPath: string, password: string) Result<void, Error> {
    val encrypted = io.readFile(inputPath);

    if !encrypted.ok() {
        return io.Result<void, Error> {
            value: null,
            error: encrypted.err(),
            isOk: false
        };
    }

    val decrypted = decryptString(encrypted.unwrap(), password);

    if !decrypted.ok() {
        return io.Result<void, Error> {
            value: null,
            error: decrypted.err(),
            isOk: false
        };
    }

    return io.writeFile(outputPath, decrypted.unwrap());
}
