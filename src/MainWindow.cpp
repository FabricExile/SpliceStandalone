#include <QtGui/QtGui>
#include <QtGui/QColor>
#include <QtGui/QRgb>
#include <QtGui/QIcon>

#include <FabricSplice.h>

#include "macros.h"
#include "MainWindow.h"
#include "SpliceStandalone.h"
#include "Widgets/TimeSliderWidget.h"
#include "Widgets/GLWidget.h"

using namespace FabricSplice;

/////////////////////////////////////////////////////////////////////////////////
// Global hot keys
/////////////////////////////////////////////////////////////////////////////////

MainWindowKeyFilter::MainWindowKeyFilter(MainWindow * window)
  :QObject(window)
{
  m_window = window;
}

bool MainWindowKeyFilter::eventFilter(QObject* object, QEvent* event)
{
  if (event->type() == QEvent::KeyPress) 
  {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);

    switch(keyEvent->key())
    {
      case Qt::Key_Space:
      {
        m_window->m_timeSliderWidget->play();
        return true;
      }
      case Qt::Key_Left:
      case Qt::Key_Right:
      case Qt::Key_Home:
      case Qt::Key_End:
      {
        switch(keyEvent->key())
        {
          case Qt::Key_Left:
          {
            m_window->m_timeSliderWidget->goToPreviousFrame();
            return true;
          }
          case Qt::Key_Right:
          {
            m_window->m_timeSliderWidget->goToNextFrame();
            return true;
          }
          case Qt::Key_Home:
          {
            m_window->m_timeSliderWidget->goToStartFrame();
            return true;
          }
          case Qt::Key_End:
          {
            m_window->m_timeSliderWidget->goToEndFrame();
            return true;
          }
        }
      }
      case Qt::Key_Q:
      {
        m_window->toggleManipulation();
        return true;
      }
      case Qt::Key_W:
      case Qt::Key_S:
      case Qt::Key_A:
      case Qt::Key_D:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
      {
        return m_window->m_glWidget->manipulateCamera(event);
      }
      case Qt::Key_F11:
      {
        m_window->m_glWidget->toggleGLFullScreen();
        return true;
      }
      case Qt::Key_Escape:
      {
        if(m_window->m_glWidget->isGLFullScreen())
          m_window->m_glWidget->toggleGLFullScreen();
        return true;
      }
    }
  }
  else if (event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);

    switch(keyEvent->key())
    {
      case Qt::Key_W:
      case Qt::Key_S:
      case Qt::Key_A:
      case Qt::Key_D:
      case Qt::Key_PageUp:
      case Qt::Key_PageDown:
      {
        return m_window->m_glWidget->manipulateCamera(event);
      }
    }
  }

  // standard event processing
  return QObject::eventFilter(object, event);
};

