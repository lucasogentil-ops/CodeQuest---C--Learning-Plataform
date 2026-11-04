param([string]$ToolchainRoot = 'C:\msys64\ucrt64')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$executable = Join-Path $projectRoot 'build\codequest.exe'
if (!(Test-Path -LiteralPath $executable)) { throw 'Build the project first with scripts/build.ps1.' }
$previousPath = $env:PATH
try {
    $env:PATH = "$(Join-Path $ToolchainRoot 'bin');$env:PATH"
    & $executable
    if ($LASTEXITCODE -ne 0) { throw "CodeQuest exited with code $LASTEXITCODE." }
} finally {
    $env:PATH = $previousPath
}
