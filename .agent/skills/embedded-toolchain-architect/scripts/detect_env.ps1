# Auto-detect embedded development tools on Windows
Write-Host "=== EMBEDDED TOOLCHAIN DETECTOR (WINDOWS) ===" -ForegroundColor Cyan

function Find-Tool($name, $hints) {
    $cmd = Get-Command $name -ErrorAction SilentlyContinue
    if ($cmd) {
        Write-Host "[OK] $name found in PATH: $($cmd.Source)" -ForegroundColor Green
        return $cmd.Source
    }
    foreach ($hint in $hints) {
        if (Test-Path $hint) {
            Write-Host "[OK] $name found at: $hint" -ForegroundColor Green
            return $hint
        }
    }
    Write-Host "[WARN] $name NOT FOUND" -ForegroundColor Yellow
    return $null
}

Write-Host "`n--- Cross-Compilers (ARM MCU Target) ---" -ForegroundColor White
$gccHints = @(Get-ChildItem -Path "$env:LOCALAPPDATA\stm32cube\bundles\gnu-tools-for-stm32\*\bin\arm-none-eabi-gcc.exe" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName)
Find-Tool "arm-none-eabi-gcc.exe" $gccHints

Write-Host "`n--- Host Compilers (PC Unit Tests) ---" -ForegroundColor White
$hostGccHints = @("$env:SystemDrive\msys64\ucrt64\bin\gcc.exe", "$env:SystemDrive\msys64\mingw64\bin\gcc.exe")
$hostGcc = Find-Tool "gcc.exe" $hostGccHints
$hostGpp = Find-Tool "g++.exe" @()
$hostCl  = Find-Tool "cl.exe" @()
if (-not $hostGcc -and -not $hostGpp -and -not $hostCl) {
    Write-Host "[ACTION REQUIRED] No Host C/C++ compiler found." -ForegroundColor Red
    Write-Host "  Install via: winget install -e --id BrechtSanders.WinLibs.POSIX.UCRT" -ForegroundColor Yellow
}

Write-Host "`n--- Build Tools ---" -ForegroundColor White
$cmakeHints = @(Get-ChildItem -Path "$env:LOCALAPPDATA\stm32cube\bundles\cmake\*\bin\cmake.exe" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName)
Find-Tool "cmake.exe" $cmakeHints
$ninjaHints = @(Get-ChildItem -Path "$env:LOCALAPPDATA\stm32cube\bundles\ninja\*\bin\ninja.exe" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName)
Find-Tool "ninja.exe" $ninjaHints

Write-Host "`n--- Debug Probes ---" -ForegroundColor White
$stlinkGdbHints = @(Get-ChildItem -Path "$env:SystemDrive\ST\STM32CubeIDE*\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server*\tools\bin\ST-LINK_gdbserver.exe" -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName)
Find-Tool "ST-LINK_gdbserver.exe" $stlinkGdbHints

Write-Host "`n--- Coverage Tools ---" -ForegroundColor White
Find-Tool "gcov.exe" @()
Find-Tool "lcov" @()

Write-Host "`n==============================================" -ForegroundColor Cyan

