@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0install.ps1" -Archive "%~dp0Terminal-v2.0-Windows-x64.zip"
exit /b %errorlevel%
