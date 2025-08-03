#!/usr/bin/env pwsh
# Build script for Path-ORAM

Write-Host "Building Path-ORAM..." -ForegroundColor Green

# Clean previous build
if (Test-Path "path_oram.exe") {
    Write-Host "Cleaning previous build..." -ForegroundColor Cyan
    Remove-Item "path_oram.exe" -Force
}

# Compile the program with all source files
Write-Host "Compiling source files..." -ForegroundColor Cyan
g++ -std=c++17 -Wall -Wextra -O2 -g -o path_oram Tree.cpp Forest.cpp StandardPathORAM.cpp showInformation.cpp Plot.cpp path_oram.cpp

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
    Write-Host "Executable: path_oram.exe" -ForegroundColor Yellow
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    Write-Host "Please check for compilation errors above." -ForegroundColor Red
    exit 1
}