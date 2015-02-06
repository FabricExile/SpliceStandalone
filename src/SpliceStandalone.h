#ifndef __SPLICESTANDALONE_H__
#define __SPLICESTANDALONE_H__

#include <QtGui/QtGui>
#include <QtGui/QApplication>
#include <QtGui/QWindowsStyle>
#include <QtGui/QFont>
#include <QtGui/QSplashScreen>
#include <vector>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include "SpliceGraphWrapper.h"
#include "MainWindow.h"

namespace FabricSplice {

  class MainWindow;

  class SpliceStandalone : public QApplication
  {
    Q_OBJECT

    friend class SlowOperationCallback;

  public:

    SpliceStandalone(int &argc, char **argv, boost::filesystem::path fabricDir, std::string spliceFilePath = "");
    virtual ~SpliceStandalone();

    SpliceGraphWrapper::Ptr addWrapper(const std::string & splicePath);
    const std::vector<SpliceGraphWrapper::Ptr> & wrappers();

    // this will make sure the main window is created and then raise it
    void showMainWindow();

    // returns the main window
    MainWindow * getMainWindow();

    // clear all the scripts
    void clearAll();

    // reload all the scripts
    void reloadAll();

    void setupFusionLook();

    // something trigger the need for a redraw of the window
    void needRedraw();

    // dispatch a message to the log window 
    void displayMessage(std::string message);

    // dispatch a message to the log window 
    void slowOperation(const char *descCStr, unsigned int descLength);

    // dispatch a message to the status bar
    void setStatusBarText(std::string caption);

    // access to the application's path
    boost::filesystem::path getFabricPath() const { return m_fabricPath; }

    // get the standard font for widgets
    QFont getWidgetFont();

    static SpliceStandalone * getInstance();

  signals:

    void slowOperationDescChanged( char const *descCStr );

  private:

    void constructFabricClient();

    QThread *m_slowOperationThread;

    // the splash screen
    QSplashScreen * m_splashScreen;

    // the main window 
    MainWindow * m_mainWindow;

    boost::filesystem::path m_fabricPath;
    std::vector<SpliceGraphWrapper::Ptr> m_wrappers;
  };

  // global accessor for all fonts
  inline QFont getApplicationWidgetFont()
  {
    if(SpliceStandalone::getInstance())
      return SpliceStandalone::getInstance()->getWidgetFont();
    return QFont();
  }
};

#endif // __SPLICESTANDALONE_H__
