#  +---------------------------------------------------------------------------+
#  |                                                                           |
#  | Fluicell AB, http://fluicell.com/                                         |
#  | BioZone6 - 2021                                                           |
#  |                                                                           |
#  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
#  | Released under GNU GPL License.                                           |
#  +---------------------------------------------------------------------------+ */

# build a CPack driven installer package


# the build package only works in windows 
if( WIN32 AND NOT UNIX )

#define the website
set (WEBSITE "https://www.fluicell.com/")   
set (WEB_TUTORIAL "https://fluicell.com/products-fluicell/biopensystems/")  
set (WEB_DOCS "https://bellonemauro.github.io/PPC1API-docs.io/")  
set (GUIDE_PDF "/guide/Biozone6 - Setup guide - FBZ001.pdf")  

# set the info/about for the executable - can be changed to some other comment
set(CPACK_NSIS_URL_INFO_ABOUT "${WEBSITE}")
set(CPACK_NSIS_HELP_LINK "${WEBSITE}")

set(CPACK_PACKAGE_NAME "BioZone6")
set(CPACK_PACKAGE_VENDOR "Fluicell AB")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fluicell BioZone6")
set(CPACK_NSIS_CONTACT "Mauro Bellone bellonemauro@gmail.com" )
set(CPACK_PACKAGE_INSTALL_DIRECTORY "FluicellBioZone")# @_VERSION@")

# this allows to check older installations and properly remove them
set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL "ON") 

# add the license file
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")  

# and readme file
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")#AUTHORS.txt

#set the package version
set (CPACK_PACKAGE_VERSION "${CMAKE_Fluicell_FULL_VERSION}")     

set (BIOZONE_PROJECT_NAME "BioZone6")   

