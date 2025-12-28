# Stratos Dependency Management: Lock Files & Cache System

This document describes the lock file mechanism and global cache system implemented in Stratos for dependency management.

## Overview

Stratos provides a robust dependency management system with two key features:

1. **Lock Files** (`stratos.lock`) - Ensure reproducible builds across environments
2. **Global Cache** (`~/.stratos/cache`) - Share dependencies across projects and reduce disk usage

## Lock File Mechanism

### What is a Lock File?

The lock file (`stratos.lock`) is an auto-generated file that records the exact versions of all dependencies used in your project, including transitive dependencies. This ensures that every developer and CI/CD system uses the identical dependency versions.

### Lock File Format

Lock files use HOCON (Human-Optimized Config Object Notation) format:

```hocon
# Stratos dependency lock file
# This file is auto-generated. Do not edit manually.
# Generated: 2025-12-28T22:30:00Z

metadata {
  generated = "2025-12-28T22:30:00Z"
  stratos_version = "0.1.0"
}

dependencies = [
  {
    name = my-lib
    url = "https://github.com/user/my-lib"
    type = git
    tag = "v1.0.0"
    resolved_commit = "abc123def456789"
    checksum = "sha256:cbee97a2d756db38"
  }
]
```

### Lock File Fields

| Field | Description |
|-------|-------------|
| `name` | Package name |
| `url` | Repository URL or local path |
| `type` | Dependency type: `git` or `local` |
| `tag` | Git tag (if specified) |
| `branch` | Git branch (if specified) |
| `hash` | Commit hash (if specified) |
| `resolved_commit` | Actual commit hash that was fetched |
| `checksum` | SHA-256 checksum of dependency contents |
| `via` | Parent dependency (for transitive deps) |

### Lock File Operations

#### Generate Lock File

Automatically generated when running `stratos get`:

```bash
cd my-project
stratos get
```

This creates/updates `stratos.lock` with all resolved dependencies.

#### Verify Lock File

Check if installed dependencies match the lock file:

```bash
stratos get --verify
```

Output:
```
Verifying 3 locked dependencies...
✓ my-lib
✓ utils
✓ logger

✓ All dependencies verified successfully
```

#### Update Lock File

Update all dependencies to latest versions:

```bash
stratos get --update
```

This will:
1. Fetch latest versions of all dependencies
2. Regenerate the lock file
3. Update transitive dependencies

### When to Commit Lock Files

**✅ DO commit** `stratos.lock` to version control:
- Ensures team uses same dependency versions
- Makes builds reproducible
- Prevents "works on my machine" issues
- Critical for CI/CD reliability

**❌ DON'T commit** `deps/` directory:
- Dependencies are large and bloat repository
- Can be regenerated from lock file
- Add to `.gitignore`:
  ```gitignore
  deps/
  ```

### Lock File Workflow

```
Developer A:
1. Add dependency to stratos.conf
2. Run: stratos get
3. stratos.lock is created/updated
4. Commit stratos.lock to git
5. Push to repository

Developer B:
1. Pull from repository (gets stratos.lock)
2. Run: stratos get
3. Gets EXACT same dependency versions as Developer A
4. Build works identically
```

## Global Cache System

### What is the Global Cache?

The global cache stores downloaded dependencies in a central location (`~/.stratos/cache`) instead of duplicating them in every project. This provides:

- **Reduced Disk Usage**: Dependencies downloaded once, shared across projects
- **Faster Fetching**: Already-cached dependencies copy instantly
- **Offline Support**: Cached dependencies work without network access

### Cache Structure

```
~/.stratos/cache/
├── git/                           # Git dependencies
│   ├── github.com_user_repo_v1.0.0/
│   │   ├── stratos.conf
│   │   ├── src/
│   │   └── ...
│   ├── github.com_user_lib_main/
│   └── gitlab.com_org_pkg_v2.1.0/
└── metadata/                      # Cache metadata
    ├── github.com_user_repo_v1.0.0.json
    └── github.com_user_lib_main.json
```

### Cache Keys

Dependencies are cached using a key derived from the URL and version:

| Dependency | Cache Key |
|------------|-----------|
| `github.com/user/repo@v1.0.0` | `github.com_user_repo_v1.0.0` |
| `github.com/user/repo@main` | `github.com_user_repo_main` |
| `https://gitlab.com/org/pkg@v2.1.0` | `gitlab.com_org_pkg_v2.1.0` |

Special characters (`/:@?&=+$,#<>%"'|*`) are replaced with underscores.

### Cache Behavior

#### First Fetch (Not Cached)

```bash
cd project1
stratos get
```

