# Git Hooks Setup - Pre-Commit Testing

**Date:** 2025-12-28
**Status:** ✅ Active

---

## Overview

The Stratos project now has an automated **pre-commit git hook** that runs the test suite before allowing commits. This ensures that broken code is never committed to the repository.

---

## How It Works

### When You Commit:

```bash
git add .
git commit -m "Your changes"
```

### What Happens:

1. **Git triggers the pre-commit hook**
2. **Hook runs test suite** (`./test-all.sh --no-rebuild`)
3. **If all tests pass (9/9):**
   - ✅ Commit is created
   - ✅ You see success message
4. **If any test fails:**
   - ❌ Commit is aborted
   - ❌ You see which tests failed
   - ❌ You must fix before committing

---

## Example Output

### Successful Commit (Tests Pass)

```bash
$ git commit -m "Fix interpreter bug"

========================================
Running Stratos Tests Before Commit
========================================

========================================
Stratos Comprehensive Test Suite
========================================

[1/3] Skipping rebuild (--no-rebuild)

[2/3] Discovering tests...
Found 9 tests to run

[3/3] Running tests...

✓ hello_world
✓ dependency_test
✓ function_definition
✓ oop_demo
✓ class_definition
✓ loops
✓ variables
✓ prelude_demo
✓ package

========================================
Test Summary
========================================
Total tests run:    9
Passed:             9
Failed:             0

Pass rate: 100%

All tests passed! ✓

✓ All tests passed! Proceeding with commit.

[master abc1234] Fix interpreter bug
 1 file changed, 5 insertions(+), 2 deletions(-)
```

### Failed Commit (Tests Fail)

```bash
$ git commit -m "Broken change"

========================================
Running Stratos Tests Before Commit
========================================

========================================
Stratos Comprehensive Test Suite
========================================

[1/3] Skipping rebuild (--no-rebuild)

[2/3] Discovering tests...
Found 9 tests to run

[3/3] Running tests...

✓ hello_world
✗ dependency_test (exit code: 1)
✓ function_definition
...

========================================
Test Summary
========================================
Total tests run:    9
Passed:             8
Failed:             1

✗ Tests failed! Commit aborted.

Fix the failing tests and try again, or use:
  git commit --no-verify  (to skip tests - NOT RECOMMENDED)

To debug:
  ./test-all.sh -v
```

**Result:** Commit was NOT created. You must fix the issue first.

---

## Hook Configuration

### Location
```
.git/hooks/pre-commit
```

### What It Does
1. Checks that `test-all.sh` exists
2. Runs `./test-all.sh --no-rebuild` (fast mode)
3. If exit code = 0 → Allow commit
4. If exit code ≠ 0 → Abort commit

### Why `--no-rebuild`?
- The hook assumes you've already built and tested locally
- Skipping rebuild makes the hook faster
- If you need to rebuild, run `./test-all.sh` manually first

---

## Bypassing the Hook (Emergency Only)

### Skip Hook for One Commit

```bash
git commit --no-verify -m "Emergency fix"
```

**⚠️ WARNING:** Only use `--no-verify` in emergencies:
- Critical production bug fix
- Documentation-only changes
- You've verified tests pass manually

**DO NOT** make it a habit!

---

## Common Scenarios

### Scenario 1: Tests Pass Locally, Hook Runs Them Again

**Why:** The hook ensures tests pass at commit time, even if you made changes after running tests.

**Solution:** This is intentional. The hook is fast (~5-10 seconds).

---

### Scenario 2: Hook Says Tests Failed, But They Pass Manually

**Possible causes:**
1. You modified code after running tests manually
2. Working directory is dirty
3. Build is out of sync

**Solution:**
```bash
# Rebuild and test manually
./test-all.sh

# Then commit
git commit -m "Your changes"
```

---

### Scenario 3: Hook is Slow

**Current speed:** ~5-10 seconds (with `--no-rebuild`)

**If too slow:**
1. Check that `--no-rebuild` is in the hook (it is)
2. Consider if all 9 tests are necessary
3. Remember: This catches bugs before they're committed!

---

### Scenario 4: I Want to Commit Anyway

**Option 1 (Recommended):** Fix the failing tests

```bash
# Debug the failure
./test-all.sh -v

# Fix the issue
vim interpreter/C++/src/...

# Rebuild and test
./test-all.sh

# Commit
git commit -m "Your changes"
```

