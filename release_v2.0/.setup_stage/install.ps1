param([Parameter(Mandatory=$true)][string]$Archive)
$ErrorActionPreference='Stop'
$dest=Join-Path $env:LOCALAPPDATA 'Programs\Terminal'
New-Item -ItemType Directory -Force -Path $dest | Out-Null
Expand-Archive -LiteralPath $Archive -DestinationPath $dest -Force
$ws=New-Object -ComObject WScript.Shell
$shortcut=$ws.CreateShortcut((Join-Path $env:USERPROFILE 'Desktop\Terminal.lnk'))
$shortcut.TargetPath=Join-Path $dest 'Terminal.exe'
$shortcut.WorkingDirectory=$dest
$shortcut.Save()
Start-Process (Join-Path $dest 'Terminal.exe') -WorkingDirectory $dest
