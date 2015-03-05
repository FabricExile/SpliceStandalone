#include "MainWindow.h"
#include "SpliceStandalone.h"

#include <string>

#include <FabricSplice.h>
#include <FabricCore.h>

#include "macros.h"

using namespace FabricSplice;

SpliceStandalone * gApplication = NULL;

void appLogFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appLogErrorFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appKLReportFunc(const char * message, unsigned int length)
{
  printf("%s\n", message);
  if(gApplication)
    gApplication->displayMessage(message);
}

void appSlowOperationFunc(const char *descCStr, unsigned int descLength)
{
  if ( gApplication )
    gApplication->emitSlowOperationDescChanged( descCStr );
}

void appCompilerErrorFunc(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc)
{
  printf("%d, %d, %s: %s\n", row, col, file, desc);
  
  char buf[128];
#ifdef _WIN32
  itoa(row, buf, 10);
#else
  snprintf(buf, 128, "%d", row);
#endif

  std::string stringMessage;
  stringMessage += "[KL Compiler ";
  stringMessage += level;
  stringMessage += "]: line: ";
  stringMessage += buf;
  stringMessage += " op: ";
  stringMessage += file;
  stringMessage += " : ";
  stringMessage += desc;

  if(gApplication)
    gApplication->displayMessage(stringMessage);
}

void appKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
{
  std::string stringMessage;
  stringMessage += "[KL Status]: ";
  stringMessage += message;

  if(gApplication)
  {
    std::string stringTopic = topic;
    if(stringTopic == "[StatusBar]")
      gApplication->setStatusBarText(message);
    gApplication->displayMessage(stringMessage);
  }
}

SpliceStandalone * SpliceStandalone::getInstance()
{
  return gApplication;
}

SpliceStandalone::SpliceStandalone(
  int &argc, char **argv,
  boost::filesystem::path fabricDir,
  std::string const &spliceFilePath
  ) 
  : QApplication(argc, argv)
{
  qRegisterMetaType<SpliceGraphWrapper::Ptr>("SpliceGraphWrapper::Ptr");

  gApplication = this;

  Qt::WindowFlags flags = 0;
  m_mainWindow = new MainWindow(0,flags);

  m_fabricPath = fabricDir;
  m_spliceFilePath = spliceFilePath;

  connect(this, SIGNAL(slowOperationDescChanged(QString)), this, SLOT(slowOperation(QString)));

  QPixmap pixmap((m_fabricPath / "Resources" / "splice_splash.jpg").string().c_str());
  m_splashScreen = new QSplashScreen( pixmap );
  m_splashScreen->show();

  setupFusionLook();

  Initialize(); 

  constructFabricClient();
}

SpliceStandalone::~SpliceStandalone()
{
  gApplication = NULL;
  DestroyClient();
  Finalize();
}


void SpliceStandalone::displayMessage(std::string message)
{
  if (m_mainWindow)
  {
    m_mainWindow->displayMessage(message+"\n");
  }
}

void SpliceStandalone::emitSlowOperationDescChanged( char const *descCStr )
{
  emit slowOperationDescChanged( QString( descCStr ) );
}

void SpliceStandalone::slowOperation( QString desc )
{
  if ( m_splashScreen )
    m_splashScreen->showMessage( desc, Qt::AlignHCenter | Qt::AlignBottom );
}

// dispatch a message to the status bar
void SpliceStandalone::setStatusBarText(std::string caption)
{
  if (m_mainWindow)
  {
    m_mainWindow->setStatusBarText(caption.c_str());
  }
}

const std::vector<SpliceGraphWrapper::Ptr> & SpliceStandalone::wrappers()
{
  return m_wrappers;
}

QFont SpliceStandalone::getWidgetFont()
{
  return QFont("Sans Cherif", 8 );
}

MainWindow * SpliceStandalone::getMainWindow()
{
  return m_mainWindow;
}

