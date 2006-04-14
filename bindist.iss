; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=lbDMF Binary Samples
AppVerName=lbDMF Binary Samples 0.7.1
OutputBaseFilename=lbDMF-BinSamples-0.7.1
AppPublisher=Lothar Behrens
AppPublisherURL=http://www.lollisoft.de
AppSupportURL=http://www.sourceforge.net/projects/lbdmf
AppUpdatesURL=http://www.sourceforge.net/projects/lbdmf
DefaultDirName={sd}\lbDMF
DefaultGroupName=lbDMF
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4

[Files]
Source: "Q:\Develop\Projects\CPP\installODBC.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "Q:\Develop\Projects\bin\wxwrapper.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\Develop\Projects\CPP\watcomenv.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "Q:\Develop\Projects\CPP\binsample.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "Q:\Develop\Projects\dll\mspdb60.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\CPP\Database\psqlodbc.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\CPP\Database\*.*"; DestDir: "{app}\Database"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
; --- My current binary only file
Source: "Q:\Develop\Projects\dll\lbClasses.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbDB.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbModule.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbHook.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbDynApp.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbPluginManager.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\lbMetaApplication.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\wxWrapperDLL.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\wxmsw26_wat_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\wxAUI.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\dll\wxPropgrid.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\plugins\lbDatabaseForm.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\plugins\lbDatabaseReport.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\plugins\lbLoginWizard.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "Q:\Develop\Projects\plugins\lbVisitorOperations.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
; --- Some files needed from OW's BinNT directory
Source: "Q:\develop\Tools\watcom\binnt\mt7r13.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\develop\Tools\watcom\binnt\clbr13.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "Q:\develop\Tools\watcom\binnt\plbr13.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

[Icons]
Name: "{group}\lbDMF"; Filename: "{app}\binsample.bat"; WorkingDir: "{app}"
Name: "{userdesktop}\lbDMF"; Filename: "{app}\binsample.bat"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}"
;Name: "{userdesktop}\lbDMF Help"; Filename: "{app}\Doc\html\index.html"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}\develop\projects\cpp\BaseDevelopment"

[Run]
Filename: "{app}\installODBC.bat"; Description: "Install ODBC driver settings"; Flags: shellexec postinstall

