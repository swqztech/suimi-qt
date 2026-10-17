$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent (Split-Path -Parent $MyInvocation.MyCommand.Path)
$Packaging = Join-Path $Root "packaging"
$Stage = Join-Path $Packaging "stage"
$Dist = Join-Path $Root "dist"
$InstallerMode = if ($env:INSTALLER) { $env:INSTALLER.ToLower() } else { "both" }

$cm = Get-Content (Join-Path $Root "CMakeLists.txt") -Raw
if ($cm -notmatch 'VERSION\s+([0-9.]+)') { throw "version not found" }
$Version = $Matches[1]

function Find-Cmake {
    $cmd = Get-Command cmake -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    "$env:LOCALAPPDATA\Microsoft\WinGet\Packages\Kitware.CMake_Microsoft.Winget.Source_8wekyb3d8bbwe\cmake-4.4.4-windows-x86_64\bin\cmake.exe"
}

function Find-QtPrefix {
    if ($env:CMAKE_PREFIX_PATH -and (Test-Path $env:CMAKE_PREFIX_PATH)) {
        return $env:CMAKE_PREFIX_PATH
    }
    $roots = @("C:\Qt")
    foreach ($r in $roots) {
        if (-not (Test-Path $r)) { continue }
        $found = Get-ChildItem -Path $r -Directory -ErrorAction SilentlyContinue |
            ForEach-Object { Get-ChildItem -Path $_.FullName -Filter "msvc*_64" -Directory -ErrorAction SilentlyContinue } |
            Sort-Object FullName -Descending | Select-Object -First 1
        if ($found) { return $found.FullName }
    }
    return $null
}

$cmake = Find-Cmake
$qt = Find-QtPrefix
if (-not $qt) { throw "Set CMAKE_PREFIX_PATH to Qt6 msvc_64 folder, e.g. C:\Qt\6.8.0\msvc2022_64" }

Write-Host "Qt: $qt"
Push-Location $Root
& $cmake -B build "-DCMAKE_PREFIX_PATH=$qt"
& $cmake --build build --config Release
Pop-Location

$exeSrc = Join-Path $Root "build\Release\suimi-qt.exe"
if (-not (Test-Path $exeSrc)) { throw "build failed: $exeSrc" }

$portableName = "随密-Qt-$Version-win64-绿色版"
$portableDir = Join-Path $Dist $portableName
if (Test-Path $Dist) { Remove-Item $Dist -Recurse -Force }
New-Item -ItemType Directory -Path $portableDir -Force | Out-Null
Copy-Item $exeSrc (Join-Path $portableDir "随密-Qt.exe") -Force
Copy-Item (Join-Path $Stage "启动.bat") $portableDir -Force
Copy-Item (Join-Path $Stage "使用说明.txt") $portableDir -Force

$windeploy = Join-Path $qt "bin\windeployqt.exe"
if (-not (Test-Path $windeploy)) { throw "windeployqt not found under $qt" }
& $windeploy --no-compiler-runtime --no-translations (Join-Path $portableDir "随密-Qt.exe")

$zipPath = Join-Path $Dist "$portableName.zip"
Push-Location $Dist
tar -a -cf $zipPath (Split-Path $portableDir -Leaf)
Pop-Location
Write-Host "ZIP: $zipPath"

$installerStage = Join-Path $Packaging "installer-stage"
if (Test-Path $installerStage) { Remove-Item $installerStage -Recurse -Force }
New-Item -ItemType Directory -Path $installerStage -Force | Out-Null
Copy-Item (Join-Path $portableDir "*") $installerStage -Recurse -Force

$setupExe = Join-Path $Dist "随密-Qt-$Version-win64-setup.exe"
$makensis = "${env:ProgramFiles(x86)}\NSIS\makensis.exe"
if ((Test-Path $makensis) -and $InstallerMode -in @("nsis", "both")) {
    $setupAscii = Join-Path $Dist "Suimi-Qt-$Version-win64-setup.exe"
    $nsi = Join-Path $Packaging "nsis\setup.nsi"
    & $makensis "/INPUTCHARSET" "UTF8" "/DVERSION=$Version" "/DSTAGE_DIR=$installerStage" "/DOUTFILE=$setupAscii" $nsi
    Move-Item -Force $setupAscii $setupExe
}
Write-Host "Done: $Dist"
