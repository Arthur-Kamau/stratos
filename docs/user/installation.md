# Stratos Installation System Setup

This document describes how the Stratos installation system works and how to set it up.

## Overview

The installation system allows users to install Stratos with a single command:

```bash
curl -sSL https://get.stratos-lang.org | sh
```

This system consists of three main components:

1. **install.sh** - The installation script
2. **GitHub Actions Release Workflow** - Automated binary builds
3. **Domain Hosting** - Serving the install script

## Components

### 1. Install Script (`install.sh`)

The `install.sh` script is a comprehensive bash installer that:

- **Auto-detects platform**: Linux, macOS, Windows (MSYS/MinGW)
- **Auto-detects architecture**: x86_64, aarch64, armv7
- **Downloads pre-built binaries** from GitHub Releases
- **Falls back to source build** if binaries are unavailable
- **Configures PATH automatically** by updating shell RC files (.bashrc, .zshrc, .config/fish/config.fish)
- **Provides verification** and next steps

**Key features**:
- Colored terminal output for better UX
- Error handling with cleanup on failure
- Support for custom installation directory via `STRATOS_INSTALL_DIR`
- Support for specific version via `STRATOS_VERSION` environment variable

**Usage examples**:
```bash
# Install latest version to default location (~/.stratos)
curl -sSL https://get.stratos-lang.org | sh

# Install to custom location
curl -sSL https://get.stratos-lang.org | STRATOS_INSTALL_DIR=/usr/local sh

# Install specific version
curl -sSL https://get.stratos-lang.org | STRATOS_VERSION=v0.2.0 sh
```

### 2. GitHub Actions Release Workflow

The `.github/workflows/release.yml` workflow automates the release process:

**Triggers**:
- Version tags matching `v*` pattern (e.g., v0.1.0, v1.0.0)
- Manual trigger via `workflow_dispatch`

**Build Matrix**:
- Linux x86_64 (ubuntu-latest)
- macOS x86_64 (macos-latest)
- macOS ARM64/aarch64 (macos-latest with cross-compile)

**Process**:
1. Checks out code
2. Sets up build tools (g++, cmake, etc.)
3. Runs `interpreter/C++/build.sh`
4. Verifies build output
5. Creates tar.gz archives
6. Uploads binaries to GitHub Release
7. Deploys install script to GitHub Pages (gh-pages branch)

**Artifacts produced**:
- `stratos-linux-x86_64` (binary)
- `stratos-linux-x86_64.tar.gz` (archive)
- `stratos-macos-x86_64` (binary)
- `stratos-macos-x86_64.tar.gz` (archive)
- `stratos-macos-aarch64` (binary)
- `stratos-macos-aarch64.tar.gz` (archive)

### 3. Domain Hosting Options

To make `https://get.stratos-lang.org` work, you need to host the install script. Here are three options:

#### Option A: GitHub Pages (Recommended - Free)

The workflow already deploys to GitHub Pages. You just need to configure the domain:

1. **Enable GitHub Pages**:
   - Go to repository Settings > Pages
   - Source: Deploy from branch `gh-pages`
   - Directory: `/` (root)

2. **Configure Custom Domain**:
   - In Settings > Pages > Custom domain, enter: `get.stratos-lang.org`
   - This creates a `CNAME` file in gh-pages branch

3. **DNS Configuration**:
   Add these DNS records at your domain registrar:
   ```
   Type: CNAME
   Name: get
   Value: <your-github-username>.github.io
   TTL: 3600
   ```

4. **Verify**:
   ```bash
   curl -sSL https://get.stratos-lang.org/install.sh
   ```

**Pros**: Free, automatic deployment, GitHub CDN
**Cons**: Requires domain ownership, DNS propagation delay

#### Option B: Cloudflare Workers (Alternative)

Create a Cloudflare Worker to redirect to the raw GitHub file:

