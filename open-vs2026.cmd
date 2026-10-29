@echo off
setlocal
cd /d "%~dp0"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" goto no_visual_studio
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
if not defined VS_PATH goto no_visual_studio
set "DEVENV_EXE=%VS_PATH%\Common7\IDE\devenv.exe"
if not exist "%DEVENV_EXE%" goto no_visual_studio
start "" "%DEVENV_EXE%" "%~dp0컴그최종프로젝트.sln"
exit /b 0
:no_visual_studio
echo Visual Studio 2026 with Desktop development with C++ is required.
pause
exit /b 1
