/*  +---------------------------------------------------------------------------+
*  |                                                                           |
*  |  Fluicell AB                                                              |
*  |  Copyright 2021 © Fluicell AB, http://fluicell.com/                       |
*  |  BioZone6                                                                 |
*  |                                                                           |
*  | Authors: Mauro Bellone - http://www.maurobellone.com                      |
*  | Released under GNU GPL License.                                           |
*  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
#ifndef _DEBUG
  #define HIDE_TERMINAL 
#endif
#ifdef HIDE_TERMINAL
	#if defined (_WIN64) || defined (_WIN32)
	  #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
	#else
	// define it for a Unix machine
	#endif
#endif

// extract the version string
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define VER STR(BIOZONE6_VERSION)

#include "BioZone6_GUI.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSplashScreen>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QScreen>


// if it is the first time that the software runs,
// it will check if required paths already exist and 
// set up useful files and folders in the user files
// if this function return false, some path may be broken
bool initPaths(BioZone6_GUI &_l, QString &_user_path)
{
	// detect the home path ... C:/users/user/
	QString home_path = QDir::homePath();   

	// is the installation folder  ... C:/Program Files/...
	QDir app_dir = QDir::currentPath();    
	
	// default tips path into the installation folder
	QString app_tips_path = app_dir.path();    
	app_tips_path.append("/tips/");

	// default ext_data path into the installation folder
	QString ext_data_path = app_dir.path();
	ext_data_path.append("/Ext_data/");

	// if the directory BioZone6 does not exist in the home folder, create it
	home_path.append("/Documents/BioZone6/");
	QDir home_dir;
	if (!home_dir.exists(home_path)) {
		std::cerr << " BioZone6 directory does not exists in the home folder .... creating it" << std::endl;
		home_dir.mkpath(home_path);
		std::cout << " Created directory " <<
            home_path.toStdString() << std::endl;
	}
	else {
		std::cout << " Found directory " <<
            home_path.toStdString() << std::endl;
	}

	// check if the tips directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir tips_dir;
	tips_dir.setPath(app_tips_path);
	if (!tips_dir.exists(app_tips_path) ) {
		std::cerr << "ERROR: BioZone6 tips directory does not exists in the installation folder"
			 << "A reinstallation may solve the problem "<< std::endl;
		QString ss = "Tips directory does not exists in the installation folder,";
		ss.append("BioZone6 wizard cannot run  <br>"); 
		ss.append ("A reinstallation of BioZone6 wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
			app_tips_path.toStdString() << std::endl;
	}
	

	// check if the ext_data directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir ext_data_dir;
	ext_data_dir.setPath(ext_data_path);
	if (!ext_data_dir.exists(ext_data_path)) {
		std::cerr << "BioZone6 wizard ext_data directory does not exists" << std::endl;
		QString ss = "Ext_data directory does not exists in the installation folder,";
		ss.append("BioZone6 wizard cannot run  <br>");
		ss.append("A reinstallation of BioZone6 wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
            ext_data_path.toStdString() << std::endl;
	}

	// here we set the macro path in the user folder 
	QDir protocols_user_dir;
	QString protocols_home_path = home_path;
	//protocols_home_path.append("/presetProtocols/");
	if (!protocols_user_dir.exists(protocols_home_path)) // if the protocol user folder does not exist, create and copy
	{
		_user_path = protocols_home_path;
		if (!protocols_user_dir.mkpath(_user_path))
		{
			std::cerr << "Could not create presetProtocols folder in the user directory" << std::endl;
			QString ss = "Could not create presetProtocols folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}
	else {
		_user_path = protocols_home_path;
	}


	return true;
}


int main(int argc, char **argv)
{	
	// get the version 
	std::string version;
#ifdef BIOZONE6_VERSION
	version = VER;
	std::cout << "\n Running BioZone6 version "
         << version << "\n"<< std::endl;
#endif
	try {

        QApplication a (argc, argv);
        // there is a problem with high dpi displays
        a.setAttribute(Qt::AA_EnableHighDpiScaling);
		
		BioZone6_GUI window;

		// check for high DPI screens
		int logical_dpi_x = QApplication::desktop()->logicalDpiX();
		int logical_dpi_y = QApplication::desktop()->logicalDpiY();
		int physical_dpi_x = QApplication::desktop()->physicalDpiX();
		int physical_dpi_y = QApplication::desktop()->physicalDpiY();

		// get the screen resolution of the current screen
		// so we can resize the application in case of small screens
	    QScreen *primaryScreen = QGuiApplication::primaryScreen();
		QRect rec = primaryScreen->geometry();
		int screen_height = rec.height();
		int screen_width = rec.width();

		std::cout << " Labonatip_GUI::main ::: "
			<< " logical_dpi_x " << logical_dpi_x
			<< " logical_dpi_y " << logical_dpi_y
			<< " physical_dpi_x " << physical_dpi_x
			<< " physical_dpi_y " << physical_dpi_y 
			<< " screen_height " << screen_height 
			<< " screen_width " << screen_width << std::endl;

		if (logical_dpi_x > 150) {
			QString ss = "Your display DPI is out of bound for the correct visualization of BioZone6 \n";
			ss.append("You can continue, but you will probably get bad visualization \n\n");
			ss.append("To properly visualize BioZone, try to reduce the resolution and scaling of your screen");
			
			QMessageBox::warning(&window, "ERROR", ss);

			window.appScaling(logical_dpi_x, logical_dpi_y);
			window.setGeometry(50, 50, screen_width*0.8, screen_height*0.8);
		}
        
        // set internal application paths
        QString user_path;

#ifdef _DEBUG
        std::cout << " Running with debug settings " << std::endl;
        initPaths(window, user_path);
#else
        if (!initPaths(window, user_path)) return 0;
#endif

	  // detect the home path ... C:/users/user/
	  QString biozone_home_path = QDir::homePath();
	  biozone_home_path.append("/Documents/BioZone6/");
	  window.setUserSettingPath(QString(biozone_home_path + "/settings/settings.ini"));
	  window.setUserFilesPath(biozone_home_path);

#ifdef BIOZONE6_VERSION
	  window.setVersion(version);
#endif

	  // show the slash screen
	  QSplashScreen s;
	  s.setPixmap(QPixmap(":/icons/splash_screen.png"));
	  s.show();
	  QTimer::singleShot(5000, &s, SLOT(close()));

	  //window.showFullScreen();
	  window.move(QPoint(50, 50));
	  if (screen_width < 1400)
		 QTimer::singleShot(5000, &window, SLOT(showMaximized()));
	  else
		  QTimer::singleShot(5000, &window, SLOT(show()));
  
	  return a.exec ();
  }
  catch (std::exception &e) {
	  std::cerr << " BioZone6_GUI::main ::: Unhandled Exception: "
           << e.what() << std::endl;
	  // clean up here, e.g. save the session, save the current protocol
	  // and close all config files.
	  std::cout << " Something really bad just happened, press ok to exit "
           << std::endl;
#ifndef HIDE_TERMINAL
	  std::cin.get();
#endif
	  return 0; // exit the application
  }

  return 0; // exit the application
}