void FabricClientConstructor::process()
{
  FABRIC_TRY("SpliceStandalone::constructFabricClient",

    Logging::setSlowOperationFunc(appSlowOperationFunc);

    FabricCore::Client client = ConstructClient();

    Logging::setLogFunc(appLogFunc);
    Logging::setLogErrorFunc(appLogErrorFunc);
    Logging::setKLReportFunc(appKLReportFunc);

    Logging::setCompilerErrorFunc(appCompilerErrorFunc);
    Logging::setKLStatusFunc(appKLStatusFunc);

    client.loadExtension("Animation", "", false);
    client.loadExtension("InlineDrawing", "", false);
    client.loadExtension("Manipulation", "", false);
  );

  emit finished();
}

void WrapperLoader::process()
{
  m_mainWindow->makeGLCurrent();

  SpliceGraphWrapper::Ptr wrapper =
    SpliceGraphWrapper::Ptr( new SpliceGraphWrapper(m_splicePath) );

  // setup the evaluation context
  FabricCore::RTVal context = wrapper->getGraph().getEvalContext();
  context.setMember("host",
    FabricSplice::constructStringRTVal("Splice Standalone"));
  context.setMember("graph",
    FabricSplice::constructStringRTVal(m_splicePath.c_str()));
  context.setMember("currentFilePath",
    FabricSplice::constructStringRTVal(m_splicePath.c_str()));
  
  m_mainWindow->doneGLCurrent();

  emit wrapperLoaded( wrapper );
  emit finished();
}

void SpliceStandalone::fabricClientConstructed()
{
  if ( m_splashScreen )
  {
    m_splashScreen->close();
    m_splashScreen = NULL;
  }

  if ( !m_spliceFilePath.empty() )
  {
    QPixmap pixmap((m_fabricPath / "Resources" / "splice_loading.jpg").string().c_str());
    m_splashScreen = new QSplashScreen( pixmap );
    m_splashScreen->show();

    QThread* thread = new QThread;
    thread->setStackSize( 64 * 1024 * 1024 );
    WrapperLoader* worker =
      new WrapperLoader( m_spliceFilePath, m_mainWindow );
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(
      worker, SIGNAL(wrapperLoaded(SpliceGraphWrapper::Ptr)),
      this, SLOT(wrapperLoaded(SpliceGraphWrapper::Ptr))
      );
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
  }
}

void SpliceStandalone::constructFabricClient()
{
  QThread* thread = new QThread;
  thread->setStackSize( 64 * 1024 * 1024 );
  FabricClientConstructor* worker = new FabricClientConstructor();
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  connect(thread, SIGNAL(finished()), this, SLOT(fabricClientConstructed()));
  thread->start();
}

void SpliceStandalone::wrapperLoaded( SpliceGraphWrapper::Ptr wrapper )
{
  m_wrappers.push_back( wrapper );

  if(m_splashScreen)
  {
    m_splashScreen->close();
    m_splashScreen = NULL;
  }

  m_mainWindow->initialize();
}

// this will make sure the main window is created and then raise it
void SpliceStandalone::reloadAll()
{
  for (unsigned int i=0; i < m_wrappers.size(); i++ )
    m_wrappers[i]->reload();
}

void SpliceStandalone::needRedraw()
{
  m_mainWindow->redraw();
}

void SpliceStandalone::setupFusionLook()
{
  qApp->setStyle(QStyleFactory::create("Fusion"));

  // hmathee: This was causing the app to show up badly on some
  // operating systems. going for the Qt standard style now.

  // QPalette darkPalette;
  // darkPalette.setColor(QPalette::Window, QColor(53,53,53));
  // darkPalette.setColor(QPalette::WindowText, Qt::white);
  // darkPalette.setColor(QPalette::Base, QColor(25,25,25));
  // darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
  // darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
  // darkPalette.setColor(QPalette::ToolTipText, Qt::white);
  // //darkPalette.setColor(QPalette::Text, Qt::white);
  // darkPalette.setColor(QPalette::Button, QColor(53,53,53));
  // darkPalette.setColor(QPalette::ButtonText, Qt::white);
  // darkPalette.setColor(QPalette::BrightText, Qt::red);
  // darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
   
  // darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
  // darkPalette.setColor(QPalette::HighlightedText, Qt::black);
       
  // qApp->setPalette(darkPalette);
   
  // qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

}