Output:
```
Fetching dependency: my-lib
  Type: git
  URL: https://github.com/user/my-lib
  Tag: v1.0.0
Dependency not in cache, cloning from git...
Cloning: https://github.com/user/my-lib (version: v1.0.0)
✓ Cached dependency: github.com_user_my-lib_v1.0.0
✓ Successfully fetched: my-lib
```

The dependency is:
1. Cloned from GitHub
2. Copied to project `deps/`
3. Added to global cache

#### Second Fetch (Cached)

```bash
cd project2
stratos get
```

Output:
```
Fetching dependency: my-lib
  Type: git
  URL: https://github.com/user/my-lib
  Tag: v1.0.0
Using cached dependency: /home/user/.stratos/cache/git/github.com_user_my-lib_v1.0.0
✓ Copied from cache: https://github.com/user/my-lib (v1.0.0)
✓ Successfully fetched: my-lib
```

The dependency is copied from cache (instant, no network access needed).

### Cache Environment Variable

Customize cache location with `STRATOS_CACHE`:

```bash
# Use custom cache directory
export STRATOS_CACHE=/mnt/fast-ssd/stratos-cache
stratos get

# Or per-command
STRATOS_CACHE=/tmp/cache stratos get
```

Default locations:
- Linux/macOS: `~/.stratos/cache`
- Windows: `%USERPROFILE%/.stratos/cache`
- Fallback: `/tmp/stratos-cache`

### Cache Management

#### View Cache Statistics

```bash
stratos cache stats
```

Output:
```
Cache Statistics:
  Location: /home/user/.stratos/cache
  Total Entries: 15
  Total Size: 127.3 MB

Entries:
  - github.com_user_my-lib_v1.0.0 (12.5 MB)
  - github.com_org_utils_v2.3.1 (8.2 MB)
  ...
```

#### Clean Old Entries

Remove cache entries not accessed in 30 days:

```bash
stratos cache clean
```

Or specify custom days:

```bash
stratos cache clean --days 60
```

#### Clear Entire Cache

Remove all cached dependencies:

```bash
stratos cache clear
```

Output:
```
Removed 15 cache entries
✓ Cache cleared successfully
```

### What Gets Cached?

| Dependency Type | Cached? | Reason |
|----------------|---------|--------|
| Git (GitHub/GitLab) | ✅ Yes | Remote dependencies benefit from caching |
| Local (file paths) | ❌ No | Already on disk, no download needed |
| Transitive deps (git) | ✅ Yes | Treated same as direct git deps |

### Cache and Lock Files Working Together

Lock files and cache complement each other:

```
Developer workflow:
1. stratos.conf specifies dependency: github.com/user/lib@v1.0.0
2. Run: stratos get
3. Check lock file → already locked to commit abc123
4. Check cache → found github.com_user_lib_v1.0.0
5. Copy from cache to deps/ (instant)
6. Verify checksum matches lock file
7. Done! (no network, no build time)
```

Benefits:
- Lock file ensures version consistency
- Cache eliminates redundant downloads
- Checksum verification ensures integrity

## Implementation Details

### Files and Classes

#### Lock File Management

**File**: `interpreter/C++/src/config/LockFile.cpp`

**Class**: `LockFileManager`

**Key Methods**:
```cpp
// Load lock file
std::optional<std::vector<LockedDependency>> load(const std::string& lockFilePath);

// Save lock file
bool save(const std::string& lockFilePath,
          const std::vector<LockedDependency>& deps,
          const std::string& stratosVersion);

// Verify dependencies match lock file
bool verify(const std::string& projectRoot);

// Generate lock file from installed dependencies
bool generate(const std::string& projectRoot);
```

#### Cache Management

**File**: `interpreter/C++/src/config/CacheManager.cpp`

**Class**: `CacheManager`

**Key Methods**:
```cpp
// Check if dependency is cached
bool isCached(const std::string& url, const std::string& version);

// Get cached dependency path
std::optional<std::string> getCachedDependency(const std::string& url,
                                                const std::string& version);

// Add dependency to cache
std::string cacheDependency(const std::string& url,
                            const std::string& version,
                            const std::string& sourcePath);

// Cache management
bool clearCache();
int cleanOldEntries(int days = 30);
CacheStats getStats();
```

#### Dependency Manager Integration

**File**: `interpreter/C++/src/config/DependencyManager.cpp`

The `DependencyManager` integrates both lock files and cache:

```cpp
bool DependencyManager::cloneGitRepo(...) {
    // 1. Check cache first
    auto cachedPath = cacheManager_.getCachedDependency(url, version);
    if (cachedPath) {
        // Copy from cache
        fs::copy(*cachedPath, destPath, fs::copy_options::recursive);
        return true;
    }

    // 2. Not cached, clone from git
    int result = std::system("git clone ...");

    // 3. Add to cache
    cacheManager_.cacheDependency(url, version, destPath);

    // 4. Save metadata for lock file
    saveMetadata(name, dep, resolvedCommit);
}
```

