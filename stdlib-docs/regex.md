# Package: regex

## Functions

### `compile(pattern: string) int`

*Native function*

**Source:** [std/regex/init.st:35](std/regex/init.st#L35)

---

### `compileWithFlags(pattern: string, flags: int) int`

*Native function*

**Source:** [std/regex/init.st:38](std/regex/init.st#L38)

---

### `matches(pattern: string, text: string) bool`

*Native function*

**Source:** [std/regex/init.st:41](std/regex/init.st#L41)

---

### `contains(pattern: string, text: string) bool`

*Native function*

**Source:** [std/regex/init.st:44](std/regex/init.st#L44)

---

### `find(pattern: string, text: string) int`

*Native function*

**Source:** [std/regex/init.st:48](std/regex/init.st#L48)

---

### `replace(pattern: string, text: string, replacement: string) string`

*Native function*

**Source:** [std/regex/init.st:55](std/regex/init.st#L55)

---

### `replaceAll(pattern: string, text: string, replacement: string) string`

*Native function*

**Source:** [std/regex/init.st:58](std/regex/init.st#L58)

---

### `escape(text: string) string`

*Native function*

**Source:** [std/regex/init.st:71](std/regex/init.st#L71)

---

### `startsWith(pattern: string, text: string) bool`

**Source:** [std/regex/init.st:78](std/regex/init.st#L78)

---

### `endsWith(pattern: string, text: string) bool`

**Source:** [std/regex/init.st:84](std/regex/init.st#L84)

---

### `count(pattern: string, text: string) int`

**Source:** [std/regex/init.st:90](std/regex/init.st#L90)

---

### `isEmail(text: string) bool`

**Source:** [std/regex/init.st:185](std/regex/init.st#L185)

---

### `isURL(text: string) bool`

**Source:** [std/regex/init.st:189](std/regex/init.st#L189)

---

### `isIPv4(text: string) bool`

**Source:** [std/regex/init.st:193](std/regex/init.st#L193)

---

### `isPhoneNumber(text: string) bool`

**Source:** [std/regex/init.st:197](std/regex/init.st#L197)

---

### `isDateISO(text: string) bool`

**Source:** [std/regex/init.st:201](std/regex/init.st#L201)

---

### `isInteger(text: string) bool`

**Source:** [std/regex/init.st:205](std/regex/init.st#L205)

---

### `isDecimal(text: string) bool`

**Source:** [std/regex/init.st:209](std/regex/init.st#L209)

---

### `isHexColor(text: string) bool`

**Source:** [std/regex/init.st:213](std/regex/init.st#L213)

---

### `removeWhitespace(text: string) string`

**Source:** [std/regex/init.st:242](std/regex/init.st#L242)

---

### `normalizeWhitespace(text: string) string`

**Source:** [std/regex/init.st:247](std/regex/init.st#L247)

---

### `stripHTMLTags(text: string) string`

**Source:** [std/regex/init.st:252](std/regex/init.st#L252)

---

### `maskSensitive(text: string, pattern: string, maskChar: string) string`

**Source:** [std/regex/init.st:272](std/regex/init.st#L272)

---

### `camelToSnake(text: string) string`

**Source:** [std/regex/init.st:277](std/regex/init.st#L277)

---

### `snakeToCamel(text: string) string`

**Source:** [std/regex/init.st:283](std/regex/init.st#L283)

---

### `findWithInfo(pattern: string, text: string) MatchInfo`

**Source:** [std/regex/init.st:354](std/regex/init.st#L354)

---

### `isValid(pattern: string) bool`

**Source:** [std/regex/init.st:381](std/regex/init.st#L381)

---

### `quote(text: string) string`

**Source:** [std/regex/init.st:386](std/regex/init.st#L386)

---

### `charClass(chars: string) string`

**Source:** [std/regex/init.st:396](std/regex/init.st#L396)

---

### `negatedCharClass(chars: string) string`

**Source:** [std/regex/init.st:401](std/regex/init.st#L401)

---

### `optional(pattern: string) string`

**Source:** [std/regex/init.st:406](std/regex/init.st#L406)

---

### `oneOrMore(pattern: string) string`

**Source:** [std/regex/init.st:411](std/regex/init.st#L411)

---

### `zeroOrMore(pattern: string) string`

**Source:** [std/regex/init.st:416](std/regex/init.st#L416)

---

### `exactly(pattern: string, n: int) string`

**Source:** [std/regex/init.st:421](std/regex/init.st#L421)

---

### `between(pattern: string, min: int, max: int) string`

**Source:** [std/regex/init.st:426](std/regex/init.st#L426)

---

## Classes

### Class: `Regex`

**Properties:**

- `pattern: string`
- `flags: int`
- `valid: bool`

**Source:** [std/regex/init.st:17](std/regex/init.st#L17)

---

### Class: `MatchInfo`

**Properties:**

- `text: string`
- `start: int`
- `end: int`
- `length: int`

**Source:** [std/regex/init.st:346](std/regex/init.st#L346)

---

## Variables

- `CASE_INSENSITIVE: `

- `MULTILINE: `

- `DOTALL: `

- `EXTENDED: `

- `allMatches: `

- `matches: `

- `result: `

- `replacement: `

- `EMAIL_PATTERN: `

- `URL_PATTERN: `

- `IPV4_PATTERN: `

- `PHONE_PATTERN: `

- `DATE_ISO: `

- `DATE_US: `

- `DATE_EU: `

- `TIME_24H: `

- `HEX_COLOR: `

- `INTEGER: `

- `DECIMAL: `

- `WHITESPACE: `

- `WORD: `

- `pattern: `

- `matches: `

- `i: `

- `substring: `

- `pos: `

- `match: `

- `parts: `

- `i: `

- `remaining: `

- `matches: `

- `matchText: `

