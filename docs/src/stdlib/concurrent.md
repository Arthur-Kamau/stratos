---
title: concurrent
---

# Package: concurrent

## Functions

### `send(value: T) bool`

*Native function*

**Source:** [std/concurrent/init.st:15](std/concurrent/init.st#L15)

---

### `receive() T`

*Native function*

**Source:** [std/concurrent/init.st:16](std/concurrent/init.st#L16)

---

### `isClosed() bool`

*Native function*

**Source:** [std/concurrent/init.st:19](std/concurrent/init.st#L19)

---

### `tryLock() bool`

*Native function*

**Source:** [std/concurrent/init.st:28](std/concurrent/init.st#L28)

---

### `tryAcquire() bool`

*Native function*

**Source:** [std/concurrent/init.st:64](std/concurrent/init.st#L64)

---

### `get() T`

*Native function*

**Source:** [std/concurrent/init.st:73](std/concurrent/init.st#L73)

---

### `isDone() bool`

*Native function*

**Source:** [std/concurrent/init.st:75](std/concurrent/init.st#L75)

---

### `newMutex() Mutex`

*Native function*

**Source:** [std/concurrent/init.st:102](std/concurrent/init.st#L102)

---

### `newRWMutex() RWMutex`

*Native function*

**Source:** [std/concurrent/init.st:103](std/concurrent/init.st#L103)

---

### `newWaitGroup() WaitGroup`

*Native function*

**Source:** [std/concurrent/init.st:106](std/concurrent/init.st#L106)

---

### `newOnce() Once`

*Native function*

**Source:** [std/concurrent/init.st:107](std/concurrent/init.st#L107)

---

### `newSemaphore(capacity: int) Semaphore`

*Native function*

**Source:** [std/concurrent/init.st:108](std/concurrent/init.st#L108)

---

### `getThreadId() int`

*Native function*

**Source:** [std/concurrent/init.st:111](std/concurrent/init.st#L111)

---

### `getThreadCount() int`

*Native function*

**Source:** [std/concurrent/init.st:112](std/concurrent/init.st#L112)

---

### `tryAcquire() bool`

**Source:** [std/concurrent/init.st:323](std/concurrent/init.st#L323)

---

### `newRateLimiter(requestsPerSecond: int) RateLimiter`

**Source:** [std/concurrent/init.st:328](std/concurrent/init.st#L328)

---

## Classes

### Class: `RateLimiter`

**Properties:**

- `tokens: Semaphore`
- `rate: Duration`

**Methods:**

#### `constructor(requestsPerSecond: int) void`

**Source:** [std/concurrent/init.st:302](std/concurrent/init.st#L302)

---



