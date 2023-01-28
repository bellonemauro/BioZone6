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
#include <QScreen>
#include <QSplashScreen>
#include <QTimer>
#include <QDir>
#include <QMessageBox>
#include <QScreen>


bool copyPath(QString src, QString dst)
{
	QDir dir(src);
	if (!dir.exists())
		return false;

	foreach(QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString dst_path = dst + QDir::separator() + d;
		dir.mkpath(dst_path);
		copyPath( src + QDir::separator() + d, dst_path);
	}

	foreach(QString f, dir.entryList(QDir::Files)) {
		QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
	}
	return true;
}

// if it is the first time that the software runs,
// it will check if required paths already exist and 
// set up useful files and folders in the user files
// if this function return false, some path may be broken
bool initPaths(BioZone6_GUI &_l, QString& _tips_user_path,
	QString& _settings_user_path, QString& _ext_data_user_path)
{
	// detect the home path ... C:/users/user/
	QString home_path = QDir::homePath();   

	// is the installation folder  ... C:/Program Files/...
	QDir app_dir = QDir::currentPath();    
	
	// default tips path into the installation folder
	QString app_tips_path = app_dir.path();    
	app_tips_path.append("/tips/");

	// default setting path into the installation folder
	QString app_settings_path = app_dir.path();
	app_settings_path.append("/settings/");

	// default ext_data path into the installation folder
	QString ext_data_path = app_dir.path();
	ext_data_path.append("/Ext_data/");

	// if the directory BioZone6 does not exist in the home folder, create it
	home_path.append("/Documents/BioZone6/");
	QDir home_dir;
	if (!home_dir.exists(home_path)) {
		std::cerr << " BioZone6 directory does not exists in the home folder .... creating it" << std::endl;
		if (home_dir.mkpath(home_path))
		{
			std::cout << " Directory created " <<
				home_path.toStdString() << std::endl;
		}
		else
		{
			std::cerr << " BioZone6 wizard could not create the home folder " << std::endl;
			QString ss = "Home documents directory does not exists in the installation folder,";
			ss.append("BioZone6 wizard cannot run  <br>");
			ss.append("A reinstallation of BioZone6 wizard may solve the problem ");
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}
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
	
	// check if the settings directory exists in the program files path, 
	// if it doesn't the installation may be broken
	QDir settings_dir;
	settings_dir.setPath(app_settings_path);
	if (!settings_dir.exists(app_settings_path)) {
		std::cerr << "BioZone6 wizard settings directory does not exists" << std::endl;
		QString ss = "Settings directory does not exists in the installation folder,";
		ss.append("BioZone6 wizard cannot run  <br>");
		ss.append("A reinstallation of BioZone6 wizard may solve the problem ");
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}
	else {
		std::cout << " Found directory " <<
			app_settings_path.toStdString() << std::endl;
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


	// here we set the tips path in the user folder 
	QDir tips_user_dir;
	_tips_user_path = home_path + "/tips/";
	if (!tips_user_dir.exists(_tips_user_path)) // if the tips user folder does not exist, create and copy
	{
		if (!tips_user_dir.mkpath(_tips_user_path))
		{
			std::cerr << "Could not create tips folder in the user directory" << std::endl;
			QString ss = "Could not create tips folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

		if(!copyPath( app_tips_path ,_tips_user_path))
		{
			std::cerr << "Could not copy tips folder in the user directory" << std::endl;
			QString ss = "Could not copy tips folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}
	}
	
	// here we set the setting path in the user folder 
	QDir settings_user_dir;
	_settings_user_path = home_path+ "/settings/";
	if (!settings_user_dir.exists(_settings_user_path))
	{
		if (!settings_user_dir.mkpath(_settings_user_path)) {
			std::cerr << "Could not create settings folder in the user directory" << std::endl;
			QString ss = "Could not create settings folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

		if (!copyPath(app_settings_path, _settings_user_path))
		{
			std::cerr << "Could not copy settings folder in the user directory" << std::endl;
			QString ss = "Could not copy settings folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}


	// here we set the ext data path in the user folder 
	QDir ext_data_user_dir;
	QString ext_data_home_path = home_path;
	ext_data_home_path.append("/Ext_data/");
	if (!ext_data_user_dir.exists(ext_data_home_path))
	{
		_ext_data_user_path = ext_data_home_path;
		if (!ext_data_user_dir.mkpath(_ext_data_user_path)) {
			std::cerr << "Could not create ext_data folder in the user directory" << std::endl;
			QString ss = "Could not create ext_data folder in the user directory";
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}

	}
	else {
		_ext_data_user_path = ext_data_home_path;
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
        //a.setAttribute(Qt::AA_EnableHighDpiScaling); // this is enabled by default from qt6
		
		BioZone6_GUI window;

		// check for high DPI screens
		
		int logical_dpi_x = QApplication::primaryScreen()->logicalDotsPerInchX();// ->logicalDpiX();
		int logical_dpi_y = QApplication::primaryScreen()->logicalDotsPerInchY();//desktop()->logicalDpiY();
		int physical_dpi_x = QApplication::primaryScreen()->physicalDotsPerInchX();// ->physicalDpiX();
		int physical_dpi_y = QApplication::primaryScreen()->physicalDotsPerInchY();//desktop()->physicalDpiY();

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
        //QString user_path;
		// set internal application paths
		QString tips_user_path;
		QString settings_user_path;
		QString ext_data_user_path;

#ifdef _DEBUG
        std::cout << " Running with debug settings " << std::endl;
        initPaths(window, user_path);
#else
        if (!initPaths(window, tips_user_path, 
			settings_user_path, ext_data_user_path)) return 0;
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