/////////////////////////////////////////////////////////////////////////////////
// MainWindow
/////////////////////////////////////////////////////////////////////////////////

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
  QMainWindow(parent, flags),
  m_glWidget( 0 )
{
  SpliceStandalone* app = SpliceStandalone::getInstance();
  boost::filesystem::path resourcesDir = app->getFabricPath() / "Resources";
  boost::filesystem::path feLogoPath = resourcesDir / "fe_logo.png";

  // title && logo
  setWindowTitle("Fabric Engine Splice Standalone");
  QIcon icon(feLogoPath.string().c_str()); 
  setWindowIcon(icon);

  // global keys
  m_eventFilter = new MainWindowKeyFilter(this);
  installEventFilter(m_eventFilter);

  // status bar
  m_fpsTimer.setInterval( 1000 );
  connect( &m_fpsTimer, SIGNAL(timeout()), this, SLOT(updateFPS()) );
  m_fpsTimer.start();

  m_statusBar = new QStatusBar(this);
  m_fpsLabel = new QLabel( m_statusBar );
  m_statusBar->addPermanentWidget( m_fpsLabel );
  setStatusBar(m_statusBar);
  m_statusBar->show();

  // menu
  QMenuBar * menuBar = new QMenuBar(this);
  setMenuBar(menuBar);
  QMenu * menu;
  QAction * action;

  // file menu
  menu = menuBar->addMenu("File");
  connect(action = menu->addAction("Quit"), SIGNAL(triggered()), this, SLOT(close()));
  action->setShortcut(QKeySequence("Alt+F4"));
  action->setShortcutContext(Qt::ApplicationShortcut);

  // display
  menu = menuBar->addMenu("Display");
  connect(action = menu->addAction("Wireframe"), SIGNAL(triggered()), this, SLOT(displayAsWireFrame()));
  connect(action = menu->addAction("Shaded"), SIGNAL(triggered()), this, SLOT(displayAsShaded()));
  connect(action = menu->addAction("Oculus Full Screen Mode"), SIGNAL(triggered()), this, SLOT(oculusFullScreenMode()));

  // manipulation
  menu = menuBar->addMenu("Manipulation");
  connect(action = menu->addAction("Toggle Manipulation Tool (Q)"), SIGNAL(triggered()), this, SLOT(toggleManipulation()));

  // windows
  menu = menuBar->addMenu("Window");
  connect(action = menu->addAction("Attribute Editor"), SIGNAL(triggered()), this, SLOT(showAttributeEditor()));
  connect(action = menu->addAction("KL Editor"), SIGNAL(triggered()), this, SLOT(showKLEditor()));
  action->setShortcut(QKeySequence("4"));
  action->setShortcutContext(Qt::ApplicationShortcut);
  connect(action = menu->addAction("Log Window"), SIGNAL(triggered()), this, SLOT(showLogWindow()));
  action->setShortcut(QKeySequence("8"));
  action->setShortcutContext(Qt::ApplicationShortcut);

	// setup initial wrappers
  setTabPosition ( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::TopDockWidgetArea, QTabWidget::North);
  setDockOptions ( ForceTabbedDocks );

	QDockWidget * timeSliderDockWidget = new QDockWidget("TimeSlider", this);
	timeSliderDockWidget->setAllowedAreas(Qt::BottomDockWidgetArea);
	timeSliderDockWidget->setFeatures(QDockWidget::DockWidgetClosable);

	m_timeSliderWidget = new TimeSliderWidget();
	timeSliderDockWidget->setWidget(m_timeSliderWidget);
	addDockWidget(Qt::BottomDockWidgetArea, timeSliderDockWidget);
	timeSliderDockWidget->setTitleBarWidget(new QWidget());

	connect( m_timeSliderWidget, SIGNAL(frameChanged(int)) , this, SLOT(timeChanged(int)) );
	
	// Set up GL framebuffer widget
	QGLFormat glFormat;
	glFormat.setDoubleBuffer(true);
	glFormat.setDepth(true);
	glFormat.setAlpha(true);
	glFormat.setSampleBuffers(true);
	glFormat.setSamples(4);

  QWidget * glParentWidget = new QWidget(this);
  glParentWidget->setLayout(new QVBoxLayout());

	m_glWidget = new GLWidget(glFormat, glParentWidget);
  m_glWidget->enableRedraw( false );
  glParentWidget->layout()->addWidget(m_glWidget);
  glParentWidget->layout()->setContentsMargins(0, 0, 0, 0);
  m_glWidget->makeCurrent();
  m_glWidget->installEventFilter(m_eventFilter);
  setCentralWidget(glParentWidget);
  m_glWidget->doneCurrent();
}

void MainWindow::initialize()
{
  showAttributeEditor();
  showKLEditor();
  showLogWindow();
  bringToFront(m_sourceEditors[0]);
  resize(1600,1000);
  showMaximized();
  raise();

  m_glWidget->initialize();
  m_glWidget->show();
  m_glWidget->enableRedraw( true );
  m_glWidget->updateGL();

  m_manipulatorContext = new ManipulationTool(m_glWidget);

  m_glWidget->setFocus(Qt::ActiveWindowFocusReason);
}

void MainWindow::attributeChanged( QSpliceGraphWrapper wrapper, std::string attributeName )
{
  FABRIC_TRY("MainWindow::attributeChanged", 

    SpliceGraphWrapper::Ptr wrapperPtr = wrapper.wrapper;
    if(!wrapperPtr)
      return;

    // update the evaluation context's input table
    FabricCore::RTVal context = wrapperPtr->getGraph().getEvalContext();

    wrapperPtr->dirtyInput(attributeName);

    // perform an evaluation
    wrapperPtr->evaluate(true);
    redraw();

    context.callMethod("", "_clear", 0, 0);

  );
}

void MainWindow::timeChanged(int frame)
{
  SpliceStandalone* app = SpliceStandalone::getInstance();
  const std::vector<SpliceGraphWrapper::Ptr> & wrappers = app->wrappers();
  for(size_t i=0;i<wrappers.size();i++)
    wrappers[i]->setFrame(frame);

  // for now fps is 24.0
  m_glWidget->setTime(float(frame) / 24.0);
}

void MainWindow::updateFPS()
{
  if ( !m_glWidget )
    return;

  QString caption;
  caption.setNum(m_glWidget->fps(), 'f', 2);
  caption += " FPS";
  m_fpsLabel->setText( caption );
}

void MainWindow::setStatusBarText(QString const &caption)
{
  m_statusBar->showMessage( caption );
}

void MainWindow::clearStatusBarText( int afterMS )
{
  if ( afterMS > 0 )
    m_statusBar->showMessage(
      m_statusBar->currentMessage(),
      afterMS
      );
  else
    m_statusBar->clearMessage();
}

