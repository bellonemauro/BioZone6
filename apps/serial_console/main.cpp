/*  +---------------------------------------------------------------------------+
 *  |                                                                           |
 *  |                      http://www.maurobellone.com                          |
 *  |                                                                           |
 *  | Copyright (c) 2021, - All rights reserved.                                |
 *  | Authors: Mauro Bellone                                                    |
 *  | Released under GNU License.                                               |
 *  +---------------------------------------------------------------------------+ */

//uncomment to hide the console when the app starts
#define HIDE_TERMINAL
#ifdef HIDE_TERMINAL
#if defined (_WIN64) || defined (_WIN32)
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#else
// define it for a Unix machine
#endif
#endif

#include "BioZone_console.h"


  
int main (int argc, char *argv[])
{	
  QApplication a (argc, argv);
  BioZone_console window;

  //window.showFullScreen();
  //window.showMaximized();
  window.show ();
  return a.exec ();
}
