# Installation

Install Stratos on your system and set up your development environment.

## System Requirements

- **Operating System**: Linux, macOS, or Windows (WSL recommended)
- **Memory**: 512 MB RAM minimum, 2 GB recommended
- **Disk Space**: 100 MB for installation

## Quick Installation

### Linux and macOS

The fastest way to install Stratos:

```bash
curl -sSL https://get.stratos-lang.org | sh
```

This script will:
1. Auto-detect your operating system and architecture
2. Download the latest Stratos release from GitHub
3. Fall back to building from source if binary not available
4. Install it to `~/.stratos` (customizable via `STRATOS_INSTALL_DIR`)
5. Add the binary to your PATH automatically
6. Verify the installation

### Windows (WSL)

On Windows, we recommend using Windows Subsystem for Linux (WSL):

1. Install WSL2
2. Open your WSL terminal
3. Run the Linux installation command:

```bash
curl -sSL https://get.stratos-lang.org | sh
```

### Custom Installation Directory

To install to a custom location:

```bash
curl -sSL https://get.stratos-lang.org | STRATOS_INSTALL_DIR=/opt/stratos sh
```

### Install Specific Version

To install a specific version:

```bash
curl -sSL https://get.stratos-lang.org | STRATOS_VERSION=v0.1.0 sh
```

## Manual Installation

### Download

Visit the [releases page](https://github.com/Arthur-Kamau/stratos/releases) and download the appropriate binary for your system:

- `stratos-linux-x86_64` - Linux (64-bit)
- `stratos-macos-x86_64` - macOS (Intel)
- `stratos-macos-aarch64` - macOS (Apple Silicon)

Or download the compressed archives:
- `stratos-linux-x86_64.tar.gz`
- `stratos-macos-x86_64.tar.gz`
- `stratos-macos-aarch64.tar.gz`

### Extract

```bash
tar -xzf stratos-*.tar.gz
cd stratos
```

### Install

```bash
sudo mv stratos /usr/local/bin/
sudo chmod +x /usr/local/bin/stratos
```

Or install to a custom location:

```bash
mkdir -p ~/bin
mv stratos ~/bin/
export PATH="$HOME/bin:$PATH"
```

Add the export line to your shell profile (`~/.bashrc`, `~/.zshrc`, etc.) to make it permanent.

## Verify Installation

Check that Stratos is installed correctly:

```bash
stratos --version
```

You should see output like:

```
Stratos 0.1.0
```

## Building from Source

### Prerequisites

- C++ compiler (g++ or clang)
- CMake 3.15 or later
- LLVM 14 or later
- Git

### Clone and Build

```bash
git clone https://github.com/Arthur-Kamau/stratos.git
cd stratos/interpreter/C++
bash build.sh
```

### Install

```bash
sudo cp build/stratos /usr/local/bin/
```

## IDE Support

### Visual Studio Code

Install the Stratos extension from the VSCode marketplace:

```bash
code --install-extension stratos-lang.stratos
```

Features:
- Syntax highlighting
- Code completion
- Error checking
- Formatting

### Vim/Neovim

Install the Vim plugin:

```bash
git clone https://github.com/stratos-lang/vim-stratos.git ~/.vim/pack/plugins/start/vim-stratos
```

## Environment Variables

Configure Stratos with environment variables:

| Variable | Description | Default |
|----------|-------------|---------|
| `STRATOS_INSTALL_DIR` | Installation directory (install script) | `~/.stratos` |
| `STRATOS_VERSION` | Version to install (install script) | `latest` |
| `STRATOS_CACHE` | Dependency cache | `~/.stratos/cache` |
| `STRATOS_PATH` | Module search path | `./std:./deps` |

Example:

```bash
export STRATOS_HOME=/opt/stratos
export STRATOS_CACHE=~/.cache/stratos
```

## Updating

### Using the Installer

Re-run the installation script to get the latest version:

```bash
curl -sSL https://get.stratos-lang.org | sh
```

### Manual Update

1. Download the latest release
2. Replace the old binary:

```bash
sudo mv stratos /usr/local/bin/stratos
```

## Uninstallation

### Standard Installation

If you used the install script (installs to `~/.stratos`):

```bash
rm -rf ~/.stratos
# Remove PATH entry from your shell config (~/.bashrc, ~/.zshrc, etc.)
```

If you installed to `/usr/local`:

```bash
sudo rm /usr/local/bin/stratos
rm -rf ~/.stratos
```

### Custom Installation

Remove the Stratos binary from wherever you installed it:

```bash
rm ~/bin/stratos
rm -rf ~/.stratos
```

## Troubleshooting

### Command Not Found

If you get "command not found" after installation:

1. Check if the binary is in your PATH:

```bash
echo $PATH
which stratos
```

2. Add the installation directory to your PATH:

```bash
export PATH="/usr/local/bin:$PATH"
```

3. Reload your shell:

```bash
source ~/.bashrc  # or ~/.zshrc
```

### Permission Denied

If you get permission errors when running Stratos:

```bash
chmod +x ~/.stratos/bin/stratos
```

Or if you manually installed to `/usr/local`:

```bash
sudo chmod +x /usr/local/bin/stratos
```

Note: The installation script installs to `~/.stratos` by default and does not require sudo.

### Build Errors

When building from source, ensure you have all dependencies:

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake llvm-14 llvm-14-dev

# macOS
brew install cmake llvm@14
```

## Next Steps

- [Getting Started](/guide/getting-started) - Create your first program
- [Language Basics](/guide/basics) - Learn the fundamentals
- [CLI Reference](/reference/cli) - Command-line tools

::: tip
After installation, try running `stratos --help` to see all available commands.
:::
