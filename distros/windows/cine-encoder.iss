; Build deploy\ with build-windows.bat, then compile this script with Inno Setup.
#define MyAppName "Cine Encoder"
#define MyAppVersion "3.5.5"
#define MyAppExeName "cine_encoder.exe"

[Setup]
AppId={{F261F997-18A7-4DEE-B351-F942FB21DFF3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={autopf}\{#MyAppName}
OutputDir=.
OutputBaseFilename=cine-encoder-{#MyAppVersion}-windows-x64
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "Launch {#MyAppName}"; Flags: nowait postinstall skipifsilent
