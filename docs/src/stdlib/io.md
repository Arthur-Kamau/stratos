---
title: io
---

# Package: io

## Functions

### `ok() bool`

**Source:** [std/io/init.st:26](std/io/init.st#L26)

---

### `err() E`

**Source:** [std/io/init.st:30](std/io/init.st#L30)

---

### `unwrap() T`

**Source:** [std/io/init.st:34](std/io/init.st#L34)

---

### `exists(path: string) bool`

*Native function*

**Source:** [std/io/init.st:85](std/io/init.st#L85)

---

### `isFile(path: string) bool`

*Native function*

**Source:** [std/io/init.st:86](std/io/init.st#L86)

---

### `isDirectory(path: string) bool`

*Native function*

**Source:** [std/io/init.st:87](std/io/init.st#L87)

---

### `basename(path: string) string`

*Native function*

**Source:** [std/io/init.st:93](std/io/init.st#L93)

---

### `dirname(path: string) string`

*Native function*

**Source:** [std/io/init.st:94](std/io/init.st#L94)

---

### `extension(path: string) string`

*Native function*

**Source:** [std/io/init.st:95](std/io/init.st#L95)

---

### `absolute(path: string) string`

*Native function*

**Source:** [std/io/init.st:96](std/io/init.st#L96)

---

### `hasExtension(path: string, ext: string) bool`

**Source:** [std/io/init.st:139](std/io/init.st#L139)

---

## Classes

### Class: `Error`

**Properties:**

- `message: string`

**Methods:**

#### `constructor(msg: string) void`

**Source:** [std/io/init.st:42](std/io/init.st#L42)

---

### Class: `FileInfo`

**Properties:**

- `name: string`
- `size: int`
- `isDirectory: bool`
- `modTime: int`

**Source:** [std/io/init.st:50](std/io/init.st#L50)

---



