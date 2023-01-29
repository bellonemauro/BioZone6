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


bool copyPath(QString _src, QString _dst)
{
	// check if the source folder exists
	QDir dir(_src);
	if (!dir.exists())
		return false;

	foreach(QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
		QString dst_path = _dst + QDir::separator() + d;
		dir.mkpath(dst_path);
		copyPath( _src + QDir::separator() + d, dst_path); // recursive copy
	}

	foreach(QString f, dir.entryList(QDir::Files)) {
		QFile::copy(_src + QDir::separator() + f, _dst + QDir::separator() + f);
	}
	return true;
}

bool existFolder(BioZone6_GUI& _l, QString _folder, bool _create = false)
{
	QDir my_dir;
	if (!my_dir.exists(_folder)) {
		std::cerr << _folder.toStdString() << 
			" directory does not exists in the home folder .... creating it" << std::endl;
		if (my_dir.mkpath(_folder) && _create)
		{
			std::cout << " Directory created " <<
				_folder.toStdString() << std::endl;
		}
		else
		{
			std::cerr << " BioZone6 wizard could not create the " << _folder.toStdString() << " folder " << std::endl;

			QString ss = "The BioZone6 wizard directory <br>";
			ss.append(_folder);
			ss.append("<br> does not exists in the installation folder, ");
			ss.append("BioZone6 wizard cannot run  <br>");
			ss.append("A reinstallation of BioZone6 wizard may solve the problem ");
			QMessageBox::warning(&_l, "ERROR", ss);
			return false;
		}
	}

	return true;
}


// if it is the first time that the software runs,
// it will check if required paths already exist and 
// set up useful files and folders in the user files
// if this function return false, some path may be broken
//
// verify that we have a correct installation and user setting
// expected folder structure: 
// 
// c:\users\__name__\Documents\Biozone6\
//                                     \Ext_data
//                                     \settings
//                                     \tips\
//											\Standard\
//											\UWZ\
//                                          \Wide\
//
bool initPaths(BioZone6_GUI &_l, QString& _tips_user_path,
	QString& _settings_user_path, QString& _ext_data_user_path)
{
	// detect the home path ... C:/users/user/
	QString home_path = QDir::homePath();   
	home_path.append("/Documents/BioZone6/");
	if (!existFolder(_l, home_path, true)) return false;

	// is the software running folder  ... C:/Program Files/...
	QDir app_dir = QDir::currentPath();    
	
	// default tips path into the installation folder
	QString app_tips_path = app_dir.path();    
	app_tips_path.append("/tips/");
	if (!existFolder(_l, app_tips_path)) return false;

	// default setting path into the installation folder
	QString app_settings_path = app_dir.path();
	app_settings_path.append("/settings/");
	if (!existFolder(_l, app_settings_path)) return false;

	// default ext_data path into the installation folder
	QString ext_data_path = app_dir.path();
	ext_data_path.append("/Ext_data/");
	if (!existFolder(_l, ext_data_path)) return false;
	// if the directory BioZone6 does not exist in the home folder, create it

	
	// here we set the tips path in the user folder 
	_tips_user_path = home_path + "/tips/";
	if (!existFolder(_l, _tips_user_path, true)) return false;
	if (!copyPath(app_tips_path, _tips_user_path))
	{
		std::cerr << "Could not copy tips folder in the user directory" << std::endl;
		QString ss = "Could not copy tips folder in the user directory";
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}

	// here we set the setting path in the user folder 
	_settings_user_path = home_path+ "/settings/";
	if (!existFolder(_l, _settings_user_path, true)) return false;
	if (!copyPath(app_settings_path, _settings_user_path))
	{
		std::cerr << "Could not copy settings folder in the user directory" << std::endl;
		QString ss = "Could not copy settings folder in the user directory";
		QMessageBox::warning(&_l, "ERROR", ss);
		return false;
	}

	// here we set the ext data path in the user folder 
	_ext_data_user_path = home_path + "/Ext_data/";
	if (!existFolder(_l, _ext_data_user_path, true)) return false;

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
