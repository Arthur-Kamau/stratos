#!/bin/bash
# Stratos Project Reorganization Script
# This script reorganizes the project structure for better clarity

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Dry run mode
DRY_RUN=false
if [[ "$1" == "--dry-run" ]]; then
    DRY_RUN=true
    echo -e "${YELLOW}DRY RUN MODE - No changes will be made${NC}\n"
fi

# Function to execute commands (or just print in dry-run mode)
execute() {
    if $DRY_RUN; then
        echo -e "${BLUE}[DRY RUN]${NC} $*"
    else
        eval "$*"
    fi
}

echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}Stratos Project Reorganization${NC}"
echo -e "${GREEN}========================================${NC}\n"

# Check if we're in a git repository
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    echo -e "${RED}Error: Not in a git repository${NC}"
    exit 1
fi

# Check for uncommitted changes
if ! git diff-index --quiet HEAD -- 2>/dev/null; then
    echo -e "${YELLOW}Warning: You have uncommitted changes${NC}"
    echo -e "${YELLOW}It's recommended to commit or stash them first${NC}"
    read -p "Continue anyway? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo -e "${BLUE}Phase 1: Creating new directory structure${NC}\n"

# Create new directory structure
execute "mkdir -p docs/{user,developer/architecture,developer/implementation,devtools,design,changelog}"
execute "mkdir -p src/{interpreter,compiler,devtools}"
execute "mkdir -p scripts"

echo -e "\n${BLUE}Phase 2: Moving user documentation${NC}\n"

# User docs
if [ -f "CLI_REFERENCE.md" ]; then
    execute "git mv CLI_REFERENCE.md docs/user/cli-reference.md"
fi

if [ -f "install.md" ]; then
    execute "git mv install.md docs/user/installation.md"
fi

if [ -f "STYLE_GUIDE.md" ]; then
    execute "git mv STYLE_GUIDE.md docs/user/style-guide.md"
fi

if [ -f "docs/FFI.md" ]; then
    execute "git mv docs/FFI.md docs/user/ffi.md"
fi

echo -e "\n${BLUE}Phase 3: Moving developer documentation${NC}\n"

# Developer architecture docs
if [ -f "INTERPRETER_IMPLEMENTATION.md" ]; then
    execute "git mv INTERPRETER_IMPLEMENTATION.md docs/developer/architecture/interpreter.md"
fi

if [ -f "SEMANTIC_ANALYZER_IMPLEMENTATION.md" ]; then
    execute "git mv SEMANTIC_ANALYZER_IMPLEMENTATION.md docs/developer/architecture/semantic-analyzer.md"
fi

if [ -f "docs/MEMORY_MANAGEMENT.md" ]; then
    execute "git mv docs/MEMORY_MANAGEMENT.md docs/developer/architecture/memory-management.md"
fi

if [ -f "MEMORY_ANALYSIS.md" ]; then
    execute "git mv MEMORY_ANALYSIS.md docs/developer/architecture/memory-analysis.md"
fi

# Developer implementation docs
if [ -f "STDLIB_IMPLEMENTATION.md" ]; then
    execute "git mv STDLIB_IMPLEMENTATION.md docs/developer/implementation/stdlib.md"
fi

if [ -f "ENTRY_POINT_RESOLUTION.md" ]; then
    execute "git mv ENTRY_POINT_RESOLUTION.md docs/developer/implementation/entry-point-resolution.md"
fi

if [ -f "DEPENDENCY_SYSTEM_REQUIREMENTS.md" ]; then
    execute "git mv DEPENDENCY_SYSTEM_REQUIREMENTS.md docs/developer/implementation/dependency-system.md"
fi

if [ -f "DEPENDENCY_CACHE_AND_LOCK_FILES.md" ]; then
    execute "git mv DEPENDENCY_CACHE_AND_LOCK_FILES.md docs/developer/implementation/dependency-cache.md"
fi

