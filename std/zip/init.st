// Zip module - Compression and archiving
// Inspired by Go's compress package

package zip;

// ============================================================================
// Native function declarations
// ============================================================================

// Compression
fn compress(data: string) Array<byte>;
fn decompress(compressed: Array<byte>) string;

// File compression
fn compressFile(inputPath: string, outputPath: string) Result<void, Error>;
fn decompressFile(inputPath: string, outputPath: string) Result<void, Error>;

// Archive operations
fn createArchive(archivePath: string, files: Array<string>) Result<void, Error>;
fn extractArchive(archivePath: string, destPath: string) Result<void, Error>;
fn listArchive(archivePath: string) Result<Array<string>, Error>;

// Add/remove files from archive
fn addToArchive(archivePath: string, filePath: string) Result<void, Error>;
fn removeFromArchive(archivePath: string, fileName: string) Result<void, Error>;

// ============================================================================
// Pure Stratos implementations
// ============================================================================

// Compress string to file
fn compressString(data: string, outputPath: string) Result<void, Error> {
    val compressed = compress(data);
    return io.writeBytes(outputPath, compressed);
}

// Decompress file to string
fn decompressToString(inputPath: string) Result<string, Error> {
    val bytesResult = io.readBytes(inputPath);

    if !bytesResult.ok() {
        return io.Result<string, Error> {
            value: "",
            error: bytesResult.err(),
            isOk: false
        };
    }

    val decompressed = decompress(bytesResult.unwrap());

    return io.Result<string, Error> {
        value: decompressed,
        error: null,
        isOk: true
    };
}

// Compress directory to archive
fn compressDirectory(dirPath: string, archivePath: string) Result<void, Error> {
    val filesResult = io.readDir(dirPath);

    if !filesResult.ok() {
        return io.Result<void, Error> {
            value: null,
            error: filesResult.err(),
            isOk: false
        };
    }

    var filePaths = Array<string>();
    for val file in filesResult.unwrap() {
        if !file.isDirectory {
            filePaths.add(io.join([dirPath, file.name]));
        }
    }

    return createArchive(archivePath, filePaths);
}

// Extract archive to directory
fn extractToDirectory(archivePath: string, destPath: string) Result<void, Error> {
    // Ensure destination exists
    val ensureResult = io.ensureDir(destPath);

    if !ensureResult.ok() {
        return ensureResult;
    }

    return extractArchive(archivePath, destPath);
}

// Check if file is compressed
fn isCompressed(filePath: string) bool {
    val ext = io.extension(filePath);
    return ext == ".zip" || ext == ".gz" || ext == ".tar" || ext == ".7z";
}

// Get compression ratio
fn getCompressionRatio(originalSize: int, compressedSize: int) double {
    if originalSize == 0 {
        return 0.0;
    }
    return (1.0 - (compressedSize / originalSize)) * 100.0;
}
