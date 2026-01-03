# Package: os

## Functions

### `wait() int`

*Native function*

**Source:** [std/os/init.st:33](std/os/init.st#L33)

---

### `get(key: string) string`

*Native function*

**Source:** [std/os/init.st:39](std/os/init.st#L39)

---

### `has(key: string) bool`

*Native function*

**Source:** [std/os/init.st:42](std/os/init.st#L42)

---

### `getenv(key: string) string`

*Native function*

**Source:** [std/os/init.st:51](std/os/init.st#L51)

---

### `getpid() int`

*Native function*

**Source:** [std/os/init.st:57](std/os/init.st#L57)

---

### `getppid() int`

*Native function*

**Source:** [std/os/init.st:58](std/os/init.st#L58)

---

### `getuid() int`

*Native function*

**Source:** [std/os/init.st:59](std/os/init.st#L59)

---

### `getgid() int`

*Native function*

**Source:** [std/os/init.st:60](std/os/init.st#L60)

---

### `platform() Platform`

*Native function*

**Source:** [std/os/init.st:63](std/os/init.st#L63)

---

### `arch() Arch`

*Native function*

**Source:** [std/os/init.st:64](std/os/init.st#L64)

---

### `hostname() string`

*Native function*

**Source:** [std/os/init.st:65](std/os/init.st#L65)

---

### `homedir() string`

*Native function*

**Source:** [std/os/init.st:66](std/os/init.st#L66)

---

### `tmpdir() string`

*Native function*

**Source:** [std/os/init.st:67](std/os/init.st#L67)

---

### `cpuCount() int`

*Native function*

**Source:** [std/os/init.st:68](std/os/init.st#L68)

---

### `totalMemory() int`

*Native function*

**Source:** [std/os/init.st:69](std/os/init.st#L69)

---

### `freeMemory() int`

*Native function*

**Source:** [std/os/init.st:70](std/os/init.st#L70)

---

### `system(command: string) int`

*Native function*

**Source:** [std/os/init.st:76](std/os/init.st#L76)

---

### `getcwd() string`

*Native function*

**Source:** [std/os/init.st:83](std/os/init.st#L83)

---

### `getUsername() string`

*Native function*

**Source:** [std/os/init.st:87](std/os/init.st#L87)

---

### `getUserHome() string`

*Native function*

**Source:** [std/os/init.st:88](std/os/init.st#L88)

---

### `getEnvOr(key: string, defaultValue: string) string`

**Source:** [std/os/init.st:98](std/os/init.st#L98)

---

### `hasEnv(key: string) bool`

**Source:** [std/os/init.st:107](std/os/init.st#L107)

---

### `runSuccess(command: string) bool`

**Source:** [std/os/init.st:122](std/os/init.st#L122)

---

### `getSystemInfo() SystemInfo`

**Source:** [std/os/init.st:140](std/os/init.st#L140)

---

### `isLinux() bool`

**Source:** [std/os/init.st:152](std/os/init.st#L152)

---

### `isDarwin() bool`

**Source:** [std/os/init.st:156](std/os/init.st#L156)

---

### `isWindows() bool`

**Source:** [std/os/init.st:160](std/os/init.st#L160)

---

### `expandPath(path: string) string`

**Source:** [std/os/init.st:170](std/os/init.st#L170)

---

### `which(command: string) string`

**Source:** [std/os/init.st:178](std/os/init.st#L178)

---

### `commandExists(command: string) bool`

**Source:** [std/os/init.st:194](std/os/init.st#L194)

---

### `get(key: string) string`

**Source:** [std/os/init.st:235](std/os/init.st#L235)

---

### `newEnvironment() Environment`

**Source:** [std/os/init.st:264](std/os/init.st#L264)

---

### `getCurrentProcess() Process`

**Source:** [std/os/init.st:285](std/os/init.st#L285)

---

### `uptime() int`

**Source:** [std/os/init.st:295](std/os/init.st#L295)

---

### `pathSeparator() string`

**Source:** [std/os/init.st:325](std/os/init.st#L325)

---

### `lineEnding() string`

**Source:** [std/os/init.st:330](std/os/init.st#L330)

---

## Classes

### Class: `SystemInfo`

**Properties:**

- `platform: Platform`
- `arch: Arch`
- `hostname: string`
- `cpuCount: int`
- `totalMemory: int`
- `freeMemory: int`

**Source:** [std/os/init.st:131](std/os/init.st#L131)

---

## Variables

- `output: `

- `result: `

- `env: `

- `result: `

- `content: `

- `parts: `

- `SIGINT: `

- `SIGTERM: `

- `SIGKILL: `

- `SIGHUP: `

- `SIGUSR1: `

- `SIGUSR2: `