# Testing and development process docs
if [ -f "TESTING_PLAN.md" ]; then
    execute "git mv TESTING_PLAN.md docs/developer/testing.md"
fi

if [ -f "GIT_HOOKS_SETUP.md" ]; then
    execute "git mv GIT_HOOKS_SETUP.md docs/developer/git-hooks.md"
fi

echo -e "\n${BLUE}Phase 4: Moving DevTools documentation${NC}\n"

if [ -f "DEVTOOLS_ROADMAP.md" ]; then
    execute "git mv DEVTOOLS_ROADMAP.md docs/devtools/roadmap.md"
fi

if [ -f "DEVTOOLS_PROGRESS.md" ]; then
    execute "git mv DEVTOOLS_PROGRESS.md docs/devtools/progress.md"
fi

if [ -f "devtools/QUICKSTART.md" ]; then
    execute "git mv devtools/QUICKSTART.md docs/devtools/quickstart.md"
fi

if [ -f "devtools/PROTOCOL.md" ]; then
    execute "git mv devtools/PROTOCOL.md docs/devtools/protocol.md"
fi

if [ -f "devtools/UI_MOCKUPS.md" ]; then
    execute "git mv devtools/UI_MOCKUPS.md docs/devtools/ui-mockups.md"
fi

if [ -f "devtools/README.md" ]; then
    execute "git mv devtools/README.md docs/devtools/README.md"
fi

echo -e "\n${BLUE}Phase 5: Moving design documentation${NC}\n"

if [ -f "design/grammar.ebnf" ]; then
    execute "git mv design/grammar.ebnf docs/design/"
fi

if [ -f "design/project_configuration.md" ]; then
    execute "git mv design/project_configuration.md docs/design/project-configuration.md"
fi

if [ -f "design/standard_library.md" ]; then
    execute "git mv design/standard_library.md docs/design/standard-library.md"
fi

# Move any other design docs
if [ -f "design/compiler_architecture.md" ]; then
    execute "git mv design/compiler_architecture.md docs/design/compiler-architecture.md"
fi

if [ -f "design/interpreter_implementation.md" ]; then
    execute "git mv design/interpreter_implementation.md docs/design/interpreter-implementation.md"
fi

if [ -f "design/oop_runtime_implementation.md" ]; then
    execute "git mv design/oop_runtime_implementation.md docs/design/oop-runtime-implementation.md"
fi

echo -e "\n${BLUE}Phase 6: Moving implementation summaries to changelog${NC}\n"

if [ -f "IMPLEMENTATION_SUMMARY.md" ]; then
    execute "git mv IMPLEMENTATION_SUMMARY.md docs/changelog/2024-12-26-initial-implementation.md"
fi

if [ -f "COMPLETE_INTEGRATION.md" ]; then
    execute "git mv COMPLETE_INTEGRATION.md docs/changelog/2024-12-27-complete-integration.md"
fi

if [ -f "MEMORY_FIX_ROADMAP.md" ]; then
    execute "git mv MEMORY_FIX_ROADMAP.md docs/changelog/2024-12-29-memory-fixes.md"
fi

if [ -f "MEMORY_FIX_IMPLEMENTATION_PLAN.md" ]; then
    execute "git mv MEMORY_FIX_IMPLEMENTATION_PLAN.md docs/changelog/2024-12-29-memory-fix-plan.md"
fi

if [ -f "FFI_IMPLEMENTATION_SUMMARY.md" ]; then
    execute "git mv FFI_IMPLEMENTATION_SUMMARY.md docs/changelog/2024-12-30-ffi.md"
fi

if [ -f "FIX_SUMMARY.md" ]; then
    execute "git mv FIX_SUMMARY.md docs/changelog/misc-fixes.md"
fi

if [ -f "COMPLETE_SETUP_SUMMARY.md" ]; then
    execute "git mv COMPLETE_SETUP_SUMMARY.md docs/changelog/setup-summary.md"