### Checksums

Currently uses simple hash-based checksums:

```cpp
std::string calculateChecksum(const std::string& dirPath) {
    // Concatenate: filename + size + modification time
    // Hash with std::hash<std::string>
    // Return: "sha256:abc123..."
}
```

**Note**: For production use, implement proper SHA-256 hashing using OpenSSL or similar library.

## Best Practices

### 1. Always Commit Lock Files

```bash
git add stratos.lock
git commit -m "Add/update dependencies"
```

### 2. Never Commit deps/

Add to `.gitignore`:
```gitignore
# Dependencies (regenerated from lock file)
deps/

# Build output
build/
*.ll
```

### 3. Verify Before Building

In CI/CD:
```bash
#!/bin/bash
# ci-build.sh

# Fetch dependencies from lock file
stratos get

# Verify they match
stratos get --verify || exit 1

# Build
stratos build
```

### 4. Periodic Cache Cleaning

Add to cron or CI maintenance:
```bash
# Clean cache entries older than 60 days
stratos cache clean --days 60
```

### 5. Use Specific Versions

Instead of:
```hocon
dependencies = [
  {name = my-lib, url = "github.com/user/my-lib"}  # Uses 'main' branch
]
```

Use:
```hocon
dependencies = [
  {name = my-lib, url = "github.com/user/my-lib", tag = "v1.2.3"}
]
```

This ensures:
- Reproducible builds
- No unexpected breaking changes
- Lock file tracks specific commit

## Troubleshooting

### Lock File Verification Fails

```
✗ Checksum mismatch for: my-lib
```

**Cause**: Dependency was modified locally or lock file is stale

**Solution**:
```bash
# Option 1: Refetch dependency
rm -rf deps/my-lib
stratos get

# Option 2: Regenerate lock file
stratos get --update
```

### Cache Copy Fails

```
Failed to copy from cache: Permission denied
```

**Cause**: Cache directory permissions

**Solution**:
```bash
# Fix permissions
chmod -R u+rw ~/.stratos/cache

# Or clear and rebuild cache
stratos cache clear
stratos get
```

### Missing Dependency After Pull

```
Error: Could not load module 'my-lib'
```

**Cause**: Forgot to run `stratos get` after pulling

**Solution**:
```bash
stratos get  # Fetches dependencies from lock file
```

## Future Enhancements

### Planned Features

1. **Symlink Cache Mode**: Symlink from `deps/` to cache instead of copying
   - Saves even more disk space
   - Instant dependency "fetching"
   - Requires read-only enforcement

2. **Cryptographic Checksums**: Use SHA-256 instead of simple hash
   - Better security
   - Detect corrupted dependencies
   - Industry standard

3. **Dependency Resolution**: Smart conflict resolution for transitive dependencies
   - Choose newest compatible version
   - Warn about version conflicts
   - Suggest resolution strategies

4. **Registry Support**: Central package registry (like npm, crates.io)
   - Easier dependency discovery
   - Versioning and deprecation info
   - Security advisories

5. **Lock File Diff**: Show what changed in lock file
   ```bash
   stratos lock diff
   ```
   Output:
   ```
   Dependencies updated:
   - my-lib: v1.0.0 -> v1.1.0
   + new-dep: v2.3.0 (new)
   - old-dep: v1.5.0 (removed)
   ```

6. **Integrity Verification**: Verify dependency integrity with signatures
   - GPG-signed dependencies
   - Verify author identity
   - Prevent supply chain attacks

## Summary

Stratos provides enterprise-grade dependency management through:

✅ **Lock Files**
- Ensure reproducible builds
- Track exact dependency versions
- Prevent "works on my machine" issues

✅ **Global Cache**
- Reduce disk usage across projects
- Speed up dependency fetching
- Enable offline development

✅ **Seamless Integration**
- Automatic cache management
- Transparent to developers
- Works with existing workflows

By combining lock files and global caching, Stratos ensures reliable, efficient, and reproducible builds across all environments.

## Quick Reference

```bash
# Fetch dependencies (creates/uses lock file and cache)
stratos get

# Fetch and update to latest versions
stratos get --update

# Verify lock file matches installed dependencies
stratos get --verify

# View cache stats
stratos cache stats

# Clean old cache entries (30+ days)
stratos cache clean

# Clear entire cache
stratos cache clear

# Use custom cache location
export STRATOS_CACHE=/path/to/cache
stratos get
```

For more information, see:
- `DEPENDENCY_SYSTEM_REQUIREMENTS.md` - System overview and testing
- `CONTRIBUTING.md` - Development setup and workflow
- `install.md` - Installation system documentation
