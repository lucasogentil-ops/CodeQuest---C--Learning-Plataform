param([string]$ToolchainRoot = 'C:\msys64\ucrt64')
$ErrorActionPreference = 'Stop'
$project = Split-Path -Parent $PSScriptRoot
& "$PSScriptRoot/build.ps1" -ToolchainRoot $ToolchainRoot

$package = Join-Path $project 'dist/CodeQuest'
$licences = Join-Path $package 'licenses'
New-Item -ItemType Directory -Force $licences | Out-Null
foreach ($entry in 'codequest.exe', 'assets', 'content') {
    Copy-Item -LiteralPath (Join-Path "$project/build" $entry) -Destination $package -Recurse -Force
}
foreach ($dll in 'libraylib.dll', 'glfw3.dll', 'libcjson-1.dll') {
    Copy-Item -LiteralPath (Join-Path "$ToolchainRoot/bin" $dll) -Destination $package -Force
}
foreach ($dependency in 'raylib', 'glfw', 'cjson') {
    Copy-Item -LiteralPath (Join-Path "$ToolchainRoot/share/licenses" $dependency) -Destination $licences -Recurse -Force
}
@'
CodeQuest for Windows

Extract the entire ZIP and open codequest.exe from the CodeQuest folder.
Keep the assets, content and DLL files together with the executable.
Choose a topic to read its lesson and try its practice question.
Open My progress to see your badges. The app saves progress locally.
Press Escape or close the window to exit.
'@ | Set-Content -LiteralPath "$package/START HERE.txt" -Encoding utf8

Compress-Archive -LiteralPath $package -DestinationPath "$project/dist/CodeQuest-Windows-x64.zip" -Force
Write-Output "Package created at $project/dist/CodeQuest-Windows-x64.zip"
