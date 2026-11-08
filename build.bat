@echo off
setlocal
cd /d "%~dp0"

rem ============================================================
rem  ContactsV3 (C, linked list) - one-command build for MSVC
rem  Usage: double-click build.bat, or run it from any shell.
rem  Requires: VS2026/2022 + "Desktop development with C++" + Windows SDK.
rem
rem  Why not VsDevCmd.bat? This script sets INCLUDE/LIB/PATH directly,
rem  so it needs no registry access and every step is visible.
rem ============================================================

rem ---- 1. Locate the Visual Studio install --------------------
set "VSINSTALL="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
  for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VSINSTALL=%%i"
)
if not defined VSINSTALL set "VSINSTALL=%ProgramFiles%\Microsoft Visual Studio\18\Community"
if not exist "%VSINSTALL%\VC\Tools\MSVC" (
  echo [ERROR] MSVC toolset not found under "%VSINSTALL%".
  echo         Edit VSINSTALL in this script to point to your VS install.
  exit /b 1
)

rem ---- 2. Pick the newest MSVC toolset and Windows SDK --------
set "MSVCVER="
for /f "delims=" %%i in ('dir /b /ad /o-n "%VSINSTALL%\VC\Tools\MSVC" 2^>nul') do (
  if not defined MSVCVER set "MSVCVER=%%i"
)
set "SDKROOT=%ProgramFiles(x86)%\Windows Kits\10"
set "SDKVER="
for /f "delims=" %%i in ('dir /b /ad /o-n "%SDKROOT%\Lib" 2^>nul') do (
  if not defined SDKVER set "SDKVER=%%i"
)
if not defined MSVCVER goto :noenv
if not defined SDKVER goto :noenv

set "VCTOOLS=%VSINSTALL%\VC\Tools\MSVC\%MSVCVER%"
set "PATH=%VCTOOLS%\bin\Hostx64\x64;%PATH%"
set "INCLUDE=%VCTOOLS%\include;%SDKROOT%\Include\%SDKVER%\ucrt;%SDKROOT%\Include\%SDKVER%\um;%SDKROOT%\Include\%SDKVER%\shared;%INCLUDE%"
set "LIB=%VCTOOLS%\lib\x64;%SDKROOT%\Lib\%SDKVER%\ucrt\x64;%SDKROOT%\Lib\%SDKVER%\um\x64;%LIB%"

rem ---- 3. Fallback: if cl is still missing, try VsDevCmd -------
where cl >nul 2>nul
if errorlevel 1 (
  if exist "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" call "%VSINSTALL%\Common7\Tools\VsDevCmd.bat" -arch=x64 -no_logo
)
where cl >nul 2>nul
if errorlevel 1 goto :noenv

rem ---- 4. Compile + link --------------------------------------
echo [ENV] MSVC %MSVCVER%  /  Windows SDK %SDKVER%
echo [BUILD] cl /nologo /W4 main.c contact.c -^> ContactsV3.exe
echo.
cl /nologo /W4 /Fe:ContactsV3.exe main.c contact.c
if errorlevel 1 (
  echo.
  echo [ERROR] Build failed. Read the compiler output above.
  exit /b 1
)

echo.
echo [OK] ContactsV3.exe built in %CD%
echo      Run it with:  ContactsV3.exe
echo      Data file:    contacts.txt  (created in the current directory)
echo.
endlocal
exit /b 0

:noenv
echo [ERROR] Cannot set up the MSVC environment.
echo         Check that "Desktop development with C++" and a Windows 10/11 SDK
echo         are installed in Visual Studio Installer, then run this again.
exit /b 1
