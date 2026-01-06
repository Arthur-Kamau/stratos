---
title: http
---

# Package: http

## Functions

### `header(name: string) string`

*Native function*

**Source:** [std/net/http/init.st:30](std/net/http/init.st#L30)

---

### `queryParam(name: string) string`

*Native function*

**Source:** [std/net/http/init.st:31](std/net/http/init.st#L31)

---

### `param(name: string) string`

*Native function*

**Source:** [std/net/http/init.st:32](std/net/http/init.st#L32)

---

### `group(prefix: string) Router`

*Native function*

**Source:** [std/net/http/init.st:62](std/net/http/init.st#L62)

---

### `newServer(router: Router) Server`

*Native function*

**Source:** [std/net/http/init.st:102](std/net/http/init.st#L102)

---

### `newRouter() Router`

*Native function*

**Source:** [std/net/http/init.st:103](std/net/http/init.st#L103)

---

### `newClient(timeout: int) Client`

*Native function*

**Source:** [std/net/http/init.st:106](std/net/http/init.st#L106)

---

### `buildResponse(response: Response) string`

*Native function*

**Source:** [std/net/http/init.st:112](std/net/http/init.st#L112)

---

### `loggerMiddleware() Middleware`

**Source:** [std/net/http/init.st:159](std/net/http/init.st#L159)

---

### `corsMiddleware(allowOrigin: string) Middleware`

**Source:** [std/net/http/init.st:183](std/net/http/init.st#L183)

---

### `rateLimitMiddleware(requestsPerMinute: int) Middleware`

**Source:** [std/net/http/init.st:223](std/net/http/init.st#L223)

---

### `jsonBodyMiddleware() Middleware`

**Source:** [std/net/http/init.st:239](std/net/http/init.st#L239)

---

### `staticMiddleware(directory: string) Middleware`

**Source:** [std/net/http/init.st:259](std/net/http/init.st#L259)

---

### `newRestAPI(basePath: string) RestAPI`

**Source:** [std/net/http/init.st:338](std/net/http/init.st#L338)

---

### `handler() Handler`

**Source:** [std/net/http/init.st:362](std/net/http/init.st#L362)

---

### `newSSE() SSE`

**Source:** [std/net/http/init.st:386](std/net/http/init.st#L386)

---

## Classes

### Class: `URL`

**Properties:**

- `scheme: string`
- `host: string`
- `port: int`
- `path: string`
- `query: string`
- `fragment: string`

**Source:** [std/net/http/init.st:117](std/net/http/init.st#L117)

---

### Class: `ResourceHandlers`

**Properties:**

- `list: Handler`
- `create: Handler`
- `show: Handler`
- `update: Handler`
- `delete: Handler`

**Source:** [std/net/http/init.st:330](std/net/http/init.st#L330)

---



