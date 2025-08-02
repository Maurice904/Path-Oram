#!/usr/bin/env pwsh
# Build script for Path-ORAM

Write-Host "Building Path-ORAM..." -ForegroundColor Green

# Compile the program
g++ -std=c++17 -Wall -Wextra -g -o path_oram Tree.cpp Forest.cpp path_oram.cpp rgen.cpp

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful!" -ForegroundColor Green
    Write-Host "Run with: .\path_oram.exe" -ForegroundColor Yellow
} else {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}