// Crypto module - native bindings
package crypto;

// Cryptographic functions (native implementations)
fn randomBytes(length: int) Array<byte>;
fn randomHex(length: int) string;
fn md5String(data: string) string;
fn sha256String(data: string) string;
fn bcrypt(password: string, rounds: int) string;
fn bcryptVerify(password: string, hash: string) bool;
