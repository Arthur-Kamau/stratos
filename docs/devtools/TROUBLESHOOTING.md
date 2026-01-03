# DevTools Troubleshooting Guide

## Common Issues and Solutions

### Issue: "No log entries to display" in Browser

This is the most common issue when first using DevTools. Here's why it happens and how to fix it:

#### Problem
The DevTools UI polls for events every 100ms. If your program finishes before the browser connects, the events are lost.

#### Solution: Use a Long-Running Program

Create a test program that runs long enough for you to open the browser:

```stratos
package main;

use log;

fn main() {
    log.info("Program starting - open http://localhost:8080 NOW!");

    val count = 0;
    while (count < 10) {
        val count = count + 1;
        log.info("Log message " + count);

        // Sleep for ~1 second
        val i = 0;
        while (i < 100000000) {
            val i = i + 1;
        }
    }

    log.info("Program ending");
}
```

### Step-by-Step Testing Procedure

1. **Start the DevTools UI Server** (keep this running in terminal 1):
   ```bash
   cd src/devtools/ui
   python3 -m http.server 8080
   ```

2. **Run your Stratos program with --devtools** (in terminal 2):
   ```bash
   ./src/interpreter/cpp/build/stratos run --devtools tests/devtools/live_demo.st
   ```

3. **IMMEDIATELY open your browser** to `http://localhost:8080`

4. **Check the connection status**:
   - Top-right corner should show: "Connected • localhost:9222" (green dot)
   - If it shows "Disconnected" (red dot), the program may have already finished

5. **View logs**:
   - Click the "Logging" tab
   - You should see logs streaming in real-time
   - Try the Memory tab and click "Enable Memory Profiling"

### Debugging Connection Issues

#### Check if DevTools Server is Running

```bash
curl http://localhost:9222
```

Expected response:
```json
{"name":"Stratos DevTools","version":"1.0.0","protocol":"1.0.0"}
```

If you get "Connection refused", the Stratos program has already finished.

#### Check for Events

```bash
curl http://localhost:9222/events
```

Expected response:
```json
[
  {"jsonrpc":"2.0","method":"Log.entryAdded","params":{...}},
  ...
]
```

If you get `[]` (empty array), either:
- No logs have been generated yet
- Events were already polled and cleared
- The program hasn't started logging yet

#### Browser Console Debugging

Open browser DevTools (F12) and check the Console tab:

**Good output:**
```
Connected to Stratos DevTools server: {name: "Stratos DevTools", version: "1.0.0", ...}
```

**Bad output:**
```
DevTools server not available, using demo mode
Attempting to reconnect...
```

This means the server isn't running or the program has finished.

### Connection Status Indicator

Look at the top-right of the DevTools UI:

- 🟢 **"Connected • localhost:9222"** = Good! Logs should appear
- 🔴 **"Disconnected"** = Program not running or finished

### Common Mistakes

1. **Opening browser AFTER program finishes**
   - Solution: Use longer-running programs or keep program in infinite loop during testing

2. **Wrong UI server directory**
   - Make sure you're running `python3 -m http.server 8080` from `src/devtools/ui/`

3. **Port conflicts**
   - If port 9222 or 8080 is already in use, you'll need to kill the existing process:
   ```bash
   lsof -ti:9222 | xargs kill -9
   lsof -ti:8080 | xargs kill -9
   ```

4. **Browser cache**
   - Hard refresh the browser: `Ctrl+Shift+R` (Linux/Windows) or `Cmd+Shift+R` (Mac)

### Testing with Provided Demo Files

We provide test programs designed for DevTools testing:

#### Quick Test (finishes in ~10 seconds):
```bash
./src/interpreter/cpp/build/stratos run --devtools tests/devtools/logging_demo.st
```

#### Long-Running Test (30 seconds with continuous logs):
```bash
./src/interpreter/cpp/build/stratos run --devtools tests/devtools/live_demo.st
```

#### Memory Profiling Test:
```bash
./src/interpreter/cpp/build/stratos run --devtools tests/devtools/memory_demo.st
```

### Infinite Loop Programs (For Extended Testing)

If you want to test DevTools without time pressure, use a program with an intentional infinite loop:

```stratos
package main;

use log;

fn main() {
    log.info("DevTools Test - Running Forever");
    log.info("Press Ctrl+C to stop");

    val count = 0;
    while (true) {
        val count = count + 1;

        if (count % 100 == 0) {
            log.info("Still running... count: ");
        }

        if (count % 1000 == 0) {
            log.warn("1000 iterations reached");
        }
    }
}
```

Then open the browser at your leisure. Press `Ctrl+C` in the terminal to stop the program when done.

### Memory View Issues

If the Memory tab shows no data:

1. Click **"Enable Memory Profiling"** button
2. Click **"Refresh Stats"** to manually update
3. Run a program that creates many objects:
   ```bash
   ./src/interpreter/cpp/build/stratos run --devtools tests/devtools/memory_demo.st
   ```

### Network Issues

If you can't connect even though everything seems correct:

1. Check firewall settings (localhost should always work, but check anyway)
2. Try a different browser
3. Check if another process is using port 9222:
   ```bash
   lsof -i:9222
   ```

### Still Not Working?

Enable verbose browser console logging:

1. Open browser DevTools (F12)
2. Go to Console tab
3. Look for error messages
4. Common errors:
   - `CORS error`: UI server might be on wrong port
   - `404 Not Found`: Check you're accessing correct URL
   - `ERR_CONNECTION_REFUSED`: Stratos program not running

### Best Practices

1. **Always start UI server first** (and keep it running)
2. **Open browser BEFORE running program** (or use long-running programs)
3. **Check connection status** (green dot in top-right)
4. **Use browser console** for debugging
5. **Test with demo files** first before using your own programs

### Example Full Workflow

```bash
# Terminal 1: Start UI server
cd ~/Development/Projects/stratos/src/devtools/ui
python3 -m http.server 8080
# Keep this running!

# Terminal 2: (in another terminal)
cd ~/Development/Projects/stratos

# Open browser FIRST
# Navigate to http://localhost:8080

# THEN run program
./src/interpreter/cpp/build/stratos run --devtools tests/devtools/live_demo.st

# Watch logs appear in browser!
```

## Success Checklist

- [ ] UI server running on port 8080
- [ ] Browser open at http://localhost:8080
- [ ] Connection status shows "Connected • localhost:9222"
- [ ] Logging tab active
- [ ] Program running with logs being generated
- [ ] Logs appearing in browser in real-time

If all items are checked, DevTools is working correctly! 🎉

## Quick Reference

| What | Command |
|------|---------|
| Start UI Server | `cd src/devtools/ui && python3 -m http.server 8080` |
| Run with DevTools | `stratos run --devtools <file.st>` |
| Open UI | http://localhost:8080 |
| Test Server | `curl http://localhost:9222` |
| Check Events | `curl http://localhost:9222/events` |
| Kill Port 9222 | `lsof -ti:9222 \| xargs kill -9` |
| Kill Port 8080 | `lsof -ti:8080 \| xargs kill -9` |
