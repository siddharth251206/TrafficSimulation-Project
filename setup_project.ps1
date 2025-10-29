# ==============================
# 🚦 Traffic Simulation Setup Script
# ==============================

Write-Host "🔧 Setting up project folders..." -ForegroundColor Cyan

# Ensure folders exist
$folders = @("src", "cmake", "assets", "test", "build")
foreach ($folder in $folders) {
    if (-not (Test-Path $folder)) {
        New-Item -ItemType Directory -Path $folder | Out-Null
        Write-Host "✅ Created folder: $folder"
    }
}

# Move C++ source and header files to src/
Write-Host "📦 Moving source files into src/..."
Get-ChildItem -Path . -Include *.cpp, *.hpp -File | Move-Item -Destination "src" -Force -ErrorAction SilentlyContinue

# Move cmake helper scripts (if any) to cmake/
Write-Host "⚙️ Moving cmake files..."
$cmakeFiles = @("dependencies.cmake", "project-options.cmake", "project-targets.cmake", "vcpkg-root-finder.cmake")
foreach ($file in $cmakeFiles) {
    if (Test-Path $file) {
        Move-Item $file "cmake\" -Force -ErrorAction SilentlyContinue
        Write-Host "✅ Moved: $file"
    }
}

# Clean up old build folder if needed
if (Test-Path "build") {
    Write-Host "🧹 Cleaning old build directory..."
    Remove-Item -Recurse -Force "build"
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# Run CMake + Ninja build
Write-Host "🚀 Configuring with CMake..."
cd build

cmake .. -G "Ninja" `
    -DCMAKE_TOOLCHAIN_FILE="C:/Users/balod/vcpkg/scripts/buildsystems/vcpkg.cmake" `
    -DCMAKE_BUILD_TYPE=Release

Write-Host "🏗️ Building project with Ninja..."
ninja

# Run the executable if it was built successfully
$exePath = Join-Path (Get-Location) "src\TrafficSimulation.exe"
if (Test-Path $exePath) {
    Write-Host "🎮 Running Traffic Simulation..."
    & $exePath
} else {
    Write-Host "❌ Build finished, but executable not found. Check for CMake errors." -ForegroundColor Red
}

Write-Host "`n✅ Setup complete!"
