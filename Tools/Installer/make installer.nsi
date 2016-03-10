#Settings-----------------------------------------------------------

;SetCompressor /SOLID LZMA

#MultiUser Settings
!define MULTIUSER_EXECUTIONLEVEL Highest
!define MULTIUSER_MUI
!define MULTIUSER_INSTALLMODE_COMMANDLINE
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_KEY "Software\Vile Corp\"
!define MULTIUSER_INSTALLMODE_DEFAULT_REGISTRY_VALUENAME ""
!define MULTIUSER_INSTALLMODE_INSTDIR ""

#Includes
!include "MultiUser.nsh"
!include "MUI2.nsh"

#General Settings
Name "Vile Corp"
OutFile "setup_vile_corp_1.0.exe"
BrandingText " "
Var StartMenuFolder
!define UserdataDir VileCorp1.0
!define UserdataFlags "--config-dir ${UserdataDir}"  


#Installer pages----------------------------------------------------

#Welcome page
!define MUI_WELCOMEFINISHPAGE_BITMAP InstallerGraphic.bmp
!define MUI_ABORTWARNING
!insertmacro MUI_PAGE_WELCOME

#Component selection page
;!insertmacro MUI_PAGE_COMPONENTS

#Chose install directory page
!insertmacro MUI_PAGE_DIRECTORY

#Start Menu Folder Page
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "SHCTX" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Vile Corp" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

#Installing files page
!insertmacro MUI_PAGE_INSTFILES

#Finished installing page
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_TEXT "Create desktop shortcut"
!define MUI_FINISHPAGE_RUN_FUNCTION "iconToDesktop"

Function "iconToDesktop"
	CreateShortCut "$DESKTOP\Vile Corp.lnk" "$INSTDIR\Vile Corp\Vile Corp.exe"
FunctionEnd

!insertmacro MUI_PAGE_FINISH


#Uninstaller pages----------------------------------------------------
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES


#Prepare release folder for packaging---------------------------------
!delfile "..\..\Output\Bin\x86\Release\*.cso"
!delfile "..\..\Output\Bin\x86\Release\*.exp"
!delfile "..\..\Output\Bin\x86\Release\*.iobj"
!delfile "..\..\Output\Bin\x86\Release\*.lib"
!delfile "..\..\Output\Bin\x86\Release\*.ipdb"
!delfile "..\..\Output\Bin\x86\Release\*.pdb"
!delfile "..\..\Output\Bin\x86\Release\*.txt"
!delfile "..\..\Output\Bin\x86\Release\StortSpelprojekt.exe"


#Installer language
!insertmacro MUI_LANGUAGE "English"


#Code for the installer .exe------------------------------------------
Section "Vile Corp" VCSection

	#Set the installation directoy
	SetOutPath "$INSTDIR\Vile Corp"

	#Unpack the game contents
	File /r /x *.ini "..\..\Output\Bin\x86\Release\*"
	
	#Add the "Programs and Features" post
	WriteRegStr SHCTX "Software\Vile Corp" "" "$INSTDIR\Vile Corp"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vile Corp" \
				"DisplayName" "Vile Corp"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vile Corp" \
				"UninstallString" "$\"$INSTDIR\Vile Corp\Uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vile Corp" \
				"DisplayIcon" "$\"$INSTDIR\Vile Corp\Vile Corp.exe$\""

	#Create the uninstaller .exe
	WriteUninstaller "$INSTDIR\Vile Corp\Uninstall.exe"

	#Create shortcuts
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
		CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Vile Corp.lnk" "$INSTDIR\Vile Corp\Vile Corp.exe" "${UserdataFlags}"
		CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Vile Corp\Uninstall.exe" "${UserdataFlags}"
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Function .onInit

	!insertmacro MULTIUSER_INIT
	!insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd


#Descriptions to installer sections (Needed for the MUI_PAGE_COMPONENTS)
;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
;!insertmacro MUI_DESCRIPTION_TEXT ${VCSection} "Vile Corp executable and data."
;!insertmacro MUI_FUNCTION_DESCRIPTION_END


#Code for the uninstaller .exe----------------------------------------
Section "Uninstall"

	#Delete files and directories
	RMDir /r "$INSTDIR\Assets\*"
	Delete "$INSTDIR\AI.dll"
	Delete "$INSTDIR\AssetManager.dll"
	Delete "$INSTDIR\FW1FontWrapper.dll"
	Delete "$INSTDIR\libyse32.dll"
	Delete "$INSTDIR\Renderer.dll"
	Delete "$INSTDIR\System.dll"
	Delete "$INSTDIR\Vile Corp.exe"
	Delete "$INSTDIR\Uninstall.exe"
	Delete "$INSTDIR\YSElog.txt"
	RMDir "$INSTDIR"
	
	#Delete shortcuts
	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
	Delete "$DESKTOP\Vile Corp.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\Vile Corp.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"

	#Delete "Programs and Features" post
	DeleteRegKey /ifempty SHCTX "Software\Vile Corp"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Vile Corp"
	
SectionEnd

Function un.onInit

	!insertmacro MULTIUSER_UNINIT
	!insertmacro MUI_UNGETLANGUAGE

FunctionEnd