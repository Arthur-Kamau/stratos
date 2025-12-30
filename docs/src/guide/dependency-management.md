# Dependency Management

Stratos provides a powerful dependency management system inspired by Go modules, with support for transitive dependencies, lock files, and both local and remote packages.

## Quick Start

Add dependencies to your `stratos.conf` file:

```hocon
dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
]
```

Fetch dependencies:

```bash
stratos get
```

Use in your code:

```stratos
use http;

fn main() async {
    val response = await http.get("https://api.example.com");
    print(response.text());
}
```

## Configuration Format

Dependencies are defined in `stratos.conf` using HOCON syntax:

```hocon
project {
  name = my-app
  version = "1.0.0"
}

build {
  entry = src/main.st
  output = build/my-app
}

dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
  {
    name = json
    url = "https://github.com/stratos-lang/json"
    branch = main
  }
  {
    name = utils
    url = "../shared-utils"
  }
]
```

## Dependency Types

### Git Dependencies

Reference packages from Git repositories:

```hocon
dependencies = [
  # By tag (recommended for production)
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }

  # By branch
  {
    name = experimental
    url = "https://github.com/user/experimental"
    branch = develop
  }

  # By commit hash
  {
    name = specific
    url = "https://github.com/user/specific"
    hash = "abc123def456"
  }
]
```

### Local Dependencies

Reference local packages on your filesystem:

```hocon
dependencies = [
  # Relative path
  {
    name = shared
    url = "../shared-lib"
  }

  # Absolute path
  {
    name = core
    url = "/home/user/libs/core"
  }
]
```

## Version Specification

### Semantic Versioning

Stratos supports semantic versioning tags:

```hocon
{ name = pkg, url = "...", tag = "v1.2.3" }
{ name = pkg, url = "...", tag = "v2.0.0" }
{ name = pkg, url = "...", tag = "v1.0.0-beta.1" }
```

### Version Priority

When multiple version specifiers are present, Stratos uses this priority:

1. `tag` - Specific version tag
2. `branch` - Branch name
3. `hash` - Specific commit hash
4. Default - `main` branch

## Lock Files

After running `stratos get`, a `stratos.lock` file is generated:

```hocon
metadata {
  generated = "2025-12-27T10:30:00Z"
  stratos_version = "0.1.0"
}

dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
    resolved_commit = "abc123def456"
    type = git
  }
  {
    name = json
    url = "https://github.com/stratos-lang/json"
    branch = main
    resolved_commit = "fed654cba321"
    type = git
    via = http  # Transitive dependency
  }
  {
    name = utils
    url = "../shared-utils"
    type = local
    checksum = "sha256:abcd1234..."
  }
]
```

### Lock File Benefits

- **Reproducible builds**: Exact versions are locked
- **Transitive tracking**: Dependencies of dependencies are recorded
- **Integrity checking**: Checksums prevent tampering
- **Version resolution**: The `via` field shows dependency relationships

::: tip
Always commit `stratos.lock` to version control for reproducible builds across environments.
:::

## Commands

### stratos get

Fetch all dependencies:

```bash
stratos get
```

Fetch with verbose output:

```bash
stratos get -v
```

### stratos get \<url>

Fetch a specific dependency:

```bash
stratos get github.com/stratos-lang/http@v2.1.0
```

Supported URL formats:

```bash
# GitHub with tag
stratos get github.com/user/repo@v1.0.0

# GitHub with branch
stratos get github.com/user/repo@main

# Full URL
stratos get https://github.com/user/repo

# Local path
stratos get ../local-lib
```

### stratos get --update

Update all dependencies and regenerate lock file:

```bash
stratos get --update
```

### stratos get --verify

Verify lock file matches installed dependencies:

```bash
stratos get --verify
```

## Transitive Dependencies

Stratos automatically resolves dependencies of dependencies:

```
my-app
├── http-lib (v2.1.0)
│   └── json-parser (v1.5.0)
│       └── unicode-lib (v1.0.0)
└── database (v3.0.0)
    └── json-parser (v1.5.0)  # Shared dependency
```

The lock file tracks all transitive dependencies:

```hocon
dependencies = [
  {
    name = http-lib
    tag = "v2.1.0"
    # ... direct dependency
  }
  {
    name = json-parser
    tag = "v1.5.0"
    via = http-lib  # Came from http-lib
  }
  {
    name = unicode-lib
    tag = "v1.0.0"
    via = json-parser  # Came from json-parser
  }
  {
    name = database
    tag = "v3.0.0"
    # ... direct dependency
  }
]
```

