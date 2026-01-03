# Package: strings

## Functions

### `toUpper(s: string) string`

*Native function*

**Source:** [std/strings/init.st:11](std/strings/init.st#L11)

---

### `toLower(s: string) string`

*Native function*

**Source:** [std/strings/init.st:12](std/strings/init.st#L12)

---

### `toTitle(s: string) string`

*Native function*

**Source:** [std/strings/init.st:13](std/strings/init.st#L13)

---

### `trim(s: string) string`

*Native function*

**Source:** [std/strings/init.st:16](std/strings/init.st#L16)

---

### `trimLeft(s: string) string`

*Native function*

**Source:** [std/strings/init.st:17](std/strings/init.st#L17)

---

### `trimRight(s: string) string`

*Native function*

**Source:** [std/strings/init.st:18](std/strings/init.st#L18)

---

### `trimPrefix(s: string, prefix: string) string`

*Native function*

**Source:** [std/strings/init.st:19](std/strings/init.st#L19)

---

### `trimSuffix(s: string, suffix: string) string`

*Native function*

**Source:** [std/strings/init.st:20](std/strings/init.st#L20)

---

### `contains(s: string, substr: string) bool`

*Native function*

**Source:** [std/strings/init.st:27](std/strings/init.st#L27)

---

### `indexOf(s: string, substr: string) int`

*Native function*

**Source:** [std/strings/init.st:28](std/strings/init.st#L28)

---

### `lastIndexOf(s: string, substr: string) int`

*Native function*

**Source:** [std/strings/init.st:29](std/strings/init.st#L29)

---

### `startsWith(s: string, prefix: string) bool`

*Native function*

**Source:** [std/strings/init.st:30](std/strings/init.st#L30)

---

### `endsWith(s: string, suffix: string) bool`

*Native function*

**Source:** [std/strings/init.st:31](std/strings/init.st#L31)

---

### `count(s: string, substr: string) int`

*Native function*

**Source:** [std/strings/init.st:32](std/strings/init.st#L32)

---

### `replace(s: string, old: string, new: string) string`

*Native function*

**Source:** [std/strings/init.st:35](std/strings/init.st#L35)

---

### `replaceAll(s: string, old: string, new: string) string`

*Native function*

**Source:** [std/strings/init.st:36](std/strings/init.st#L36)

---

### `substring(s: string, start: int, end: int) string`

*Native function*

**Source:** [std/strings/init.st:39](std/strings/init.st#L39)

---

### `charAt(s: string, index: int) string`

*Native function*

**Source:** [std/strings/init.st:40](std/strings/init.st#L40)

---

### `repeat(s: string, count: int) string`

*Native function*

**Source:** [std/strings/init.st:43](std/strings/init.st#L43)

---

### `padLeft(s: string, length: int, pad: string) string`

*Native function*

**Source:** [std/strings/init.st:44](std/strings/init.st#L44)

---

### `padRight(s: string, length: int, pad: string) string`

*Native function*

**Source:** [std/strings/init.st:45](std/strings/init.st#L45)

---

### `compare(a: string, b: string) int`

*Native function*

**Source:** [std/strings/init.st:48](std/strings/init.st#L48)

---

### `equals(a: string, b: string) bool`

*Native function*

**Source:** [std/strings/init.st:49](std/strings/init.st#L49)

---

### `equalsIgnoreCase(a: string, b: string) bool`

*Native function*

**Source:** [std/strings/init.st:50](std/strings/init.st#L50)

---

### `length(s: string) int`

*Native function*

**Source:** [std/strings/init.st:53](std/strings/init.st#L53)

---

### `isEmpty(s: string) bool`

*Native function*

**Source:** [std/strings/init.st:54](std/strings/init.st#L54)

---

### `isBlank(s: string) bool`

*Native function*

**Source:** [std/strings/init.st:55](std/strings/init.st#L55)

---

### `reverse(s: string) string`

*Native function*

**Source:** [std/strings/init.st:56](std/strings/init.st#L56)

---

### `isDigit(s: string) bool`

**Source:** [std/strings/init.st:63](std/strings/init.st#L63)

---

### `isAlpha(s: string) bool`

**Source:** [std/strings/init.st:78](std/strings/init.st#L78)

---

### `isAlphaNumeric(s: string) bool`

**Source:** [std/strings/init.st:95](std/strings/init.st#L95)

---

### `capitalize(s: string) string`

**Source:** [std/strings/init.st:109](std/strings/init.st#L109)

---

### `uncapitalize(s: string) string`

**Source:** [std/strings/init.st:120](std/strings/init.st#L120)

---

### `toCamelCase(s: string) string`

**Source:** [std/strings/init.st:131](std/strings/init.st#L131)

---

### `toPascalCase(s: string) string`

**Source:** [std/strings/init.st:147](std/strings/init.st#L147)

---

### `toSnakeCase(s: string) string`

**Source:** [std/strings/init.st:159](std/strings/init.st#L159)

---

### `toKebabCase(s: string) string`

**Source:** [std/strings/init.st:175](std/strings/init.st#L175)

---

### `truncate(s: string, maxLength: int, suffix: string) string`

**Source:** [std/strings/init.st:191](std/strings/init.st#L191)

---

### `removeWhitespace(s: string) string`

**Source:** [std/strings/init.st:201](std/strings/init.st#L201)

---

### `wordCount(s: string) int`

**Source:** [std/strings/init.st:206](std/strings/init.st#L206)

---

### `indent(s: string, spaces: int) string`

**Source:** [std/strings/init.st:222](std/strings/init.st#L222)

---

### `center(s: string, width: int, fill: string) string`

**Source:** [std/strings/init.st:238](std/strings/init.st#L238)

---

## Variables

- `digits: `

- `lower: `

- `letters: `

- `first: `

- `rest: `

- `first: `

- `rest: `

- `truncated: `

- `words: `

- `padding: `

- `leftPad: `

- `rightPad: `

- `first: `

- `prefix: `

- `char: `

- `match: `