void MainWindow::showAttributeEditor()
{
  if(m_attributeEditors.size() > 0)
  {
    bringToFront(m_attributeEditors[0]);
    return;
  }
  SpliceStandalone* app = SpliceStandalone::getInstance();
  const std::vector<SpliceGraphWrapper::Ptr> & wrappers = app->wrappers();
  QDockWidget *dock = new QDockWidget("Attribute Editor", this);
  AttributeEditorWidget * widget = new AttributeEditorWidget(this);
  dock->setWidget(widget);
  if (wrappers.size())
    widget->setWrapper(wrappers[0]);
  m_attributeEditors.push_back(widget);
  connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(destroyedAttributeEditor(QObject*)));
  connect(widget, SIGNAL(attributeChanged(QSpliceGraphWrapper, std::string)), this, SLOT(attributeChanged(QSpliceGraphWrapper, std::string)));
  addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::destroyedAttributeEditor(QObject * obj)
{
  for(size_t i=0;i<m_attributeEditors.size();i++)
  {
    if(m_attributeEditors[i] == obj)
    {
      std::vector<AttributeEditorWidget*>::iterator it = m_attributeEditors.begin() + i;
      m_attributeEditors.erase(it);
      break;
    }
  }
}

void MainWindow::showKLEditor()
{
  if(m_sourceEditors.size() > 0)
  {
    bringToFront(m_sourceEditors[0]);
    return;
  }
  SpliceStandalone* app = SpliceStandalone::getInstance();
  const std::vector<SpliceGraphWrapper::Ptr> & wrappers = app->wrappers();
  QDockWidget *dock = new QDockWidget("KL Editor", this);
  KLEditor * widget = new KLEditor(this);
  dock->setWidget(widget);
  if (wrappers.size())
    widget->setWrapper(wrappers[0]);
  m_sourceEditors.push_back(widget);
  connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(destroyedKLEditor(QObject*)));
  addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::destroyedKLEditor(QObject * obj)
{
  for(size_t i=0;i<m_sourceEditors.size();i++)
  {
    if(m_sourceEditors[i] == obj)
    {
      std::vector<KLEditor*>::iterator it = m_sourceEditors.begin() + i;
      m_sourceEditors.erase(it);
      break;
    }
  }
}

void MainWindow::displayAsWireFrame()
{
  m_glWidget->setWireFrame(true);
}

void MainWindow::displayAsShaded()
{
  m_glWidget->setWireFrame(false);
}

void MainWindow::oculusFullScreenMode()
{
  m_glWidget->toggleGLFullScreen(1);
}

void MainWindow::showLogWindow()
{
  if(m_logWidgets.size() > 0)
  {
    bringToFront(m_logWidgets[0]);
    return;
  }
  QDockWidget *dock = new QDockWidget("Log", this);
  LogWidget * widget = new LogWidget(this);
  dock->setWidget(widget);
  m_logWidgets.push_back(widget);
  connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(destroyedLogWindow(QObject*)));
  addDockWidget(Qt::LeftDockWidgetArea, dock);
}

void MainWindow::destroyedLogWindow(QObject * obj)
{
  for(size_t i=0;i<m_logWidgets.size();i++)
  {
    if(m_logWidgets[i] == obj)
    {
      std::vector<LogWidget*>::iterator it = m_logWidgets.begin() + i;
      m_logWidgets.erase(it);
      break;
    }
  }
}

void MainWindow::bringToFront(QWidget * widget)
{
  QDockWidget * dock = (QDockWidget *)widget->parent();
  Qt::DockWidgetArea area = dockWidgetArea(dock);
  if(area == Qt::NoDockWidgetArea)
    return;
  dock->raise();
  dock->show();
}

void MainWindow::displayMessage(std::string message)
{
  for(size_t i=0;i<m_logWidgets.size();i++)
    m_logWidgets[i]->addLog(message);
}

void MainWindow::redraw()
{
	m_glWidget->updateGL();
}

void MainWindow::updateViews()
{ 
	SpliceStandalone* app = SpliceStandalone::getInstance();
  const std::vector<SpliceGraphWrapper::Ptr> & wrappers = app->wrappers();
  if(wrappers.size() > 0)
  {
    for(size_t i=0;i<m_logWidgets.size();i++)
      m_logWidgets[i]->clear();
    for(size_t i=0;i<m_attributeEditors.size();i++)
      m_attributeEditors[i]->setWrapper(wrappers[0]);
    for(size_t i=0;i<m_sourceEditors.size();i++)
      m_sourceEditors[i]->setWrapper(wrappers[0]);
  }
}

void MainWindow::toggleManipulation()
{
	if (!m_manipulatorContext->isActive())
	{
		m_manipulatorContext->toolOnSetup();
	}
	else
	{
		m_manipulatorContext->toolOffCleanup();
	}
}
