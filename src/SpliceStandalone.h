#ifndef __SPLICESTANDALONE_H__
#define __SPLICESTANDALONE_H__

#include <QtGui/QApplication>
#include <QtGui/QWindowsStyle>
#include <QtGui/QFont>
#include <QtGui/QSplashScreen>
#include <vector>
#include <boost/filesystem/path.hpp>

#include "SpliceGraphWrapper.h"

namespace FabricSplice {

  class MainWindow;

  class SpliceStandalone : public QApplication
  {
  public:
  	SpliceStandalone(int &argc, char **argv);
  	virtual ~SpliceStandalone();

  	SpliceGraphWrapper::Ptr addWrapper(const std::string & klPath);
  	const std::vector<SpliceGraphWrapper::Ptr> & wrappers();

  	// this will make sure the main window is created and then raise it
  	void showMainWindow();

  	// clear all the scripts
  	void clearAll();

  	// reload all the scripts
  	void reloadAll();

  	void setupFusionLook();

  	// something trigger the need for a redraw of the window
  	void needRedraw();

  	// a node change the params on a node , the window need to reload them 
  	void updateParams();

  	// dispatch a message to the log window 
  	void displayMessage(std::string message);

    // access to the application's path
    boost::filesystem::path getAppPath() const { return m_appPath; }

    // get the standard font for widgets
    QFont getWidgetFont();

  private:

    void constructFabricClient();

    // the splash screen
    QSplashScreen * m_splashScreen;

  	// the main window 
  	MainWindow * m_mainWindow;

    boost::filesystem::path m_appPath;
  	std::vector<SpliceGraphWrapper::Ptr> m_wrappers;

  };

  // global accessor for all fonts
  inline QFont getApplicationWidgetFont()
  {
    SpliceStandalone * app = static_cast<SpliceStandalone *>(QApplication::instance());
    return app->getWidgetFont();
  }


};

#endif // __SPLICESTANDALONE_H__