## Circular Dependency Detection

Stratos detects circular dependencies:

```
Package A → Package B → Package C → Package A  # ❌ Error!
```

Error message:

```
Error: Circular dependency detected: A → B → C → A
```

## Dependency Cache

Downloaded dependencies are cached in `~/.stratos/cache/`:

```
~/.stratos/cache/
├── github.com/
│   └── stratos-lang/
│       ├── http@v2.1.0/
│       └── json@v1.5.0/
└── metadata/
```

### Cache Management

Clear the cache:

```bash
rm -rf ~/.stratos/cache
```

Set custom cache location:

```bash
export STRATOS_CACHE=~/my-cache
```

## Creating Packages

### Package Structure

```
my-package/
├── stratos.conf         # Package configuration
├── src/
│   └── init.st         # Package entry point
├── README.md
└── LICENSE
```

### stratos.conf for Libraries

```hocon
project {
  name = my-package
  version = "1.0.0"
  author = "Your Name"
  type = library
  description = "Package description"
}

build {
  source_dir = src
}

exports {
  main = src/init.st
}

dependencies = []
```

### Publishing on GitHub

1. Create a repository
2. Tag releases with semantic versions:

```bash
git tag v1.0.0
git push origin v1.0.0
```

3. Users can reference it:

```hocon
dependencies = [
  {
    name = yourpackage
    url = "https://github.com/user/yourpackage"
    tag = "v1.0.0"
  }
]
```

## Example: Multi-Package Project

### Project Structure

```
my-app/
├── stratos.conf
├── src/
│   └── main.st
└── deps/                # Downloaded dependencies
    ├── http-lib/
    ├── json-parser/
    └── database/
```

### stratos.conf

```hocon
project {
  name = my-app
  version = "1.0.0"
}

build {
  entry = src/main.st
  output = build/my-app
}

dependencies = [
  {
    name = http
    url = "https://github.com/stratos-lang/http"
    tag = "v2.1.0"
  }
  {
    name = db
    url = "https://github.com/stratos-lang/postgresql"
    tag = "v1.3.0"
  }
  {
    name = logging
    url = "../shared/logging"
  }
]
```

### src/main.st

```stratos
package main;

use http;
use db;
use logging;

fn main() async {
    logging.init("my-app");

    val database = db.connect("postgresql://localhost/mydb");

    val server = http.Server(8080);

    server.get("/users", fn(req, res) async {
        val users = await database.query("SELECT * FROM users");
        res.json(users);
    });

    logging.info("Server starting on port 8080");
    server.listen();
}
```

### Workflow

```bash
# Fetch dependencies
stratos get

# Build
stratos build

# Run
./build/my-app
```

## Best Practices

::: tip Pin versions in production
Use specific tags rather than branches for production dependencies:

```hocon
# Good
{ name = http, url = "...", tag = "v2.1.0" }

# Risky for production
{ name = http, url = "...", branch = main }
```
:::

::: tip Commit lock file
Always commit `stratos.lock` to version control for reproducible builds.
:::

::: tip Minimize dependencies
Only add dependencies you actually need. Each dependency adds complexity and potential security risks.
:::

::: warning Update regularly
Keep dependencies up to date to get security fixes and improvements:

```bash
stratos get --update
```
:::

## Troubleshooting

### Dependency Not Found

```bash
Error: Failed to fetch dependency: https://github.com/user/repo
```

Solution:
- Check the URL is correct
- Verify the tag/branch exists
- Check network connection

### Circular Dependency

```bash
Error: Circular dependency detected: A → B → C → A
```

Solution:
- Refactor shared code into a separate package
- Remove the circular reference

### Lock File Mismatch

```bash
Error: Lock file does not match dependencies
```

Solution:
```bash
stratos get --update  # Regenerate lock file
```

## Next Steps

- [Packages Guide](/guide/packages) - Learn about the package system
- [CLI Reference - stratos get](/reference/cli#stratos-get) - Command details
- [Creating Packages](/guide/packages#creating-packages) - Publish your own packages

::: tip
For a complete example, check out the [stratos-sample-lib](https://github.com/stratos-lang/stratos-sample-lib) project.
:::
