#define MyAppName "Terminal"
#define MyAppVersion "2.0"
#define MyAppPublisher "Terminal"

[Setup]
AppId={{A690E62E-7EAE-42DB-B542-4D449718E064}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={localappdata}\Programs\Terminal
DefaultGroupName=Terminal
UninstallDisplayIcon={app}\Terminal.exe
OutputDir=release_v2.0
OutputBaseFilename=Terminal-Setup
SetupIconFile=app.ico
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64

[Tasks]
Name: "desktopicon"; Description: "Создать ярлык на рабочем столе"; GroupDescription: "Дополнительные значки:"; Flags: unchecked

[Files]
Source: "release_v2.0\windows_install_files\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Terminal"; Filename: "{app}\Terminal.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\Terminal"; Filename: "{app}\Terminal.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\Terminal.exe"; Description: "Запустить Terminal"; WorkingDir: "{app}"; Flags: postinstall nowait skipifsilent
