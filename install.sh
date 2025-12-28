#!/bin/bash
# Stratos Programming Language Installer
# Usage: curl -sSL https://get.stratos-lang.org | sh

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
STRATOS_VERSION="${STRATOS_VERSION:-latest}"
INSTALL_DIR="${STRATOS_INSTALL_DIR:-$HOME/.stratos}"
BIN_DIR="$INSTALL_DIR/bin"
GITHUB_REPO="Arthur-Kamau/stratos"
BINARY_NAME="stratos"

# Detect OS and architecture
detect_platform() {
    OS="$(uname -s)"
    ARCH="$(uname -m)"

    case "$OS" in
        Linux*)
            PLATFORM="linux"
            ;;
        Darwin*)
            PLATFORM="macos"
            ;;
        MINGW*|MSYS*|CYGWIN*)
            PLATFORM="windows"
            BINARY_NAME="stratos.exe"
            ;;
        *)
            echo -e "${RED}Error: Unsupported operating system: $OS${NC}"
            exit 1
            ;;
    esac

    case "$ARCH" in
        x86_64|amd64)
            ARCH="x86_64"
            ;;
        aarch64|arm64)
            ARCH="aarch64"
            ;;
        armv7l)
            ARCH="armv7"
            ;;
        *)
            echo -e "${RED}Error: Unsupported architecture: $ARCH${NC}"
            exit 1
            ;;
    esac

    echo -e "${BLUE}Detected platform: $PLATFORM-$ARCH${NC}"
}

# Get latest version from GitHub
get_latest_version() {
    if [ "$STRATOS_VERSION" = "latest" ]; then
        echo -e "${BLUE}Fetching latest version...${NC}"
        STRATOS_VERSION=$(curl -sSL https://api.github.com/repos/$GITHUB_REPO/releases/latest | grep '"tag_name":' | sed -E 's/.*"([^"]+)".*/\1/')

        if [ -z "$STRATOS_VERSION" ]; then
            echo -e "${YELLOW}Warning: Could not fetch latest version, using fallback${NC}"
            STRATOS_VERSION="v0.1.0"
        fi
    fi

    echo -e "${GREEN}Installing Stratos $STRATOS_VERSION${NC}"
}

# Download binary
download_binary() {
    local download_url="https://github.com/$GITHUB_REPO/releases/download/$STRATOS_VERSION/stratos-$PLATFORM-$ARCH"
    local temp_file="/tmp/stratos-download-$$"

    echo -e "${BLUE}Downloading from: $download_url${NC}"

    if command -v curl >/dev/null 2>&1; then
        curl -sSL -f "$download_url" -o "$temp_file" || {
            echo -e "${RED}Error: Failed to download binary${NC}"
            echo -e "${YELLOW}Building from source instead...${NC}"
            build_from_source
            return
        }
    elif command -v wget >/dev/null 2>&1; then
        wget -q "$download_url" -O "$temp_file" || {
            echo -e "${RED}Error: Failed to download binary${NC}"
            echo -e "${YELLOW}Building from source instead...${NC}"
            build_from_source
            return
        }
    else
        echo -e "${RED}Error: Neither curl nor wget found${NC}"
        exit 1
    fi

    # Create installation directory
    mkdir -p "$BIN_DIR"

    # Move binary to installation directory
    mv "$temp_file" "$BIN_DIR/$BINARY_NAME"
    chmod +x "$BIN_DIR/$BINARY_NAME"

    echo -e "${GREEN}✓ Binary installed to $BIN_DIR/$BINARY_NAME${NC}"
}

# Build from source (fallback)
build_from_source() {
    echo -e "${BLUE}Building Stratos from source...${NC}"

    # Check for required tools
    if ! command -v git >/dev/null 2>&1; then
        echo -e "${RED}Error: git is required to build from source${NC}"
        exit 1
    fi

    if ! command -v g++ >/dev/null 2>&1; then
        echo -e "${RED}Error: g++ is required to build from source${NC}"
        echo -e "${YELLOW}Install build tools:${NC}"
        echo -e "  Ubuntu/Debian: sudo apt-get install build-essential"
        echo -e "  macOS: xcode-select --install"
        exit 1
    fi

    # Clone repository
    local repo_dir="/tmp/stratos-build-$$"
    echo -e "${BLUE}Cloning repository...${NC}"
    git clone --depth 1 https://github.com/$GITHUB_REPO.git "$repo_dir" || {
        echo -e "${RED}Error: Failed to clone repository${NC}"
        exit 1
    }

    # Build
    cd "$repo_dir/interpreter/C++"
    echo -e "${BLUE}Building...${NC}"
    bash build.sh || {
        echo -e "${RED}Error: Build failed${NC}"
        exit 1
    }

    # Install
    mkdir -p "$BIN_DIR"
    cp build/stratos "$BIN_DIR/$BINARY_NAME"
    chmod +x "$BIN_DIR/$BINARY_NAME"

    # Cleanup
    rm -rf "$repo_dir"

    echo -e "${GREEN}✓ Built and installed from source${NC}"
}

