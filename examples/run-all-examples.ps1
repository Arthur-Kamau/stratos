# Run all Stratos examples
# Usage: .\run-all-examples.ps1

$ErrorActionPreference = "Stop"

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$ProjectRoot = Split-Path -Parent $ScriptDir
$StratosBin = Join-Path $ProjectRoot "src\interpreter\cpp\build\stratos.exe"

Write-Host "=== Running All Stratos Examples ===" -ForegroundColor Blue
Write-Host ""

# Check if stratos binary exists
if (-Not (Test-Path $StratosBin)) {
    Write-Host "Error: Stratos binary not found at $StratosBin" -ForegroundColor Red
    Write-Host "Please build the interpreter first."
    exit 1
}

# Counter for statistics
$Total = 0
$Passed = 0
$Failed = 0
$Skipped = 0

# List of examples to skip (if any need to be skipped)
$SkipExamples = @(
    "dependency-test",
    "expect",
    "ffi_c_math",
    "ffi_cpp_string"
)

# Function to check if example should be skipped
function Should-Skip($exampleName) {
    return $SkipExamples -contains $exampleName
}

# Find all example directories (those with src/main.st)
Get-ChildItem -Path $ScriptDir -Directory | ForEach-Object {
    $exampleDir = $_.FullName
    $exampleName = $_.Name

    # Check if this example should be skipped
    if (Should-Skip $exampleName) {
        Write-Host "[SKIP] $exampleName (requires special setup)" -ForegroundColor Yellow
        $script:Skipped++
        $script:Total++
        return
    }

    # Check if it has a main.st file
    $mainFile = Join-Path $exampleDir "src\main.st"
    if (Test-Path $mainFile) {
        $script:Total++
        Write-Host "[RUN] Running example: $exampleName" -ForegroundColor Blue

        # Run the example and capture output
        try {
            & $StratosBin run $mainFile *>&1 | Out-Null
            if ($LASTEXITCODE -eq 0) {
                Write-Host "[PASS] $exampleName" -ForegroundColor Green
                Write-Host ""
                $script:Passed++
            } else {
                Write-Host "[FAIL] $exampleName" -ForegroundColor Red
                Write-Host "  Run manually to see error: $StratosBin run $mainFile"
                Write-Host ""
                $script:Failed++
            }
        } catch {
            Write-Host "[FAIL] $exampleName" -ForegroundColor Red
            Write-Host "  Run manually to see error: $StratosBin run $mainFile"
            Write-Host ""
            $script:Failed++
        }
    }
}

# Print summary
Write-Host "=== Summary ===" -ForegroundColor Blue
Write-Host "Total:   $Total"
Write-Host "Passed:  $Passed" -ForegroundColor Green
if ($Failed -gt 0) {
    Write-Host "Failed:  $Failed" -ForegroundColor Red
} else {
    Write-Host "Failed:  $Failed"
}
if ($Skipped -gt 0) {
    Write-Host "Skipped: $Skipped" -ForegroundColor Yellow
}

# Exit with error code if any tests failed
if ($Failed -gt 0) {
    exit 1
}
