// IO module - File and stream I/O operations
// Inspired by Go's io and os packages

package io;

// ============================================================================
// Types
// ============================================================================

class File {
    var path: string;
    var handle: int;  // File descriptor
    var mode: string;

    fn readAll() string;
    fn readLine() string;
    fn write(data: string) Result<int, Error>;
    fn close() void;
}

class Result<T, E> {
    var value: T;
    var error: E;
    var isOk: bool;

    fn ok() bool {
        return this.isOk;
    }

    fn err() E {
        return this.error;
    }

    fn unwrap() T {
        if !this.isOk {
            panic("Called unwrap on Error result");
        }
        return this.value;
    }
}

class Error {
    var message: string;

    constructor(msg: string) {
        this.message = msg;
    }
}

class FileInfo {
    var name: string;
    var size: int;
    var isDirectory: bool;
    var modTime: int;  // Unix timestamp
}

// ============================================================================
// Native function declarations
// ============================================================================

// File reading
fn readFile(path: string) Result<string, Error>;
fn readBytes(path: string) Result<Array<byte>, Error>;

// File writing
fn writeFile(path: string, content: string) Result<void, Error>;
fn appendFile(path: string, content: string) Result<void, Error>;
fn writeBytes(path: string, data: Array<byte>) Result<void, Error>;

// File operations
fn open(path: string, mode: string) Result<File, Error>;
fn create(path: string) Result<File, Error>;
fn remove(path: string) Result<void, Error>;
fn rename(oldPath: string, newPath: string) Result<void, Error>;
fn copy(src: string, dst: string) Result<void, Error>;

// Directory operations
fn mkdir(path: string) Result<void, Error>;
fn mkdirAll(path: string) Result<void, Error>;
fn removeDir(path: string) Result<void, Error>;
fn removeDirAll(path: string) Result<void, Error>;
fn readDir(path: string) Result<Array<FileInfo>, Error>;

// File info
fn exists(path: string) bool;
fn isFile(path: string) bool;
fn isDirectory(path: string) bool;
fn fileSize(path: string) Result<int, Error>;
fn stat(path: string) Result<FileInfo, Error>;

// Path operations
fn join(parts: Array<string>) string;
fn basename(path: string) string;
fn dirname(path: string) string;
fn extension(path: string) string;
fn absolute(path: string) string;

// Temporary files
fn tempFile(pattern: string) Result<File, Error>;
fn tempDir(pattern: string) Result<string, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Read lines from file
fn readLines(path: string) Result<Array<string>, Error> {
    val content = readFile(path);

    if !content.ok() {
        return Result<Array<string>, Error> {
            value: [],
            error: content.err(),
            isOk: false
        };
    }

    val lines = content.unwrap().split("\n");

    return Result<Array<string>, Error> {
        value: lines,
        error: None,
        isOk: true
    };
}

// Write lines to file
fn writeLines(path: string, lines: Array<string>) Result<void, Error> {
    val content = lines.join("\n");
    return writeFile(path, content);
}

// Append line to file
fn appendLine(path: string, line: string) Result<void, Error> {
    return appendFile(path, line + "\n");
}

// Check if path has extension
fn hasExtension(path: string, ext: string) bool {
    return extension(path) == ext;
}

// List files in directory with filter
fn listFiles(path: string, filter: Function<FileInfo, bool>) Result<Array<FileInfo>, Error> {
    val filesResult = readDir(path);

    if !filesResult.ok() {
        return Result<Array<FileInfo>, Error> {
            value: [],
            error: filesResult.err(),
            isOk: false
        };
    }

    val files = filesResult.unwrap();
    var filtered = Array<FileInfo>();

    for val file in files {
        if filter(file) {
            filtered.add(file);
        }
    }

    return Result<Array<FileInfo>, Error> {
        value: filtered,
        error: None,
        isOk: true
    };
}

// List only files (not directories)
fn listFilesOnly(path: string) Result<Array<FileInfo>, Error> {
    return listFiles(path, (file) => !file.isDirectory);
}

// List only directories
fn listDirectoriesOnly(path: string) Result<Array<FileInfo>, Error> {
    return listFiles(path, (file) => file.isDirectory);
}

// Ensure directory exists (create if not)
fn ensureDir(path: string) Result<void, Error> {
    if exists(path) {
        if isDirectory(path) {
            return Result<void, Error> { value: None, error: None, isOk: true };
        } else {
            return Result<void, Error> {
                value: None,
                error: Error("Path exists but is not a directory"),
                isOk: false
            };
        }
    }

    return mkdirAll(path);
}

// Walk directory tree
fn walk(root: string, visitor: Function<string, FileInfo, void>) Result<void, Error> {
    val filesResult = readDir(root);

    if !filesResult.ok() {
        return Result<void, Error> {
            value: None,
            error: filesResult.err(),
            isOk: false
        };
    }

    val files = filesResult.unwrap();

    for val file in files {
        val fullPath = join([root, file.name]);
        visitor(fullPath, file);

        if file.isDirectory {
            walk(fullPath, visitor);
        }
    }

    return Result<void, Error> { value: None, error: None, isOk: true };
}
