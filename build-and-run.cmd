@echo off
setlocal
cd /d "%~dp0"
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
)
if not defined VS_PATH (
    echo Visual Studio 2026 with Desktop development with C++ is required.
    pause
    exit /b 1
)
set "MSBUILD_EXE=%VS_PATH%\MSBuild\Current\Bin\MSBuild.exe"
"%MSBUILD_EXE%" "컴그최종프로젝트.sln" /t:Build /p:Configuration=Release /p:Platform=x64 /m
if errorlevel 1 goto failed
pushd "bin\x64\Release"
컴그최종프로젝트.exe
set "RESULT=%ERRORLEVEL%"
popd
if not "%RESULT%"=="0" goto failed
exit /b 0
:failed
echo Visual Studio build or execution failed. Check the error message above.
pause
exit /b 1