# Setup PATH
setup_path() {
    local shell_rc=""
    local shell_name="$(basename "$SHELL")"

    case "$shell_name" in
        bash)
            shell_rc="$HOME/.bashrc"
            ;;
        zsh)
            shell_rc="$HOME/.zshrc"
            ;;
        fish)
            shell_rc="$HOME/.config/fish/config.fish"
            ;;
        *)
            shell_rc="$HOME/.profile"
            ;;
    esac

    # Check if already in PATH
    if echo "$PATH" | grep -q "$BIN_DIR"; then
        echo -e "${GREEN}✓ $BIN_DIR already in PATH${NC}"
        return
    fi

    # Add to shell rc file
    if [ -f "$shell_rc" ]; then
        echo "" >> "$shell_rc"
        echo "# Stratos" >> "$shell_rc"
        echo "export PATH=\"$BIN_DIR:\$PATH\"" >> "$shell_rc"
        echo -e "${GREEN}✓ Added $BIN_DIR to PATH in $shell_rc${NC}"
        echo -e "${YELLOW}Run: source $shell_rc (or restart your terminal)${NC}"
    else
        echo -e "${YELLOW}Warning: Could not find shell config file${NC}"
        echo -e "${YELLOW}Add this to your shell config:${NC}"
        echo -e "  export PATH=\"$BIN_DIR:\$PATH\""
    fi
}

# Verify installation
verify_installation() {
    if [ -x "$BIN_DIR/$BINARY_NAME" ]; then
        echo ""
        echo -e "${GREEN}✓ Stratos installed successfully!${NC}"
        echo ""
        echo -e "${BLUE}Installation location:${NC} $BIN_DIR/$BINARY_NAME"

        # Try to run and show version
        if echo "$PATH" | grep -q "$BIN_DIR"; then
            echo ""
            "$BIN_DIR/$BINARY_NAME" --version 2>/dev/null || echo -e "${BLUE}Version:${NC} $STRATOS_VERSION"
        fi

        echo ""
        echo -e "${BLUE}Next steps:${NC}"
        echo "  1. Restart your terminal or run: source ~/.bashrc (or your shell config)"
        echo "  2. Verify: stratos --version"
        echo "  3. Get started: stratos new my-project"
        echo ""
        echo -e "${BLUE}Documentation:${NC} https://stratos-lang.org/docs"
        echo -e "${BLUE}Examples:${NC} https://github.com/$GITHUB_REPO/tree/master/samples"
        echo ""
    else
        echo -e "${RED}Error: Installation verification failed${NC}"
        exit 1
    fi
}

# Cleanup on error
cleanup() {
    if [ $? -ne 0 ]; then
        echo -e "${RED}Installation failed!${NC}"
        echo -e "${YELLOW}Please report issues at: https://github.com/$GITHUB_REPO/issues${NC}"
    fi
}

trap cleanup EXIT

# Main installation flow
main() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}  Stratos Language Installer${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""

    detect_platform
    get_latest_version
    download_binary
    setup_path
    verify_installation
}

# Run main function
main
