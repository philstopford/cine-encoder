; Modern Inno Setup Script for Cine Encoder with Qt6 and SVG support
; This script is designed to work with GitHub Actions deployment

#define MyAppName "Cine Encoder 2023"
#define MyAppVersion "3.5.5"
#define MyAppPublisher "Oleg Kozhukharenko"
#define MyAppURL "https://github.com/philstopford/cine-encoder"
#define MyAppExeName "cine_encoder.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
AppId={{F261F997-18A7-4DEE-B351-F942FB21DFF3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableWelcomePage=no
DisableProgramGroupPage=yes
OutputDir=.
OutputBaseFilename=cine-encoder-{#MyAppVersion}-windows-x64
SetupIconFile=app\cine-encoder.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern
MinVersion=6.1sp1
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 6.1

[Files]
; Main application and all deployed Qt6 dependencies (including SVG support)
Source: "deploy\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

; License file
Source: "LICENSE"; DestDir: "{app}"; Flags: ignoreversion

; Documentation
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

[Registry]
; Register file associations for video files
Root: HKCR; Subkey: ".mp4"; ValueType: string; ValueName: ""; ValueData: "CineEncoder.VideoFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".avi"; ValueType: string; ValueName: ""; ValueData: "CineEncoder.VideoFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mkv"; ValueType: string; ValueName: ""; ValueData: "CineEncoder.VideoFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".mov"; ValueType: string; ValueName: ""; ValueData: "CineEncoder.VideoFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "CineEncoder.VideoFile"; ValueType: string; ValueName: ""; ValueData: "Video File"; Flags: uninsdeletekey
Root: HKCR; Subkey: "CineEncoder.VideoFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"
Root: HKCR; Subkey: "CineEncoder.VideoFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
  // Check if we're running on a 64-bit system
  if not Is64BitInstallMode then begin
    MsgBox('This application requires a 64-bit version of Windows.', mbError, MB_OK);
    Result := False;
  end;
end;