fi

if [ -f "TEST_SETUP_COMPLETE.md" ]; then
    execute "git mv TEST_SETUP_COMPLETE.md docs/changelog/test-setup.md"
fi

echo -e "\n${BLUE}Phase 7: Moving scripts${NC}\n"

if [ -f "install.sh" ]; then
    execute "git mv install.sh scripts/"
fi

if [ -f "test-all.sh" ]; then
    execute "git mv test-all.sh scripts/"
fi

echo -e "\n${BLUE}Phase 8: Moving source code${NC}\n"

# Move interpreter (if not already in src/)
if [ -d "interpreter/C++" ] && [ ! -d "src/interpreter/cpp" ]; then
    execute "git mv interpreter/C++ src/interpreter/cpp"
fi

# Move compiler cases
if [ -d "interpreter/cases" ] && [ ! -d "src/compiler/cases" ]; then
    execute "git mv interpreter/cases src/compiler/cases"
fi

# Move existing compiler if it exists
if [ -d "compiler" ] && [ ! -d "src/compiler" ]; then
    execute "git mv compiler/* src/compiler/ 2>/dev/null || true"
fi

# Move devtools UI
if [ -d "devtools/ui" ] && [ ! -d "src/devtools/ui" ]; then
    execute "git mv devtools/ui src/devtools/"
fi

echo -e "\n${BLUE}Phase 9: Renaming examples (samples → examples, kebab-case)${NC}\n"

if [ -d "samples" ] && [ ! -d "examples" ]; then
    execute "git mv samples examples"

    # Rename subdirectories to kebab-case
    cd examples 2>/dev/null || true

    for dir in */; do
        dir=${dir%/}  # Remove trailing slash
        kebab=$(echo "$dir" | sed 's/_/-/g')
        if [ "$dir" != "$kebab" ] && [ -d "$dir" ]; then
            execute "git mv \"$dir\" \"$kebab\""
        fi
    done

    cd ..
fi

echo -e "\n${BLUE}Phase 10: Creating README files${NC}\n"

# Create docs/README.md
if ! $DRY_RUN && [ ! -f "docs/README.md" ]; then
    cat > docs/README.md << 'EOF'
# Stratos Documentation

## Directory Structure

