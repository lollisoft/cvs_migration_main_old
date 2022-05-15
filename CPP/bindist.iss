; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
AppName=lbDMF Binary Samples
AppVerName=lbDMF Binary Samples 1.3.2
OutputBaseFilename=lbDMF-BinSamples-1.3.2
AppPublisher=Lothar Behrens
AppPublisherURL=http://www.lollisoft.de
AppSupportURL=http://www.sourceforge.net/projects/lbdmf
AppUpdatesURL=http://www.sourceforge.net/projects/lbdmf
DefaultDirName={sd}\lbDMF
DefaultGroupName=lbDMF
LicenseFile=license-bindist.txt
; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; MinVersion: 4,4

[Files]
Source: "installODBC.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "dist\bindist_iss\bin\wxwrapper.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "dist\bindist_iss\bin\xsltproc.exe"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
;Source: "dist\bindist_iss\dll\mspdb60.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\dist_iss_psqlodbc\psqlodbc.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

Source: "dist\bindist_iss\plugins\lbCryptoStream.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbDMFBasicActionSteps.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbFileOperationsPlugin.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbXSLTTransformer.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbDatabaseForm.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbDatabaseReport.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbDMFDataModel.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbDynamicAppStorage.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbLoginWizard.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbVisitorOperations.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\UserFeedback.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbwxSFDesigner.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbwxUpdateChecker.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\lbwxPropertyForm.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;

Source: "dist\bindist_iss\dll\lbClasses.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbDB.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbModule.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbHook.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbDynApp.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbPluginManager.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbMetaApplication.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbWorkflowEngine.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxWrapperDLL.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxShapeFramework.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxJson.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxbase28_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxbase28_xml_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxbase28_net_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxmsw28_adv_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxmsw28_core_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxmsw28_html_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxmsw28_xrc_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxmsw28_aui_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

; Monolithic
;Source: "Q:\Develop\Projects\dll\wxmsw28_gcc_custom.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
;Source: "Q:\Develop\Projects\dll\wxAUI.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\wxPropgrid.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\lbxslt.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

; Also now add the code generation plugin.
Source: "dist\bindist_iss\plugins\lbDMFXslt.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;
Source: "Test\GUI\wxWrapper\splash.png"; DestDir: "{app}\Develop\Projects\bin"; CopyMode: alwaysoverwrite
Source: "Test\GUI\wxWrapper\splash.png"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "watcomenv.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "binsample.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite
Source: "Database\*.*"; DestDir: "{app}\Database"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
Source: "Test\GUI\wxWrapper\toolbarimages\*.xpm"; DestDir: "{app}\toolbarimages"; CopyMode: alwaysoverwrite;
Source: "Test\GUI\wxWrapper\toolbarimages\*.png"; DestDir: "{app}\toolbarimages"; CopyMode: alwaysoverwrite;
;Source: "Q:\develop\Projects\CPP\BaseDevelopment\MetaApp-Bindist.mad"; DestDir: "{app}"; DestName: "MetaApp.mad"; CopyMode: alwaysoverwrite;
Source: "AppDevelopmentDemo\DynamicApp\XSLT_Templates\*.*"; DestDir: "{app}\XSLT"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
Source: "AppDevelopmentDemo\DynamicApp\UMLSamples\*.*"; DestDir: "{app}\UMLSamples\"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
Source: "AppDevelopmentDemo\DynamicApp\Doc\ApplicationprototypingDokumentation.pdf"; DestDir: "{app}\"; CopyMode: alwaysoverwrite;

; Initially write a required file for this folder as it is no longer versioned due to permanent changes
Source: "AppDevelopmentDemo\DynamicApp\XSLT_Templates\include\XMISettingsTemplate.xsl"; DestDir: "{app}\XSLT\XMIToDMF"; DestName: "XMISettings.xsl"; CopyMode: alwaysoverwrite
; --- My current binary only file

; Solved Windows 7 File locking issues by using a transaction. Also this is more than many times faster!
;Source: "Q:\Develop\Projects\CPP\Database\lbDMF.db3"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
;Source: "Q:\Develop\Projects\CPP\Database\MetaApp.mad"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;
;Source: "Q:\Develop\Projects\CPP\Database\lbDMF Manager.daf"; DestDir: "{app}"; CopyMode: alwaysoverwrite; Flags: recursesubdirs;



; --- wxLua Runtime to demonstrate code generation for it.
Source: "dist\dist_iss_wxLua_dist\*.bat"; DestDir: "{app}"; CopyMode: alwaysoverwrite;
Source: "dist\dist_iss_wxLua_dist\*.exe"; DestDir: "{app}"; CopyMode: alwaysoverwrite;
Source: "dist\dist_iss_wxLua_dist\*.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

; --- MinGW Library needed to run my applications.
Source: "dist\bindist_iss_MinGW_Libraries\mingwm10.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss_MinGW_Libraries\libgcc_s_dw2-1.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss_MinGW_Libraries\libstdc++-6.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;


; Stuff for Sqlite based databases
Source: "dist\bindist_iss\dll\sqlite3.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\databaselayer_sqllite.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\plugins\DatabaseLayerGateway.dll"; DestDir: "{app}\Develop\Projects\plugins"; CopyMode: alwaysoverwrite;


; --- Some files needed from OW's BinNT directory
Source: "dist\bindist_iss_Watcom_Libraries\mt7r17.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss_Watcom_Libraries\clbr17.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss_Watcom_Libraries\plbr17.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
; --- Some files needed from libxml and libxslt
;Source: "Q:\Develop\Projects\dll\iconv.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\zlib1.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\libxml2.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

; Required by xsltproc
Source: "dist\bindist_iss\dll\libxslt.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;
Source: "dist\bindist_iss\dll\libexslt.dll"; DestDir: "{app}\Develop\Projects\dll"; CopyMode: alwaysoverwrite;

Source: "Test\GUI\wxWrapper\lbdmf.ico"; DestDir: "{app}"; CopyMode: alwaysoverwrite;

[Icons]
Name: "{group}\lbDMF"; IconFilename: "{app}\lbdmf.ico"; Filename: "{app}\binsample.bat"; WorkingDir: "{app}"
Name: "{group}\Applicationprototyping Dokumentation"; IconFilename: "{app}\lbdmf.ico"; Filename: "{app}\ApplicationprototypingDokumentation.pdf"; WorkingDir: "{app}"
Name: "{userdesktop}\lbDMF"; IconFilename: "{app}\lbdmf.ico"; Filename: "{app}\binsample.bat"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}"
;Name: "{userdesktop}\lbDMF Help"; Filename: "{app}\Doc\html\index.html"; MinVersion: 4,4; Tasks: desktopicon; WorkingDir: "{app}\develop\projects\cpp\BaseDevelopment"

[Run]
Filename: "http://www.lollisoft.de/index.php?module=xarpages&func=display&pid=23"; Flags: shellexec
Filename: "{app}\installODBC.bat"; Description: "Install ODBC driver settings"; Flags: shellexec postinstall unchecked
