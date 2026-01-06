$ErrorActionPreference = "Continue"
& ".\src\interpreter\cpp\build\stratos.exe" run ".\examples\expect\" 2>&1 | Select-Object -First 100
