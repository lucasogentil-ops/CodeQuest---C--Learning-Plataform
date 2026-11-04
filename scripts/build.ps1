param([string]$ToolchainRoot = 'C:\msys64\ucrt64')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$toolBin = Join-Path $ToolchainRoot 'bin'
foreach ($tool in @('gcc.exe', 'cmake.exe', 'ninja.exe')) {
    if (!(Test-Path -LiteralPath (Join-Path $toolBin $tool))) {
        throw "Missing $tool in $toolBin. Install the MSYS2 UCRT64 toolchain or pass -ToolchainRoot."
    }
}
$previousPath = $env:PATH
try {
    $env:PATH = "$toolBin;$env:PATH"
    & "$toolBin\cmake.exe" -S $projectRoot -B "$projectRoot\build" -G Ninja "-DCMAKE_C_COMPILER=$toolBin/gcc.exe" "-DCMAKE_MAKE_PROGRAM=$toolBin/ninja.exe" "-DCMAKE_PREFIX_PATH=$ToolchainRoot" -DCMAKE_BUILD_TYPE=Debug
    if ($LASTEXITCODE -ne 0) { throw 'CMake configuration failed.' }
    & "$toolBin\cmake.exe" --build "$projectRoot\build"
    if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
} finally {
    $env:PATH = $previousPath
}
