
#include AddBackslash(SourcePath) + "..\src\common\version.h"

#define APP_EXE_NAME	"FortFirewall.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppMutex={#APP_NAME}
AppName={#APP_NAME}
AppVersion={#APP_VERSION_STR}
VersionInfoVersion={#APP_VERSION_STR}
AppVerName={#APP_NAME}
AppCopyright={#APP_LEGALCOPYRIGHT}
AppPublisher={#APP_PUBLISHER}
AppPublisherURL={#APP_URL}
AppSupportURL={#APP_URL}
AppUpdatesURL={#APP_UPDATES_URL}
DefaultGroupName={#APP_NAME}
DefaultDirName={pf32}\{#APP_NAME}
AlwaysShowDirOnReadyPage=yes
AlwaysShowGroupOnReadyPage=yes
AllowNoIcons=yes
OutputBaseFilename=FortFirewall-{#APP_VERSION_STR}
Uninstallable=not IsTaskSelected('portable')
UninstallFilesDir={app}\uninst
ArchitecturesInstallIn64BitMode=x64
Compression=lzma2/ultra
SolidCompression=yes

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"
Name: ru; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; Flags: unchecked
Name: "portable"; Description: "Portable"; Flags: unchecked

[Files]
Source: "build\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "README.portable"; DestDir: "{app}"; Check: IsTaskSelected('portable')

[Icons]
; Start menu shortcut
Name: "{group}\{#APP_NAME}"; Filename: "{app}\{#APP_EXE_NAME}"
; Uninstaller shortcut
Name: "{group}\{cm:UninstallProgram,{#APP_NAME}}"; Filename: "{uninstallexe}"
; Desktop shortcut
Name: "{commondesktop}\{#APP_NAME}"; Filename: "{app}\{#APP_EXE_NAME}"; WorkingDir: "{app}"; Tasks: desktopicon

[Run]
Filename: "{app}\driver\scripts\reinstall.bat"; Description: "Re-install driver"; Flags: runascurrentuser

[UninstallRun]
Filename: "{app}\driver\scripts\uninstall.bat"; Flags: runascurrentuser
Filename: "{app}\{#APP_EXE_NAME}"; Parameters: "-b=0"; Flags: runascurrentuser

[InstallDelete]
Type: filesandordirs; Name: "{app}"

[UninstallDelete]
Type: filesandordirs; Name: "{app}"