#set a cool icon to start the application
set (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/fluicell_logo.ico") #program icon
#set (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/fluicell_iconBIG.ico") #uninstall icon

#set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/Resources/fluicell_iconBIG.png")# -- NOT FOUND -- ??

set(CPACK_NSIS_MENU_LINKS 
            "${WEBSITE}" "Homepage for Fluicell"          
			#"\\\\BioZone6\\\\BioZone6.exe" "BioZone6"
            #"\\\\SerialConsole\\\\Serial_console.exe" "Serial console"
			"uninstall.exe" "Uninstall BioZone"
			"${WEB_TUTORIAL}" "Tutorials"
            "${WEB_DOCS}" "Documentation"
            "${GUIDE_PDF}" "Quick start guide" )
	

set(CPACK_NSIS_DISPLAY_NAME "Fluicell BioZone6 ") #V.${CMAKE_Fluicell_FULL_VERSION}")
			
#allows NSIS to modify paths
#set (CPACK_NSIS_MODIFY_PATH "ON")     

#SET(CPACK_PACKAGE_EXECUTABLES "Target_Name" "Target Name")
#SET(CPACK_PACKAGE_EXECUTABLES "BioZone6" "BioZone6")


#create a desktop icon with link to the .exe file
#set(CPACK_CREATE_DESKTOP_LINKS "BioZone6.exe" "BioZone6")
#for some reason the command to add the desktop link does not properly work, 
#so the the desktop link are added as a custom command


# required by cmake to install new registry key for the executable
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${BIOZONE_PROJECT_NAME}")

# this is to create the user folders during the installation
set( CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	SetOutPath \\\"$INSTDIR\\\\BioZone6\\\"
	CreateShortCut \\\"$DESKTOP\\\\BioZone6.lnk\\\" \\\"$INSTDIR\\\\BioZone6\\\\BioZone6.exe\\\"
	SetOutPath \\\"$INSTDIR\\\"
	CreateDirectory \\\"$PROFILE\\\\Documents\\\\BioZone6\\\" 
	CreateDirectory \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\tips\\\"
	CreateDirectory \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\guide\\\" 
	CreateDirectory \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\Ext_data\\\" 
	CopyFiles \\\"$INSTDIR\\\\BioZone6\\\\tips\\\\*.*\\\" \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\tips\\\"
	CopyFiles \\\"$INSTDIR\\\\BioZone6\\\\guide\\\\*.pdf\\\" \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\guide\\\"
	CopyFiles \\\"$INSTDIR\\\\BioZone6\\\\settings\\\\*.*\\\" \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\settings\\\"
	SetOutPath \\\"$INSTDIR\\\\BioZone6\\\"
	CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\BioZone6.lnk\\\" \\\"$INSTDIR\\\\BioZone6\\\\BioZone6.exe\\\" 
	SetOutPath \\\"$INSTDIR\\\\serial_console\\\"
	CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Serial console.lnk\\\" \\\"$INSTDIR\\\\serial_console\\\\serial_console.exe\\\" 
	SetOutPath \\\"$INSTDIR\\\"")
	
set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")


# Icon in the add/remove control panel. Must be an .exe file 
set(CPACK_NSIS_INSTALLED_ICON_NAME BioZone6.exe)

#set(CPACK_NSIS_MUI_FINISHPAGE_RUN ./BioZone6/BioZone6.exe)
# this does not work properly, when the software starts it starts from a 
# different folder generating an error that protocols and other application files are not found


#set(CPACK_NSIS_MUI_UNPAGE_WELCOME )
#set(CPACK_NSIS_MUI_UNPAGE_CONFIRM )
#set(CPACK_NSIS_MUI_UNCONFIRMPAGE_TEXT_LOCATION "ATTENTION: this will also remove user files, click cancel to abort") 
#set(CPACK_NSIS_MUI_UNPAGE_COMPONENTS )
#set(CPACK_NSIS_MUI_UNPAGE_DIRECTORY )
#set(CPACK_NSIS_MUI_UNPAGE_INSTFILES )
#set(CPACK_NSIS_MUI_UNPAGE_FINISH )



#add a command to remove user created files - ATTENTION: it will also remove user created files !!!
#Delete \\\"C:\ProgramData\Microsoft\Windows\Start Menu\Programs\\\" #TODO this folder must to be removed
set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
	Delete \\\"$DESKTOP\\\\BioZone6.lnk\\\"
	Delete \\\"$PROFILE\\\\..\\\\..\\\\ProgramData\\\\Microsoft\\\\Windows\\\\Start Menu\\\\Programs\\\\Fluicell BioZone6\\\\*.*\\\"
	RMDir \\\"$PROFILE\\\\..\\\\..\\\\ProgramData\\\\Microsoft\\\\Windows\\\\Start Menu\\\\Programs\\\\Fluicell BioZone6\\\"
")
#set( CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
#	Delete \\\"$DESKTOP\\\\BioZone6.lnk\\\"
#	Delete \\\"$PROFILE\\\\..\\\\..\\\\ProgramData\\\\Microsoft\\\\Windows\\\\Start Menu\\\\Programs\\\\Fluicell BioZone6 wizard\\\\*.*\\\"
	#Delete \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\presetProtocols\\\\*.*\\\" 
	#Delete \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\guide\\\\*.*\\\" 
	#Delete \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\settings\\\\*.*\\\" 
	#Delete \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\Ext_data\\\\*.*\\\" 
	#RMDir \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\presetProtocols\\\"
	#RMDir \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\guide\\\" 
	#RMDir \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\settings\\\" 
	#RMDir \\\"$PROFILE\\\\Documents\\\\BioZone6\\\\Ext_data\\\" 
#	RMDir \\\"$PROFILE\\\\..\\\\..\\\\ProgramData\\\\Microsoft\\\\Windows\\\\Start Menu\\\\Programs\\\\Fluicell BioZone6 wizard\\\"
	#RMDir \\\"$PROFILE\\\\Documents\\\\BioZone6\\\" 
#")

												   
message (STATUS "BUILD PACKAGE STATUS MESSAGE : building version ${CMAKE_Fluicell_FULL_VERSION} " )

# make the package 
include (CPack)

# I really would like to have this feature but unfortunately it is still not supported ! 
if(BUILD_WINDEPLOYQT AND WIN32)
		# Run winddeployqt if it can be found
	find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt HINTS ${QT_BINARY_DIR} ENV QTDIR PATH_SUFFIXES bin)
    FILE(GLOB FILE_EXE "${PROJECT_BINARY_DIR}/bin/Release/BioZone6/*.exe")
      FOREACH(F ${FILE_EXE})
        #INSTALL(FILES "${F}" DESTINATION ./)
 	    message (STATUS "     WINDEPLOYQT_EXECUTABLE is : ${WINDEPLOYQT_EXECUTABLE} \n")
 	    message (STATUS "     Current file target is : ${F} \n")
		message (STATUS "     BIOZONE_PROJECT_NAME is : ${BIOZONE_PROJECT_NAME} \n")
	    #HERE we cannot specify the target PACKAGE or package 
		#add_custom_command(TARGET PACKAGE PRE_BUILD WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin/Release/ COMMAND ${WINDEPLOYQT_EXECUTABLE} $<TARGET_FILE:${F}> COMMENT "Preparing Qt runtime dependencies")

	  ENDFOREACH(F)

endif()

#in order to support the previous feature of deploying QT, the previous lines have been added to the lab-on-a-tip gui application
#anyway to avoid the deployment to run every time it is compiled, the variable BUILD_WINDEPLOYQT is set to 0
if (NOT ENABLE_WINDEPLOYQT )
		message (WARNING "     REMEMBER TO DEFINE BUILD_WINDEPLOYQT if you want to build the package \n")
endif()
  
else (WIN32 AND NOT UNIX)
  message(STATUS " BUILD PACKAGE NOT YET SUPPORTED " )
endif()