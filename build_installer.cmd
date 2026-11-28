@echo off
setlocal
pushd "%~dp0"
set "ISCC=%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
if not exist "%ISCC%" set "ISCC=%ProgramFiles%\Inno Setup 6\ISCC.exe"
if not exist "%ISCC%" (
  echo Inno Setup 6 compiler not found.
  echo Install Inno Setup 6, then run this file again.
  popd
  exit /b 1
)
"%ISCC%" "%~dp0installer.iss"
if errorlevel 1 (
  popd
  exit /b %errorlevel%
)
echo Installer created: %~dp0release_v2.0\Terminal-Setup.exe
popd
endlocal
