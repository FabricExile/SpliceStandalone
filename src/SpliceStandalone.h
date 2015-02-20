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

    SpliceStandalone(
      int &argc, char **argv,
      boost::filesystem::path fabricDir,
      std::string const &spliceFilePath = ""
      );
    virtual ~SpliceStandalone();

    const std::vector<SpliceGraphWrapper::Ptr> & wrappers();

    // returns the main window
    MainWindow * getMainWindow();

    // reload all the scripts
    void reloadAll();

    void setupFusionLook();

    // something trigger the need for a redraw of the window
    void needRedraw();

    // dispatch a message to the log window 
    void displayMessage(std::string message);

    // dispatch a message to the status bar
    void setStatusBarText(std::string caption);

    // access to the application's path
    boost::filesystem::path getFabricPath() const { return m_fabricPath; }

    // get the standard font for widgets
    QFont getWidgetFont();

    static SpliceStandalone * getInstance();

    void emitSlowOperationDescChanged( char const *descCStr );

  public slots:

    // dispatch a message to the log window 
    void slowOperation( QString desc );

    void fabricClientConstructed();
    void wrapperLoaded( SpliceGraphWrapper::Ptr wrapper );

  signals:

    void slowOperationDescChanged( QString desc );

  private:

    void constructFabricClient();

    // the splash screen
    QSplashScreen * m_splashScreen;

    // the main window 
    MainWindow * m_mainWindow;

    boost::filesystem::path m_fabricPath;
    std::string m_spliceFilePath;
    std::vector<SpliceGraphWrapper::Ptr> m_wrappers;
  };

  class FabricClientConstructor : public QObject
  {
    Q_OBJECT
    
  public:

    FabricClientConstructor() {}

  public slots:

    void process();

  signals:

    void finished();
  };

  class WrapperLoader : public QObject
  {
    Q_OBJECT
    
  public:

    WrapperLoader( std::string const &splicePath, MainWindow *mainWindow )
      : m_splicePath( splicePath )
      , m_mainWindow( mainWindow )
    {
    }

  public slots:

    void process();

  signals:

    void wrapperLoaded( SpliceGraphWrapper::Ptr );
    void finished();

  private:

    std::string m_splicePath;
    MainWindow *m_mainWindow;
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
