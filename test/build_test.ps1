#!/usr/bin/env pwsh
# Simple test build script for Path-ORAM

Write-Host "Building Path-ORAM Tests..." -ForegroundColor Cyan

# Source files
$sourceFiles = @(
    "../src/Tree.cpp",
    "../src/Forest.cpp", 
    "../src/rgen.cpp",
    "test.cpp"
)

# Compiler flags
$cxxFlags = "-std=c++17", "-Wall", "-Wextra", "-g"

# Build test program
Write-Host "Compiling test program..." -ForegroundColor Gray
$buildArgs = $cxxFlags + @("-o", "test_runner") + $sourceFiles
& g++ @buildArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host "Test build successful!" -ForegroundColor Green
    Write-Host "Run tests with: .\test_runner.exe" -ForegroundColor Yellow
} else {
    Write-Host "❌ Test build failed!" -ForegroundColor Red
    exit 1
}