**Option 2 (Not Recommended):** Skip the hook

```bash
git commit --no-verify -m "Your changes"
```

---

## Disabling the Hook Permanently

### Temporary Disable (Rename)

```bash
mv .git/hooks/pre-commit .git/hooks/pre-commit.disabled
```

**To re-enable:**
```bash
mv .git/hooks/pre-commit.disabled .git/hooks/pre-commit
```

### Permanent Disable (Delete)

```bash
rm .git/hooks/pre-commit
```

**⚠️ NOT RECOMMENDED:** You lose automatic safety checks!

---

## Best Practices

### ✅ DO:
- Run tests manually before committing (`./test-all.sh`)
- Let the hook run as a safety check
- Fix failing tests before committing
- Use `--no-verify` only in emergencies

### ❌ DON'T:
- Use `--no-verify` regularly
- Disable the hook permanently
- Commit without testing
- Ignore test failures

---

## Workflow with Git Hook

### Standard Workflow

```bash
# 1. Make changes
vim interpreter/C++/src/runtime/Interpreter.cpp

# 2. Build
cd interpreter/C++
bash build.sh

# 3. Test manually (optional but recommended)
cd ../..
./test-all.sh

# 4. Stage changes
git add interpreter/C++/src/runtime/Interpreter.cpp

# 5. Commit (hook runs automatically)
git commit -m "Fix: Improve module loading

- Updated UseStmt to handle edge case
- All tests passing"

# Hook runs tests...
# ✓ Tests pass → Commit created

# 6. Push
git push
```

### Fast Workflow (Trusting the Hook)

```bash
# 1. Make changes
vim interpreter/C++/src/runtime/Interpreter.cpp

# 2. Build
cd interpreter/C++
bash build.sh
cd ../..

# 3. Commit (let hook verify)
git add .
git commit -m "Your changes"

# Hook runs tests automatically
# If they fail, you'll know immediately
```

---

## Troubleshooting

### Hook Doesn't Run

**Check:**
```bash
# 1. Is the hook file present?
ls -la .git/hooks/pre-commit

# 2. Is it executable?
chmod +x .git/hooks/pre-commit

# 3. Test it manually
.git/hooks/pre-commit
```

### Hook Runs But Tests Don't Execute

**Check:**
```bash
# 1. Does test-all.sh exist?
ls -la test-all.sh

# 2. Is it executable?
chmod +x test-all.sh

# 3. Can you run it manually?
./test-all.sh
```

### Hook Exits with Error

**Debug:**
```bash
# Run hook manually to see full output
.git/hooks/pre-commit

# Check what's failing
./test-all.sh -v
```

---

## Additional Hooks (Future)

### Post-Commit Hook (Optional)

Runs AFTER commit is created. Could be used for:
- Logging commit stats
- Updating documentation
- Notifying team

**Not currently implemented** (pre-commit is sufficient)

### Pre-Push Hook (Optional)

Runs before `git push`. Could run:
- Full test suite (including slow tests)
- Integration tests
- Security scans

**Not currently implemented** (can add if needed)

---

## Summary

✅ **Git pre-commit hook is active and working**

**Benefits:**
- ✅ Prevents committing broken code
- ✅ Automatic safety net
- ✅ Fast feedback (~5-10 seconds)
- ✅ Team-wide quality enforcement
- ✅ Reduces debugging time

**Usage:**
- Just commit normally: `git commit -m "Your changes"`
- Hook runs automatically
- Tests must pass for commit to succeed

**Emergency bypass:**
```bash
git commit --no-verify -m "Emergency fix"
```

**Remember:** The hook is your friend - it catches bugs before they become commits!

---

## Files

- **Hook script:** `.git/hooks/pre-commit`
- **Test runner:** `test-all.sh`
- **Documentation:** `TESTING_PLAN.md`, `GIT_HOOKS_SETUP.md`

---

## Maintenance

### When Adding New Tests

1. Add test to `test-all.sh` in `RUNNABLE_SAMPLES` array
2. Hook automatically picks up new tests
3. No changes to hook needed

### When Modifying Hook Behavior

Edit: `.git/hooks/pre-commit`

Example changes:
- Add `--verbose` flag for more output
- Change timeout settings
- Add additional checks (linting, formatting, etc.)

---

**Status: Active and tested ✅**
