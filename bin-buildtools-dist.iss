; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=lbDMF Binary Build Tools
AppVerName=lbDMF Binary Build Tools 1.0.2
OutputBaseFilename=lbDMF-BinbuildTools-1.0.2
AppPublisher=Lothar Behrens
AppPublisherURL=http://www.lollisoft.de
AppSupportURL=http://www.sourceforge.net/projects/lbdmf
AppUpdatesURL=http://www.sourceforge.net/projects/lbdmf
DefaultDirName={sd}\lbDMF
DefaultGroupName=lbDMF
LicenseFile=license.txt
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Tasks]
;Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4

[Files]
Source: "Q:\develop\Tools\BinBuildTools\flex.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\bison.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\bison.simple"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\gawk.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\sh.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\ssh.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cp.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cat.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\echo.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\rm.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\mkdir.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Projects\bin\mkmk.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\unixfind.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\grep.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygcrypto-0.9.8.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygintl-3.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygpcre-0.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygwin1.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygz.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygminires.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
;Source: "Q:\develop\Tools\BinBuildTools\cygintl-3.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygintl-2.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygintl-1.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\cygiconv-2.dll"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\develop\Tools\BinBuildTools\make.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Q:\Develop\Projects\dll\mspdb60.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;


[Icons]
;Name: "{group}\lbDMF Develop"; Filename: "{app}\watcomenv.bat"; WorkingDir: "{app}\develop\projects\cpp\BaseDevelopment"
;Name: "{userdesktop}\lbDMF Develop"; Filename: "{app}\watcomenv.bat"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}\develop\projects\cpp\BaseDevelopment"
;Name: "{userdesktop}\lbDMF Help"; Filename: "{app}\develop\projects\cpp\Doc\html\index.html"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}\develop\projects\cpp\BaseDevelopment"

[Run]
;Filename: "{app}\watcomenv.bat"; Description: "Launch My Program"; Flags: shellexec postinstall skipifsilent

