// OS module - Operating system interface
// Inspired by Go's os package and Node.js os module

package os;

// ============================================================================
// Types
// ============================================================================

enum Platform {
    LINUX,
    DARWIN,
    WINDOWS,
    BSD,
    UNKNOWN
}

enum Arch {
    AMD64,
    ARM64,
    X86,
    ARM,
    UNKNOWN
}

class Process {
    var pid: int;
    var ppid: int;
    var command: string;
    var args: Array<string>;

    fn kill(signal: int) Result<void, Error>;
    fn wait() int;
}

class EnvVars {
    var vars: Map<string, string>;

    fn get(key: string) string;
    fn set(key: string, value: string) void;
    fn unset(key: string) void;
    fn has(key: string) bool;
    fn all() Map<string, string>;
}

// ============================================================================
// Native function declarations
// ============================================================================

// Environment
fn getenv(key: string) string;
fn setenv(key: string, value: string) void;
fn unsetenv(key: string) void;
fn environ() Map<string, string>;

// Process info
fn getpid() int;
fn getppid() int;
fn getuid() int;
fn getgid() int;

// System info
fn platform() Platform;
fn arch() Arch;
fn hostname() string;
fn homedir() string;
fn tmpdir() string;
fn cpuCount() int;
fn totalMemory() int;
fn freeMemory() int;

// Process execution
fn exec(command: string, args: Array<string>) Result<int, Error>;
fn execWithOutput(command: string, args: Array<string>) Result<string, Error>;
fn spawn(command: string, args: Array<string>) Result<Process, Error>;
fn system(command: string) int;

// Process control
fn exit(code: int) void;
fn abort() void;

// Working directory
fn getcwd() string;
fn chdir(path: string) Result<void, Error>;

// User info
fn getUsername() string;
fn getUserHome() string;

// Signals
fn signal(sig: int, handler: Function) void;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Get environment variable with default
fn getEnvOr(key: string, defaultValue: string) string {
    val value = getenv(key);
    if value == "" {
        return defaultValue;
    }
    return value;
}

// Check if environment variable exists
fn hasEnv(key: string) bool {
    return getenv(key) != "";
}

// Execute command and get output
fn run(command: string) Result<string, Error> {
    return execWithOutput(command, Array<string>());
}

// Execute command with arguments
fn runWithArgs(command: string, args: Array<string>) Result<string, Error> {
    return execWithOutput(command, args);
}

// Execute command and check success
fn runSuccess(command: string) bool {
    val result = exec(command, Array<string>());
    if result.ok() {
        return result.unwrap() == 0;
    }
    return false;
}

// Get system info
class SystemInfo {
    var platform: Platform;
    var arch: Arch;
    var hostname: string;
    var cpuCount: int;
    var totalMemory: int;
    var freeMemory: int;
}

fn getSystemInfo() SystemInfo {
    return SystemInfo {
        platform: platform(),
        arch: arch(),
        hostname: hostname(),
        cpuCount: cpuCount(),
        totalMemory: totalMemory(),
        freeMemory: freeMemory()
    };
}

// Check if running on specific platform
fn isLinux() bool {
    return platform() == Platform.LINUX;
}

fn isDarwin() bool {
    return platform() == Platform.DARWIN;
}

fn isWindows() bool {
    return platform() == Platform.WINDOWS;
}

// Path helpers
fn joinPath(parts: Array<string>) string {
    val separator = isWindows() ? "\\" : "/";
    return strings.join(parts, separator);
}

fn expandPath(path: string) string {
    if strings.startsWith(path, "~") {
        return strings.replace(path, "~", homedir());
    }
    return path;
}

// Which - find executable in PATH
fn which(command: string) string? {
    val pathEnv = getenv("PATH");
    val separator = isWindows() ? ";" : ":";
    val paths = strings.split(pathEnv, separator);

    for val dir in paths {
        val fullPath = joinPath([dir, command]);
        if io.exists(fullPath) && io.isFile(fullPath) {
            return fullPath;
        }
    }

    return null;
}

// Check if command exists
fn commandExists(command: string) bool {
    return which(command) != null;
}

// Execute shell command
fn shell(command: string) Result<string, Error> {
    val shellCmd = isWindows() ? "cmd" : "sh";
    val shellArg = isWindows() ? "/c" : "-c";

    return execWithOutput(shellCmd, [shellArg, command]);
}

// Pipe commands
fn pipe(commands: Array<string>) Result<string, Error> {
    var output = "";

    for val command in commands {
        val result = shell(command);
        if !result.ok() {
            return result;
        }
        output = result.unwrap();
    }

    return io.Result<string, Error> {
        value: output,
        error: null,
        isOk: true
    };
}

// Environment manager
class Environment {
    var vars: Map<string, string>;
    var originalVars: Map<string, string>;

    constructor() {
        this.vars = environ();
        this.originalVars = environ();
    }

    fn get(key: string) string {
        if this.vars.containsKey(key) {
            return this.vars.get(key);
        }
        return "";
    }

    fn set(key: string, value: string) void {
        this.vars.put(key, value);
        setenv(key, value);
    }

    fn unset(key: string) void {
        this.vars.remove(key);
        unsetenv(key);
    }

    fn restore() void {
        // Restore all original environment variables
        this.originalVars.keys().forEach((key) => {
            setenv(key, this.originalVars.get(key));
        });
    }

    fn snapshot() Map<string, string> {
        return this.vars;
    }
}

fn newEnvironment() Environment {
    return Environment();
}

// Temporary environment for execution
fn withEnv(vars: Map<string, string>, fn: Function) void {
    val env = newEnvironment();

    // Set temporary variables
    vars.keys().forEach((key) => {
        env.set(key, vars.get(key));
    });

    // Execute function
    fn();

    // Restore original environment
    env.restore();
}

// Current process info
fn getCurrentProcess() Process {
    return Process {
        pid: getpid(),
        ppid: getppid(),
        command: getenv("_"),
        args: Array<string>()
    };
}

// Uptime (in seconds) - requires proc filesystem on Linux
fn uptime() int {
    if isLinux() {
        val result = io.readFile("/proc/uptime");
        if result.ok() {
            val content = result.unwrap();
            val parts = strings.split(content, " ");
            if parts.length() > 0 {
                // Parse first number as uptime in seconds
                return 0; // Would need string to int conversion
            }
        }
    }
    return 0;
}

// Load average (Linux/Unix)
fn loadAverage() Array<double> {
    if isLinux() {
        val result = io.readFile("/proc/loadavg");
        if result.ok() {
            val content = result.unwrap();
            val parts = strings.split(content, " ");
            // Would return [1min, 5min, 15min] load averages
            return [0.0, 0.0, 0.0];
        }
    }
    return [0.0, 0.0, 0.0];
}

// Platform-specific path separator
fn pathSeparator() string {
    return isWindows() ? "\\" : "/";
}

// Platform-specific line ending
fn lineEnding() string {
    return isWindows() ? "\r\n" : "\n";
}

// Signals
val SIGINT = 2;
val SIGTERM = 15;
val SIGKILL = 9;
val SIGHUP = 1;
val SIGUSR1 = 10;
val SIGUSR2 = 12;
