# Memory Management Fix Roadmap

Detailed implementation plan for fixing critical memory issues and implementing missing GC features in Stratos.

## Overview

This document provides a prioritized, actionable plan to address the memory management issues identified in `MEMORY_ANALYSIS.md`.

**Total Tasks:** 12
**Critical (P0):** 4
**High Priority (P1):** 4
**Medium Priority (P2):** 2
**Nice to Have (P3):** 2

---

## Task Summary

| # | Task | Priority | Effort | Risk | Status |
|---|------|----------|--------|------|--------|
| 1 | Fix dangling pointers in Function struct | P0 | 1-2h | Low | ⏳ Pending |
| 2 | Fix dangling pointers in Class struct | P0 | 1h | Low | ⏳ Pending |
| 3 | Replace raw pointers in moduleFunctions | P0 | 2-3h | Med | ⏳ Pending |
| 4 | Add exception safety guards | P0 | 2h | Low | ⏳ Pending |
| 5 | Implement cycle detection | P1 | 4-6h | Med | ⏳ Pending |
| 6 | Add defensive null checks | P1 | 1h | Low | ⏳ Pending |
| 7 | Replace std::any with std::variant | P1 | 6-8h | High | ⏳ Pending |
| 8 | Fix environment pointer safety | P1 | 2-3h | Med | ⏳ Pending |
| 9 | Implement mark-and-sweep GC | P2 | 16-24h | High | ⏳ Pending |
| 10 | Add memory leak detection tests | P2 | 4-6h | Low | ⏳ Pending |
| 11 | Document reference counting limits | P3 | 2-3h | None | ⏳ Pending |
| 12 | Add lifetime documentation | P3 | 1h | None | ⏳ Pending |

---

## Phase 1: Critical Safety Fixes (MUST DO)

### Task 1: Fix Dangling Pointers in Function Struct ⚠️

**File:** `interpreter/C++/include/stratos/Interpreter.h:116-121`

**Current Code:**
```cpp
struct Function {
    std::unique_ptr<std::vector<std::unique_ptr<Stmt>>>* body;  // DANGEROUS!
};
```

**Fix (Recommended):**
```cpp
struct Function {
    std::vector<Token> params;
    std::vector<std::string> paramTypes;
    std::string returnType;
    std::reference_wrapper<const std::vector<std::unique_ptr<Stmt>>> body;
};

// Usage in visit(FunctionDecl& stmt):
func.body = std::cref(stmt.body);
```

**Files to Change:**
- `interpreter/C++/include/stratos/Interpreter.h` (line 120)
- `interpreter/C++/src/runtime/Interpreter.cpp` (line 535)

---

### Task 2: Fix Dangling Pointers in Class Struct ⚠️

**File:** `interpreter/C++/include/stratos/Interpreter.h:125-128`

**Fix:**
```cpp
struct Class {
    std::string name;
    std::reference_wrapper<const std::vector<std::unique_ptr<Stmt>>> methods;
};
```

**Files to Change:**
- `interpreter/C++/include/stratos/Interpreter.h` (line 127)
- `interpreter/C++/src/runtime/Interpreter.cpp` (line 545)

---

### Task 3: Replace Raw Pointers in moduleFunctions ⚠️

**File:** `interpreter/C++/include/stratos/Interpreter.h:138`

**Fix (Store by value):**
```cpp
std::unordered_map<std::string, std::unordered_map<std::string, FunctionDecl>> moduleFunctions;
```

**Files to Change:**
- `interpreter/C++/include/stratos/Interpreter.h` (line 138)
- `interpreter/C++/src/runtime/Interpreter.cpp` (line 528, module lookup)

---

### Task 4: Add Exception Safety Guards ⚠️

**File:** `interpreter/C++/include/stratos/Interpreter.h:30-37`

**Fix:**
```cpp
int asInt() const {
    if (type != "int") {
        throw RuntimeError("Type error: expected int, got " + type);
    }
    try {
        return std::any_cast<int>(value);
    } catch (const std::bad_any_cast&) {
        throw RuntimeError("Internal type mismatch");
    }
}

std::shared_ptr<ClassInstance> asObject() const {
    if (type != "object") {
        throw RuntimeError("Type error: expected object, got " + type);
    }
    auto obj = std::any_cast<std::shared_ptr<ClassInstance>>(value);
    if (!obj) {
        throw RuntimeError("Null object reference");
    }
    return obj;
}
```

---

## Phase 2: High Priority Improvements

### Task 5: Implement Cycle Detection 🔧

**Goal:** Warn users when circular references are created.

**Implementation:**
```cpp
class CycleDetector {
    bool detectCycle(const std::shared_ptr<ClassInstance>& root);
};

// Warn on assignment
void Interpreter::visit(AssignExpr& expr) {
    RuntimeValue value = evaluate(*expr.value);
    if (value.type == "object") {
        if (cycleDetector.detectCycle(value.asObject())) {
            std::cerr << "⚠️  Warning: Circular reference in '"
                      << expr.name << "' may cause memory leak\n";
        }
    }
    currentEnv->assign(expr.name, value);
}
```

