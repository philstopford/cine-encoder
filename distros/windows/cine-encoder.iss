; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Cine Encoder 2021"
#define MyAppVersion "3.5"
#define MyAppPublisher "Oleg Kozhukharenko"
#define MyAppURL "https://github.com/CineEncoder/cine-encoder"
#define MyAppExeName "cine_encoder.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{F261F997-18A7-4DEE-B351-F942FB21DFF3}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableWelcomePage=no
WizardImageFile=D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\welcome.bmp
DisableProgramGroupPage=yes
LicenseFile=D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\LICENSE.txt
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate
OutputBaseFilename=setup
SetupIconFile=D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\cine-encoder.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\cine-encoder.ico"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\cine-encoder.wav"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\ffmpeg.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\libgcc_s_seh-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\libstdc++-6.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\libwinpthread-1.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\MediaInfo.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\MediaInfo_InfoTip.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\MediaInfo_InfoTip_Register.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\MediaInfo_InfoTip_UnRegister.bat"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\mkvpropedit.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\Qt5Multimedia.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\plugins\audio\*"; DestDir: "{app}\plugins\audio"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\plugins\mediaservice\*"; DestDir: "{app}\plugins\mediaservice"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "D:\QtCreatorProjects\CineEncoder_WindowsPackageTemplate\cine_encoder_win10_x86_64\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