```javascript
addEventListener('fetch', event => {
  event.respondWith(handleRequest(event.request))
})

async function handleRequest(request) {
  const installScript = 'https://raw.githubusercontent.com/anthropics/stratos/master/install.sh'
  return fetch(installScript)
}
```

**Pros**: Fast, globally distributed, free tier available
**Cons**: Requires Cloudflare account

#### Option C: Custom Server (Nginx/Apache)

Host on your own server:

```nginx
# Nginx config
server {
    listen 443 ssl;
    server_name get.stratos-lang.org;

    ssl_certificate /path/to/cert.pem;
    ssl_certificate_key /path/to/key.pem;

    location / {
        proxy_pass https://raw.githubusercontent.com/anthropics/stratos/master/install.sh;
    }
}
```

**Pros**: Full control
**Cons**: Requires server maintenance, SSL certificate management

## Setup Instructions

### Step 1: Update Repository Reference

Before creating releases, update the GitHub repository reference in `install.sh`:

```bash
# Line 18 in install.sh
GITHUB_REPO="anthropics/stratos"  # Change to your actual repo (e.g., "yourusername/stratos")
```

### Step 2: Create First Release

1. **Commit your changes**:
   ```bash
   git add install.sh .github/workflows/release.yml
   git commit -m "Add installation system"
   git push origin master
   ```

2. **Create and push a version tag**:
   ```bash
   git tag v0.1.0
   git push origin v0.1.0
   ```

3. **Monitor the workflow**:
   - Go to Actions tab in GitHub
   - Watch the "Release" workflow execute
   - Verify all matrix jobs complete successfully

4. **Verify the release**:
   - Go to Releases tab
   - You should see v0.1.0 with 6 assets (3 binaries + 3 tar.gz files)

### Step 3: Configure Domain (Choose One Option)

Follow one of the hosting options above (A, B, or C) to configure `get.stratos-lang.org`.

### Step 4: Test Installation

Test the installation from a fresh environment:

```bash
# Test with curl
curl -sSL https://get.stratos-lang.org | sh

# Verify installation
stratos --version

# Test a simple program
echo 'fn main() { println("Hello from Stratos!"); }' > test.st
stratos run test.st
```

Test on multiple platforms:
- Linux (Ubuntu, Debian, Fedora, Arch)
- macOS (Intel and M1/M2)
- Windows (via Git Bash or MSYS2)

## Troubleshooting

### Release workflow fails to build

**Symptoms**: GitHub Actions workflow fails at "Build Stratos" step

**Solutions**:
1. Check if `interpreter/C++/build.sh` exists and is executable
2. Verify build.sh works locally: `cd interpreter/C++ && bash build.sh`
3. Check build dependencies in workflow (g++, cmake versions)

### Binary download fails, falls back to source build

**Symptoms**: Install script shows "Failed to download binary" and builds from source

**Possible causes**:
1. Release doesn't exist for the version
2. Asset naming mismatch (check `asset_name` in workflow vs. download URL in install.sh)
3. GitHub Release is marked as draft or pre-release

**Solutions**:
1. Verify release exists: `https://github.com/yourusername/stratos/releases`
2. Check asset names match pattern: `stratos-{platform}-{arch}`
3. Publish the release if it's in draft state

### Domain doesn't serve install script

**Symptoms**: `curl https://get.stratos-lang.org` returns 404 or SSL error

**Solutions**:
1. Verify DNS propagation: `dig get.stratos-lang.org`
2. Check GitHub Pages is enabled and deploying from gh-pages branch
3. Verify CNAME file exists in gh-pages branch
4. Wait for DNS propagation (can take up to 48 hours)
5. Try without SSL first: `curl http://get.stratos-lang.org`

### Installation fails on specific platform

**Symptoms**: Script exits with "Unsupported operating system" or "Unsupported architecture"

**Solutions**:
1. Check platform detection logic in `detect_platform()` function
2. Add support for the platform if it's a valid target
3. Ensure GitHub workflow builds for that platform

### PATH not updated after installation

