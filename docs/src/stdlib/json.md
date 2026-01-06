---
title: json
---

# Package: json

## Functions

### `isNull() bool`

*Native function*

**Source:** [std/encoding/json/init.st:27](std/encoding/json/init.st#L27)

---

### `isBool() bool`

*Native function*

**Source:** [std/encoding/json/init.st:28](std/encoding/json/init.st#L28)

---

### `isNumber() bool`

*Native function*

**Source:** [std/encoding/json/init.st:29](std/encoding/json/init.st#L29)

---

### `isString() bool`

*Native function*

**Source:** [std/encoding/json/init.st:30](std/encoding/json/init.st#L30)

---

### `isArray() bool`

*Native function*

**Source:** [std/encoding/json/init.st:31](std/encoding/json/init.st#L31)

---

### `isObject() bool`

*Native function*

**Source:** [std/encoding/json/init.st:32](std/encoding/json/init.st#L32)

---

### `asBool() bool`

*Native function*

**Source:** [std/encoding/json/init.st:34](std/encoding/json/init.st#L34)

---

### `asNumber() double`

*Native function*

**Source:** [std/encoding/json/init.st:35](std/encoding/json/init.st#L35)

---

### `asString() string`

*Native function*

**Source:** [std/encoding/json/init.st:36](std/encoding/json/init.st#L36)

---

### `get(key: string) JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:40](std/encoding/json/init.st#L40)

---

### `getAt(index: int) JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:41](std/encoding/json/init.st#L41)

---

### `stringify(value: JsonValue) string`

*Native function*

**Source:** [std/encoding/json/init.st:69](std/encoding/json/init.st#L69)

---

### `stringifyPretty(value: JsonValue, indent: string) string`

*Native function*

**Source:** [std/encoding/json/init.st:70](std/encoding/json/init.st#L70)

---

### `jsonNull() JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:74](std/encoding/json/init.st#L74)

---

### `jsonBool(value: bool) JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:75](std/encoding/json/init.st#L75)

---

### `jsonNumber(value: double) JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:76](std/encoding/json/init.st#L76)

---

### `jsonString(value: string) JsonValue`

*Native function*

**Source:** [std/encoding/json/init.st:77](std/encoding/json/init.st#L77)

---

### `newEncoder(indent: string, sortKeys: bool) JsonEncoder`

*Native function*

**Source:** [std/encoding/json/init.st:82](std/encoding/json/init.st#L82)

---

### `newDecoder(allowComments: bool, allowTrailingCommas: bool) JsonDecoder`

*Native function*

**Source:** [std/encoding/json/init.st:83](std/encoding/json/init.st#L83)

---

### `quickStringify(value: JsonValue) string`

**Source:** [std/encoding/json/init.st:95](std/encoding/json/init.st#L95)

---

### `marshal(obj: any) string`

**Source:** [std/encoding/json/init.st:100](std/encoding/json/init.st#L100)

---

### `getPath(json: JsonValue, path: string) JsonValue`

**Source:** [std/encoding/json/init.st:112](std/encoding/json/init.st#L112)

---

### `getStringPath(json: JsonValue, path: string) string`

**Source:** [std/encoding/json/init.st:132](std/encoding/json/init.st#L132)

---

### `getNumberPath(json: JsonValue, path: string) double`

**Source:** [std/encoding/json/init.st:141](std/encoding/json/init.st#L141)

---

### `getBoolPath(json: JsonValue, path: string) bool`

**Source:** [std/encoding/json/init.st:150](std/encoding/json/init.st#L150)

---

### `merge(a: JsonValue, b: JsonValue) JsonValue`

**Source:** [std/encoding/json/init.st:185](std/encoding/json/init.st#L185)

---

### `validate(json: string) bool`

**Source:** [std/encoding/json/init.st:242](std/encoding/json/init.st#L242)

---

### `pretty(json: JsonValue) string`

**Source:** [std/encoding/json/init.st:248](std/encoding/json/init.st#L248)

---

### `minify(json: string) string`

**Source:** [std/encoding/json/init.st:253](std/encoding/json/init.st#L253)

---



