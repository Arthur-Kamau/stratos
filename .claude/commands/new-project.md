# Create a new Stratos project

Create a new Stratos project with proper structure.

## Arguments
- $ARGUMENTS: Project name and optional flags (e.g., "my-app --with-http --with-db")

## Instructions

1. Parse the project name from $ARGUMENTS. If no name is given, ask the user.
2. Parse optional flags: `--with-http`, `--with-db`, `--with-async`, `--with-tests`, `--with-ffi`, `--with-websocket`, `--lib` (library instead of executable).
3. Create the project directory structure under the current working directory:

```
<project-name>/
├── stratos.conf
├── src/
│   └── main.st
└── (optional test/ directory if --with-tests)
```

4. Generate `stratos.conf`:
```hocon
project {
  name = <project_name>
  version = "0.1.0"
  description = ""
  type = executable  // or "library" if --lib
}

build {
  entry = src/main.st
  output = build/<project_name>
}

dependencies {
  // Add dependencies here
  // example = "https://github.com/user/stratos-example.git"
}
```

5. Generate `src/main.st` with appropriate boilerplate based on flags:

**Default (no flags):**
```stratos
package main;

fn main() {
    println("Hello from <project_name>!");
}
```

**With --with-http:**
```stratos
package main;

import "std/net";

fn main() {
    val server = HttpServer();

    server.get("/", fn(req: Request, res: Response) {
        res.send("Hello from <project_name>!");
    });

    println("Server starting on port 8080...");
    server.listen(8080);
}
```

**With --with-db:**
```stratos
package main;

import "std/db";

fn main() {
    val db = Database.connect("sqlite:<project_name>.db");

    db.execute("CREATE TABLE IF NOT EXISTS items (id INTEGER PRIMARY KEY, name TEXT)");
    db.execute("INSERT INTO items (name) VALUES (?)", ["example"]);

    val rows = db.query("SELECT * FROM items");
    for row in rows {
        println("Item: ${row["name"]}");
    }

    db.close();
}
```

**With --with-async:**
```stratos
package main;

import "std/async";

async fn fetchData() string {
    // Simulate async work
    await delay(1000);
    return "Data loaded";
}

fn main() {
    val result = await fetchData();
    println(result);
}
```

**With --with-tests:** Also create `test/main_test.st`:
```stratos
package test;

import "std/testing";

fn testExample(t: Test) {
    t.assertEqual(1 + 1, 2, "basic math works");
    t.assertTrue(true, "truth is true");
}

fn main() {
    val suite = TestSuite("Main Tests");
    suite.add("example test", testExample);
    suite.run();
}
```

6. If multiple `--with-*` flags are provided, combine the imports and boilerplate logically in a single `main.st`.

7. Print a summary:
```
Created project: <project_name>

  <project_name>/
  ├── stratos.conf
  ├── src/
  │   └── main.st
  └── test/           (if --with-tests)
      └── main_test.st

Next steps:
  cd <project_name>
  stratos run .
```