**Symptoms**: `stratos` command not found after installation

**Solutions**:
1. Source the shell config: `source ~/.bashrc` (or ~/.zshrc)
2. Restart terminal
3. Manually add to PATH: `export PATH="$HOME/.stratos/bin:$PATH"`
4. Check if script detected correct shell config file

## Testing Locally

Before pushing changes, test the installation script locally:

### Test binary download simulation

```bash
# Simulate a local "release"
cd interpreter/C++
bash build.sh
mkdir -p /tmp/fake-release
cp build/stratos /tmp/fake-release/stratos-linux-x86_64

# Modify install.sh temporarily to use local URL
# Change download_url to: file:///tmp/fake-release/stratos-linux-x86_64

# Run installer
bash install.sh
```

### Test source build fallback

```bash
# Force source build by providing invalid download URL
STRATOS_VERSION="nonexistent" bash install.sh
```

### Test PATH configuration

```bash
# Check if PATH was updated
grep "stratos" ~/.bashrc  # or ~/.zshrc

# Verify binary location
ls -lh ~/.stratos/bin/stratos

# Test execution
~/.stratos/bin/stratos --version
```

## Maintenance

### Creating New Releases

1. Update version numbers in code/docs
2. Update CHANGELOG.md
3. Commit changes
4. Create and push tag:
   ```bash
   git tag v0.2.0
   git push origin v0.2.0
   ```
5. Workflow automatically builds and creates release
6. Write release notes in GitHub Releases UI

### Updating Install Script

When modifying install.sh:

1. Test locally first (see "Testing Locally" above)
2. Update version/commit hash if needed
3. Push changes to master
4. Changes automatically deploy on next release
5. For urgent fixes, manually update gh-pages branch:
   ```bash
   git checkout gh-pages
   git checkout master -- install.sh
   git commit -m "Update install script"
   git push origin gh-pages
   ```

### Monitoring

Set up monitoring for:
- Installation success rate (via GitHub download counts)
- Platform distribution (which platforms users are on)
- Common error messages (via GitHub issues)
- Website uptime (if using custom domain)

## Security Considerations

1. **Script Integrity**: Users should be able to inspect the script before running
   - Provide option to download and review: `curl -sSL https://get.stratos-lang.org > install.sh`
   - Include checksum verification in future versions

2. **HTTPS Only**: Always use HTTPS to prevent MITM attacks
   - Install script enforces HTTPS for downloads
   - Domain should have valid SSL certificate

3. **Signature Verification**: Consider GPG signing releases in the future
   ```bash
   # Future enhancement
   gpg --verify stratos-linux-x86_64.sig stratos-linux-x86_64
   ```

4. **Minimal Permissions**: Script only modifies:
   - `~/.stratos` directory
   - User's shell RC file (for PATH)
   - Never requires sudo for default installation

## Future Enhancements

1. **Windows Native Support**: Build .exe binaries for Windows
2. **Package Managers**: Submit to Homebrew, apt, yum repositories
3. **Checksums**: Include SHA256 checksums in releases
4. **GPG Signatures**: Sign releases with GPG key
5. **Version Manager**: Allow multiple versions installed side-by-side
6. **Uninstall Script**: Provide clean uninstallation
7. **Update Command**: `stratos update` to upgrade to latest version
8. **Telemetry**: Optional anonymous usage statistics

## Resources

- GitHub Actions Docs: https://docs.github.com/en/actions
- GitHub Pages Docs: https://docs.github.com/en/pages
- Bash Scripting Guide: https://www.gnu.org/software/bash/manual/
- Semantic Versioning: https://semver.org/

## Support

If you encounter issues with the installation system:

1. Check this documentation for troubleshooting steps
2. Search existing issues: https://github.com/yourusername/stratos/issues
3. Create a new issue with:
   - Operating system and version
   - Architecture (x86_64, aarch64, etc.)
   - Full error message
   - Output of: `bash -x install.sh` (debug mode)
