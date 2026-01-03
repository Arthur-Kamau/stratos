# Package: terminal

## Functions

### `getSize() TerminalSize`

*Native function*

**Source:** [std/terminal/init.st:159](std/terminal/init.st#L159)

---

### `getWidth() int`

*Native function*

**Source:** [std/terminal/init.st:162](std/terminal/init.st#L162)

---

### `getHeight() int`

*Native function*

**Source:** [std/terminal/init.st:165](std/terminal/init.st#L165)

---

### `supportsColor() bool`

*Native function*

**Source:** [std/terminal/init.st:168](std/terminal/init.st#L168)

---

### `isTTY() bool`

*Native function*

**Source:** [std/terminal/init.st:171](std/terminal/init.st#L171)

---

### `readChar() string`

*Native function*

**Source:** [std/terminal/init.st:184](std/terminal/init.st#L184)

---

### `readKey() Key`

*Native function*

**Source:** [std/terminal/init.st:187](std/terminal/init.st#L187)

---

### `readLine() string`

*Native function*

**Source:** [std/terminal/init.st:190](std/terminal/init.st#L190)

---

### `input(prompt: string) string`

*Native function*

**Source:** [std/terminal/init.st:193](std/terminal/init.st#L193)

---

### `hasInput() bool`

*Native function*

**Source:** [std/terminal/init.st:196](std/terminal/init.st#L196)

---

### `colored(text: string, fg: Color) string`

**Source:** [std/terminal/init.st:300](std/terminal/init.st#L300)

---

### `coloredBg(text: string, fg: Color, bg: Color) string`

**Source:** [std/terminal/init.st:306](std/terminal/init.st#L306)

---

### `getColorCode(color: Color, background: bool) string`

**Source:** [std/terminal/init.st:313](std/terminal/init.st#L313)

---

### `confirm(message: string) bool`

**Source:** [std/terminal/init.st:489](std/terminal/init.st#L489)

---

### `next() string`

**Source:** [std/terminal/init.st:521](std/terminal/init.st#L521)

---

### `newSpinner() Spinner`

**Source:** [std/terminal/init.st:528](std/terminal/init.st#L528)

---

## Classes

### Class: `TerminalSize`

**Properties:**

- `width: int`
- `height: int`

**Source:** [std/terminal/init.st:42](std/terminal/init.st#L42)

---

### Class: `Key`

**Properties:**

- `char: string`
- `code: int`
- `ctrl: bool`
- `alt: bool`
- `shift: bool`

**Source:** [std/terminal/init.st:47](std/terminal/init.st#L47)

---

## Variables

- `KEY_UP: `

- `KEY_DOWN: `

- `KEY_LEFT: `

- `KEY_RIGHT: `

- `KEY_ENTER: `

- `KEY_ESC: `

- `KEY_BACKSPACE: `

- `KEY_TAB: `

- `KEY_DELETE: `

- `KEY_HOME: `

- `KEY_END: `

- `KEY_PAGE_UP: `

- `KEY_PAGE_DOWN: `

- `KEY_F1: `

- `KEY_F2: `

- `KEY_F3: `

- `KEY_F4: `

- `KEY_F5: `

- `KEY_F6: `

- `KEY_F7: `

- `KEY_F8: `

- `KEY_F9: `

- `KEY_F10: `

- `KEY_F11: `

- `KEY_F12: `

- `i: `

- `i: `

- `i: `

- `i: `

- `width: `

- `col: `

- `percentage: `

- `filled: `

- `i: `

- `i: `

- `selected: `

- `i: `

- `item: `

- `i: `

- `key: `

- `selected: `

- `selected: `

- `value: `

- `currentFrame: int`

- `running: bool`

- `i: `

- `i: `

- `j: `

- `len: `

- `j: `

- `k: `

- `k: `

- `l: `

- `l: `

- `m: `

- `m: `