- **user/** - User-facing documentation (getting started, CLI reference, guides)
- **developer/** - Developer documentation (architecture, implementation details)
- **devtools/** - DevTools documentation and specifications
- **design/** - Language design documents and specifications
- **changelog/** - Implementation history and summaries

## Quick Links

### For Users
- [Installation Guide](user/installation.md)
- [CLI Reference](user/cli-reference.md)
- [Style Guide](user/style-guide.md)
- [FFI Guide](user/ffi.md)

### For Developers
- [Architecture Overview](developer/architecture/)
- [Testing Guide](developer/testing.md)
- [Implementation Guides](developer/implementation/)

### DevTools
- [Quick Start](devtools/quickstart.md)
- [Protocol Specification](devtools/protocol.md)
- [Roadmap](devtools/roadmap.md)

### Design
- [Grammar Specification](design/grammar.ebnf)
- [Standard Library Design](design/standard-library.md)
EOF
    echo -e "${GREEN}✓ Created docs/README.md${NC}"
fi

# Create docs/changelog/README.md
if ! $DRY_RUN && [ ! -f "docs/changelog/README.md" ]; then
    cat > docs/changelog/README.md << 'EOF'
# Implementation Changelog

This directory contains detailed implementation summaries and progress reports.

## Timeline

- **2024-12-26** - [Initial Implementation](2024-12-26-initial-implementation.md)
- **2024-12-27** - [Complete Integration](2024-12-27-complete-integration.md)
- **2024-12-29** - [Memory Management Fixes](2024-12-29-memory-fixes.md)
- **2024-12-30** - [FFI Implementation](2024-12-30-ffi.md)

## Other Documents

- [Setup Summary](setup-summary.md)
- [Test Setup](test-setup.md)
- [Miscellaneous Fixes](misc-fixes.md)
EOF
    echo -e "${GREEN}✓ Created docs/changelog/README.md${NC}"
fi

# Create examples/README.md
if ! $DRY_RUN && [ ! -f "examples/README.md" ]; then
    cat > examples/README.md << 'EOF'
# Stratos Examples

This directory contains example Stratos projects demonstrating various features.

## Basic Examples
- **hello-world/** - Simple hello world program
- **variables/** - Variable declarations and types
- **function-definition/** - Function examples
- **class-definition/** - OOP examples

## Advanced Examples
- **ffi-c-math/** - FFI with C library
- **ffi-cpp-string/** - FFI with C++ library
- **oop-demo/** - Object-oriented programming
- **stdlib-examples/** - Standard library usage

## Running Examples

```bash
cd examples/hello-world
../../scripts/stratos run
```

Or specify the file directly:

```bash
stratos run examples/hello-world/src/main.st
```
EOF
    echo -e "${GREEN}✓ Created examples/README.md${NC}"
fi

# Create scripts/README.md
if ! $DRY_RUN && [ ! -f "scripts/README.md" ]; then
    cat > scripts/README.md << 'EOF'
# Stratos Utility Scripts

- **install.sh** - Installation script
- **test-all.sh** - Run all tests
- **reorganize.sh** - Project reorganization script (this script)

## Usage

```bash
# Install Stratos
./scripts/install.sh

# Run tests
./scripts/test-all.sh

# Run tests with verbose output
./scripts/test-all.sh -v
```
EOF
    echo -e "${GREEN}✓ Created scripts/README.md${NC}"
fi

# Create src/README.md
if ! $DRY_RUN && [ ! -f "src/README.md" ]; then
    cat > src/README.md << 'EOF'
# Stratos Source Code

## Directory Structure

- **interpreter/cpp/** - C++ interpreter implementation
- **compiler/** - Future LLVM-based compiler
- **devtools/** - DevTools web UI and backend

## Building

### Interpreter

```bash
cd src/interpreter/cpp
./build.sh
```

### Running

```bash
./build/stratos run path/to/program.st
```

### With DevTools

```bash
./build/stratos run --devtools path/to/program.st
```
EOF
    echo -e "${GREEN}✓ Created src/README.md${NC}"
fi

echo -e "\n${BLUE}Phase 11: Cleaning up empty directories${NC}\n"

# Remove empty directories
for dir in design devtools interpreter compiler; do
    if [ -d "$dir" ] && [ -z "$(ls -A $dir 2>/dev/null)" ]; then
        execute "rmdir $dir"
        echo -e "${GREEN}✓ Removed empty directory: $dir${NC}"
    fi
done

echo -e "\n${GREEN}========================================${NC}"
echo -e "${GREEN}Reorganization Complete!${NC}"
echo -e "${GREEN}========================================${NC}\n"

if $DRY_RUN; then
    echo -e "${YELLOW}This was a dry run. No changes were made.${NC}"
    echo -e "${YELLOW}Run without --dry-run to apply changes:${NC}"
    echo -e "  ./reorganize.sh\n"
else
    echo -e "${GREEN}✓ Project reorganized successfully!${NC}\n"
    echo -e "Next steps:"
    echo -e "  1. Review the changes: ${BLUE}git status${NC}"
    echo -e "  2. Update any hardcoded paths in code/scripts"
    echo -e "  3. Update .gitignore if needed"
    echo -e "  4. Commit the changes: ${BLUE}git add -A && git commit -m 'Reorganize project structure'${NC}"
    echo -e "  5. Update README.md to reflect new structure\n"

    echo -e "${YELLOW}Files remaining in root:${NC}"
    ls -1 *.md 2>/dev/null | grep -v "README.md" || echo -e "${GREEN}  (none - clean!)${NC}"
    echo ""
fi
