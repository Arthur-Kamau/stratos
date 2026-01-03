# Package: crypto

## Functions

### `updateString(s: string) Hash`

*Native function*

**Source:** [std/crypto/init.st:15](std/crypto/init.st#L15)

---

### `digestHex() string`

*Native function*

**Source:** [std/crypto/init.st:17](std/crypto/init.st#L17)

---

### `digestBase64() string`

*Native function*

**Source:** [std/crypto/init.st:18](std/crypto/init.st#L18)

---

### `encryptString(plaintext: string) string`

*Native function*

**Source:** [std/crypto/init.st:28](std/crypto/init.st#L28)

---

### `decryptString(ciphertext: string) string`

*Native function*

**Source:** [std/crypto/init.st:29](std/crypto/init.st#L29)

---

### `digestHex() string`

*Native function*

**Source:** [std/crypto/init.st:38](std/crypto/init.st#L38)

---

### `md5String(s: string) string`

*Native function*

**Source:** [std/crypto/init.st:57](std/crypto/init.st#L57)

---

### `sha1String(s: string) string`

*Native function*

**Source:** [std/crypto/init.st:58](std/crypto/init.st#L58)

---

### `sha256String(s: string) string`

*Native function*

**Source:** [std/crypto/init.st:59](std/crypto/init.st#L59)

---

### `sha512String(s: string) string`

*Native function*

**Source:** [std/crypto/init.st:60](std/crypto/init.st#L60)

---

### `newHash(algorithm: string) Hash`

*Native function*

**Source:** [std/crypto/init.st:63](std/crypto/init.st#L63)

---

### `randomHex(length: int) string`

*Native function*

**Source:** [std/crypto/init.st:75](std/crypto/init.st#L75)

---

### `randomBase64(length: int) string`

*Native function*

**Source:** [std/crypto/init.st:76](std/crypto/init.st#L76)

---

### `bcrypt(password: string, rounds: int) string`

*Native function*

**Source:** [std/crypto/init.st:80](std/crypto/init.st#L80)

---

### `bcryptVerify(password: string, hash: string) bool`

*Native function*

**Source:** [std/crypto/init.st:81](std/crypto/init.st#L81)

---

### `generateRSAKeyPair(bits: int) KeyPair`

*Native function*

**Source:** [std/crypto/init.st:84](std/crypto/init.st#L84)

---

### `quickMD5(s: string) string`

**Source:** [std/crypto/init.st:95](std/crypto/init.st#L95)

---

### `quickSHA256(s: string) string`

**Source:** [std/crypto/init.st:99](std/crypto/init.st#L99)

---

### `hashPassword(password: string) string`

**Source:** [std/crypto/init.st:147](std/crypto/init.st#L147)

---

### `verifyPassword(password: string, hash: string) bool`

**Source:** [std/crypto/init.st:151](std/crypto/init.st#L151)

---

### `generateToken(length: int) string`

**Source:** [std/crypto/init.st:156](std/crypto/init.st#L156)

---

### `byteToHex(b: int) string`

**Source:** [std/crypto/init.st:161](std/crypto/init.st#L161)

---

### `uuid() string`

**Source:** [std/crypto/init.st:169](std/crypto/init.st#L169)

---

### `secureCompare(a: string, b: string) bool`

**Source:** [std/crypto/init.st:191](std/crypto/init.st#L191)

---

### `newAuthToken(secret: string) AuthToken`

**Source:** [std/crypto/init.st:226](std/crypto/init.st#L226)

---

### `newJWT(secret: string) JWTToken`

**Source:** [std/crypto/init.st:300](std/crypto/init.st#L300)

---

### `encryptString(plaintext: string, password: string) string`

**Source:** [std/crypto/init.st:305](std/crypto/init.st#L305)

---

## Classes

### Class: `AuthToken`

**Properties:**

- `secret: string`
- `algorithm: string`

**Methods:**

#### `constructor(secretKey: string) void`

#### `sign(message: string) string`

#### `verify(message: string, signature: string) bool`

**Source:** [std/crypto/init.st:205](std/crypto/init.st#L205)

---

## Variables

- `content: `

- `hash: `

- `computed: `

- `match: `

- `result: `

- `headerJson: `

- `payloadJson: `

- `headerB64: `

- `payloadB64: `

- `message: `

- `hmac: `

- `signature: `

- `parts: `

- `message: `

- `hmac: `

- `expectedSig: `

- `payloadJson: `

- `parts: `

- `saltResult: `

- `ivResult: `

- `key: `

- `cipher: `

- `plaintext: `

- `content: `

- `encrypted: `

- `encrypted: `

- `decrypted: `

