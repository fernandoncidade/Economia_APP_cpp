#define MyAppName "Economia_APP"
#define MyAppVersion "2026.10.7.0"
#define MyAppPublisher "Fernando Nillsson Cidade"
#define MyAppURL "https://github.com/fernandoncidade"
#define MyAppExeName "Economia_APP.exe"
#define AppDistDir "D:\MISCELANEAS\VSCode\Economia_APP\Economia_APP_cpp\Economia_APP_cpp_mingw"
#define InstallerOutputDir "D:\MISCELANEAS\VSCode\Economia_APP\InnoSetupOutput"

[Setup]
AppId={{FC31EF66-6D22-48BC-9D32-7648A3AA253B}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
UninstallDisplayName={#MyAppName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DisableDirPage=no
UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
DisableProgramGroupPage=yes
InfoBeforeFile={#AppDistDir}\assets\ABOUT\ABOUT_en_US.txt
LicenseFile={#AppDistDir}\assets\EULA\EULA_en_US - Economia.txt
OutputDir={#InstallerOutputDir}
OutputBaseFilename=Economia_APP_mingw_v2026.10.7.0
SetupIconFile={#AppDistDir}\assets\icones\economia.ico
SolidCompression=yes
WizardStyle=modern
ShowLanguageDialog=yes
AllowNoIcons=yes
DisableReadyPage=yes
DisableFinishedPage=no

[Languages]
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"; InfoBeforeFile: "{#AppDistDir}\assets\ABOUT\ABOUT_pt_BR.txt"; LicenseFile: "{#AppDistDir}\assets\EULA\EULA_pt_BR - Economia.txt"
Name: "english"; MessagesFile: "compiler:Default.isl"; InfoBeforeFile: "{#AppDistDir}\assets\ABOUT\ABOUT_en_US.txt"; LicenseFile: "{#AppDistDir}\assets\EULA\EULA_en_US - Economia.txt"

[CustomMessages]
brazilianportuguese.AppLanguage=Idioma do aplicativo
brazilianportuguese.SelectAppLang=Selecione o idioma padrão do aplicativo
brazilianportuguese.Portuguese=Português
brazilianportuguese.English=Inglês
brazilianportuguese.PrivacyPolicyTitle=Política de Privacidade
brazilianportuguese.PrivacyPolicyDescription=Leia a Política de Privacidade antes de continuar.
brazilianportuguese.PrivacyPolicySubCaption=Role o texto até o final para liberar o botão Avançar.
brazilianportuguese.PrivacyPolicyLoadError=Não foi possível carregar a Política de Privacidade.

english.AppLanguage=Application language
english.SelectAppLang=Select the default application language
english.Portuguese=Portuguese
english.English=English
english.PrivacyPolicyTitle=Privacy Policy
english.PrivacyPolicyDescription=Please read the Privacy Policy before continuing.
english.PrivacyPolicySubCaption=Scroll to the end of the text to enable Next.
english.PrivacyPolicyLoadError=Unable to load the Privacy Policy.

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "langpt_BR"; Description: "{cm:Portuguese}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive
Name: "langen_US"; Description: "{cm:English}"; GroupDescription: "{cm:AppLanguage}"; Flags: exclusive unchecked

[Files]
Source: "{#AppDistDir}\assets\PRIVACY_POLICY\Privacy_Policy_en_US.txt"; DestName: "Installer_Privacy_Policy_en_US.txt"; Flags: dontcopy noencryption
Source: "{#AppDistDir}\assets\PRIVACY_POLICY\Privacy_Policy_pt_BR.txt"; DestName: "Installer_Privacy_Policy_pt_BR.txt"; Flags: dontcopy noencryption
Source: "{#AppDistDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{autoprograms}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon; WorkingDir: "{app}"

[Run]
Filename: "{sys}\icacls.exe"; Parameters: """{app}"" /grant *S-1-5-32-545:(OI)(CI)F"; Flags: runhidden; Description: "Configurando permissões..."
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent; WorkingDir: "{app}"

[ReturnCodes]
6000=UserCancelled
6001=AppAlreadyExists
6002=AnotherInstallationRunning
6003=DiskSpaceFull
6004=RebootRequired
6005=NetworkFailure_DownloadError
6006=NetworkFailure_ConnectionLost
6007=PackageRejectedByPolicy
0=Success
