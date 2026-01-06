---
title: time
---

# Package: time

## Functions

### `now() Time`

*Native function*

**Source:** [std/time/init.st:62](std/time/init.st#L62)

---

### `unix(seconds: int) Time`

*Native function*

**Source:** [std/time/init.st:63](std/time/init.st#L63)

---

### `fromMillis(millis: int) Time`

*Native function*

**Source:** [std/time/init.st:64](std/time/init.st#L64)

---

### `milliseconds(ms: int) Duration`

*Native function*

**Source:** [std/time/init.st:70](std/time/init.st#L70)

---

### `seconds(s: int) Duration`

*Native function*

**Source:** [std/time/init.st:71](std/time/init.st#L71)

---

### `minutes(m: int) Duration`

*Native function*

**Source:** [std/time/init.st:72](std/time/init.st#L72)

---

### `hours(h: int) Duration`

*Native function*

**Source:** [std/time/init.st:73](std/time/init.st#L73)

---

### `days(d: int) Duration`

*Native function*

**Source:** [std/time/init.st:74](std/time/init.st#L74)

---

### `tick(interval: Duration) Ticker`

*Native function*

**Source:** [std/time/init.st:82](std/time/init.st#L82)

---

### `newTimer(duration: Duration) Timer`

*Native function*

**Source:** [std/time/init.st:83](std/time/init.st#L83)

---

### `newTicker(interval: Duration) Ticker`

*Native function*

**Source:** [std/time/init.st:84](std/time/init.st#L84)

---

### `date(year: int, month: int, day: int) Time`

*Native function*

**Source:** [std/time/init.st:87](std/time/init.st#L87)

---

### `dateTime(year: int, month: int, day: int, hour: int, minute: int, second: int) Time`

*Native function*

**Source:** [std/time/init.st:88](std/time/init.st#L88)

---

### `isLeapYear(year: int) bool`

**Source:** [std/time/init.st:108](std/time/init.st#L108)

---

### `daysInMonth(year: int, month: int) int`

**Source:** [std/time/init.st:119](std/time/init.st#L119)

---

### `addDays(t: Time, numDays: int) Time`

**Source:** [std/time/init.st:138](std/time/init.st#L138)

---

### `addWeeks(t: Time, weeks: int) Time`

**Source:** [std/time/init.st:143](std/time/init.st#L143)

---

### `addMonths(t: Time, months: int) Time`

**Source:** [std/time/init.st:148](std/time/init.st#L148)

---

### `addYears(t: Time, years: int) Time`

**Source:** [std/time/init.st:153](std/time/init.st#L153)

---

### `startOfDay(t: Time) Time`

**Source:** [std/time/init.st:158](std/time/init.st#L158)

---

### `endOfDay(t: Time) Time`

**Source:** [std/time/init.st:163](std/time/init.st#L163)

---

### `startOfMonth(t: Time) Time`

**Source:** [std/time/init.st:168](std/time/init.st#L168)

---

### `endOfMonth(t: Time) Time`

**Source:** [std/time/init.st:173](std/time/init.st#L173)

---

### `ageInYears(birthDate: Time) int`

**Source:** [std/time/init.st:179](std/time/init.st#L179)

---

### `daysBetween(start: Time, end: Time) int`

**Source:** [std/time/init.st:193](std/time/init.st#L193)

---

### `humanizeDuration(d: Duration) string`

**Source:** [std/time/init.st:199](std/time/init.st#L199)

---

## Classes

### Class: `Time`

**Properties:**

- `timestamp: int`

**Methods:**

#### `year() int`

#### `month() int`

#### `day() int`

#### `hour() int`

#### `minute() int`

#### `second() int`

#### `millisecond() int`

#### `format(layout: string) string`

#### `add(duration: Duration) Time`

#### `sub(other: Time) Duration`

#### `before(other: Time) bool`

#### `after(other: Time) bool`

#### `equals(other: Time) bool`

#### `unix() int`

**Source:** [std/time/init.st:10](std/time/init.st#L10)

---

### Class: `Duration`

**Properties:**

- `milliseconds: int`

**Methods:**

#### `hours() double`

#### `minutes() double`

#### `seconds() double`

#### `milliseconds() int`

#### `add(other: Duration) Duration`

#### `sub(other: Duration) Duration`

**Source:** [std/time/init.st:30](std/time/init.st#L30)

---



