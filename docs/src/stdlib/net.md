---
title: net
---

# Package: net

## Functions

### `readString(maxBytes: int) string`

*Native function*

**Source:** [std/net/init.st:23](std/net/init.st#L23)

---

### `writeString(data: string) int`

*Native function*

**Source:** [std/net/init.st:24](std/net/init.st#L24)

---

### `readLine(conn: Conn) string`

**Source:** [std/net/init.st:135](std/net/init.st#L135)

---

### `writeLine(conn: Conn, line: string) int`

**Source:** [std/net/init.st:157](std/net/init.st#L157)

---

### `copy(dst: Conn, src: Conn) int`

**Source:** [std/net/init.st:162](std/net/init.st#L162)

---

### `isReachable(host: string, port: int, timeoutMs: int) bool`

**Source:** [std/net/init.st:241](std/net/init.st#L241)

---

## Classes

### Class: `IPAddr`

**Properties:**

- `ip: string`
- `port: int`

**Methods:**

#### `toString() string`

#### `isIPv4() bool`

#### `isIPv6() bool`

**Source:** [std/net/init.st:48](std/net/init.st#L48)

---



