// IO module - File and stream I/O operations
// Inspired by Go's io and os packages

package io;

use strings;

// ============================================================================
// Types
// ============================================================================

class File {
    var path: string;
    var handle: int;  // File descriptor
    var mode: string;

    pub fn readAll() string;
    pub fn readLine() string;
    pub fn write(data: string) Result<int, Error>;
    pub fn close() void;
}

// Result class for error handling
class Result<T, E> {
    var value: T;
    var error: E;
    var isOk: bool;

    pub fn ok() bool {
        return this.isOk;
    }

    pub fn err() E {
        return this.error;
    }

    pub fn unwrap() T {
        if !this.isOk {
            panic("Called unwrap on Error result");
        }
        return this.value;
    }
}

// Error class for error details
class Error {
    pub var message: string;

    constructor(msg: string) {
        this.message = msg;
    }
}

class FileInfo {
    pub var fileName: string;
    pub var size: int;
    pub var isDir: bool;
    pub var modTime: int;  // Unix timestamp
    
    pub fn getName() string {
        return this.fileName;
    }

    pub fn getSize() int {
        return this.size;
    }

    pub fn getIsDir() bool {
        return this.isDir;
    }

    pub fn getModTime() int {
        return this.modTime;
    }
}

// ============================================================================
// Native function declarations (C++ implementations)
// ============================================================================

// File reading
fn readFile(path: string) Result<string, Error>;
fn readBytes(path: string) Result<Array<byte>, Error>;

// File writing
fn writeFile(path: string, content: string) Result<bool, Error>;
fn appendFile(path: string, content: string) Result<bool, Error>;
fn writeBytes(path: string, data: Array<byte>) Result<bool, Error>;

// File operations
fn open(path: string, mode: string) Result<File, Error>;
fn create(path: string) Result<File, Error>;
fn remove(path: string) Result<bool, Error>;
fn rename(oldPath: string, newPath: string) Result<bool, Error>;
fn copy(src: string, dst: string) Result<bool, Error>;

// Directory operations
fn mkdir(path: string) Result<bool, Error>;
fn mkdirAll(path: string) Result<bool, Error>;
fn removeDir(path: string) Result<bool, Error>;
fn removeDirAll(path: string) Result<bool, Error>;
fn readDir(path: string) Result<Array<FileInfo>, Error>;

// File info - simple bools, no Result needed
fn exists(path: string) bool;
fn isFile(path: string) bool;
fn isDirectory(path: string) bool;
fn fileSize(path: string) Result<int, Error>;
fn stat(path: string) Result<FileInfo, Error>;

// Path operations
fn pathJoin(parts: Array<string>) string;  // Renamed from join to avoid conflict with strings.join
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
fn readLines(path: string) Array<string> {
    val result = readFile(path);
    if result.ok() {
        val content = result.unwrap();
        // Use strings module to split by newline
        return strings.split(content, "\n");
    }
    return [];
}

// Write lines to file
fn writeLines(path: string, lines: Array<string>) Result<bool, Error> {
    val content = lines.join("\n");
    return writeFile(path, content);
}

// Append line to file
fn appendLine(path: string, line: string) Result<bool, Error> {
    return appendFile(path, line + "\n");
}

// Check if path has extension
fn hasExtension(path: string, ext: string) bool {
    return extension(path) == ext;
}

// List files in directory with filter - simplified
fn listFiles(path: string, filter: Function<FileInfo, bool>) Array<FileInfo> {
    val result = readDir(path);
    if !result.ok() {
        return [];
    }

    val files = result.unwrap();
    var filtered = Array<FileInfo>();

    for val file in files {
        if filter(file) {
            filtered.add(file);
        }
    }

    return filtered;
}

// List only files (not directories)
// List only files (not directories)
// List only files (not directories)
/*
// List only files (not directories)
fn listFilesOnly(path: string) Array<FileInfo> {
    return listFiles(path, (file) => !(file as FileInfo).isDir);
}

// List only directories
fn listDirectoriesOnly(path: string) Array<FileInfo> {
    return listFiles(path, (file) => (file as FileInfo).isDir);
}
*/

// Ensure directory exists (create if not)
fn ensureDir(path: string) Result<bool, Error> {
    if exists(path) {
        if isDirectory(path) {
            return mkdirAll(path);  // Returns Ok if already exists
        }
    }
    return mkdirAll(path);
}

// Walk directory tree - simplified
fn walk(root: string, visitor: any) bool {
    val result = readDir(root);
    if !result.ok() {
        return false;
    }

    val files = result.unwrap();
    for val file in files {
        val fullPath = pathJoin([root, file.getName()]);
        visitor(fullPath, file);

        if file.getIsDir() {
            walk(fullPath, visitor);
        }
    }

    return true;
}