**New Files:**
- `interpreter/C++/include/stratos/CycleDetector.h`
- `interpreter/C++/src/runtime/CycleDetector.cpp`

---

### Task 6: Add Defensive Null Checks 🔧

**Fix:**
```cpp
RuntimeValue Interpreter::visit(GetExpr& expr) {
    RuntimeValue object = evaluate(*expr.object);
    if (object.type != "object") {
        throw RuntimeError("Cannot access property on non-object");
    }
    auto instance = object.asObject();  // Now includes null check
    if (instance->fields.find(expr.name) == instance->fields.end()) {
        throw RuntimeError("Object has no property '" + expr.name + "'");
    }
    return instance->fields.at(expr.name);
}
```

---

### Task 7: Replace std::any with std::variant 🚀

**Why:** Better performance, no exceptions, compile-time safety

**Implementation:**
```cpp
struct RuntimeValue {
    std::variant<
        std::monostate,
        int,
        double,
        std::string,
        bool,
        std::shared_ptr<ClassInstance>
    > value;

    int asInt() const {
        if (auto* v = std::get_if<int>(&value)) return *v;
        throw RuntimeError("Type error");
    }
};
```

**Benefits:**
- ✅ Faster type checking
- ✅ Smaller memory footprint
- ✅ No runtime exceptions
- ✅ Better performance

---

### Task 8: Fix Environment Pointer Safety 🔧

**Option 1: Use Index (Recommended)**
```cpp
size_t currentEnvIndex = 0;
Environment* currentEnv() { return environments[currentEnvIndex].get(); }
```

**Option 2: Reserve Capacity**
```cpp
Interpreter() { environments.reserve(1000); }
```

---

## Phase 3: Advanced Features

### Task 9: Mark-and-Sweep GC 🚀

**Long-term goal:** Full garbage collection for cycles.

**Complexity:** High - 16-24 hours
**Priority:** P2 (optional for initial release)

---

### Task 10: Memory Leak Detection Tests 🧪

**Create test suite:**
```cpp
TEST(MemoryLeakTest, CyclicReference) {
    // Test that cycles are detected/warned about
}

TEST(MemoryLeakTest, LargeObjectCreation) {
    // Test that objects are properly freed
}
```

**Run with Valgrind:**
```bash
valgrind --leak-check=full ./stratos_tests
```

---

## Phase 4: Documentation

### Task 11: Document Memory Model 📚

Create `docs/MEMORY_MODEL.md`:
- Explain reference counting
- Warn about circular references
- Show best practices

### Task 12: Add Lifetime Comments 📚

Document all non-owning pointers with lifetime invariants.

---

## Implementation Timeline

### Week 1: Critical Fixes
- **Mon-Tue:** Tasks 1-2 (Fix dangling pointers)
- **Wed:** Task 3 (Fix moduleFunctions)
- **Thu-Fri:** Task 4 (Exception safety)

### Week 2: Improvements
- **Mon-Tue:** Task 6 (Null checks)
- **Wed-Fri:** Task 7 (std::variant migration)

### Week 3: Detection
- **Mon-Wed:** Task 5 (Cycle detection)
- **Thu-Fri:** Task 10 (Leak tests)

### Week 4: Polish
- **Mon:** Task 11 (Documentation)
- **Tue:** Task 12 (Lifetime comments)
- **Wed:** Task 8 (Environment safety)
- **Thu-Fri:** Task 9 (Start GC implementation - ongoing)

---

## Testing Checklist

After each task:
- [ ] All existing tests pass
- [ ] New tests added for the fix
- [ ] Valgrind shows no new leaks
- [ ] Performance benchmarks run
- [ ] Code reviewed

---

## Tools Required

**Install:**
```bash
sudo apt-get install valgrind google-benchmark-dev libgtest-dev
```

**Build with sanitizers:**
```bash
g++ -fsanitize=address -fsanitize=undefined -g src/**/*.cpp
```

---

## Success Criteria

**Phase 1 Complete:**
- ✅ No dangling pointers
- ✅ All exceptions handled
- ✅ Valgrind: 0 errors

**Phase 2 Complete:**
- ✅ Cycle warnings work
- ✅ Null checks prevent crashes
- ✅ Performance improved with variant

**Final Release:**
- ✅ All 12 tasks complete
- ✅ Full test coverage
- ✅ Documentation complete
- ✅ Production-ready

---

## Questions to Answer

1. Can AST structure be modified? → Affects Task 3
2. Is performance critical? → Affects Task 7 priority
3. Need full GC support? → Affects Task 9
4. Release timeline? → Affects sprint planning

---

See `MEMORY_ANALYSIS.md` for detailed issue descriptions.
