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
      case Qt::Key_G:
      {
        m_window->m_glWidget->toggleGrid();
        return true;
      }
      case Qt::Key_R:
      {
        m_window->m_glWidget->resetCameraPosition();
        return true;
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
	QMainWindow(parent, flags)

{
  SpliceStandalone* app = SpliceStandalone::getInstance();
  boost::filesystem::path imagesDir = app->getAppPath() / "images";
  boost::filesystem::path feLogoPath = imagesDir / "fe_logo.png";

  // title && logo
  setWindowTitle("Fabric Engine Splice Standalone");
  QIcon icon(feLogoPath.string().c_str()); 
  setWindowIcon(icon);

  // global keys
  m_eventFilter = new MainWindowKeyFilter(this);
  installEventFilter(m_eventFilter);

  // status bar
  m_statusBarTimer.start();
  m_statusBar = new QStatusBar(this);
  setStatusBar(m_statusBar);
  m_statusBar->show();

  // menu
  QMenuBar * menuBar = new QMenuBar(this);
  setMenuBar(menuBar);
  QMenu * menu;
  QAction * action;

  // file menu
  menu = menuBar->addMenu("File");
  connect(action = menu->addAction("Load"), SIGNAL(triggered()), this, SLOT(loadScript()));
  action->setShortcut(QKeySequence("Ctrl+O"));
  action->setShortcutContext(Qt::ApplicationShortcut);
  menu->addSeparator();
  connect(action = menu->addAction("Quit"), SIGNAL(triggered()), this, SLOT(close()));
  action->setShortcut(QKeySequence("Alt+F4"));
  action->setShortcutContext(Qt::ApplicationShortcut);

  // display
  menu = menuBar->addMenu("Display");
  connect(action = menu->addAction("Wireframe"), SIGNAL(triggered()), this, SLOT(displayAsWireFrame()));
  connect(action = menu->addAction("Shaded"), SIGNAL(triggered()), this, SLOT(displayAsShaded()));

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

  showAttributeEditor();
  showKLEditor();
  showLogWindow();
  bringToFront(m_sourceEditors[0]);

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

	m_glWidget = new GLWidget(glFormat, this);
	m_glWidget->makeCurrent();
  m_glWidget->installEventFilter(m_eventFilter);
	setCentralWidget(m_glWidget);

	m_glWidget->show();

  connect( m_glWidget, SIGNAL(redrawn()) , this, SLOT(glViewRedrawn()) );

	m_glWidget->updateGL();

	m_manipulatorContext = new ManipulationTool(m_glWidget);

  m_glWidget->setFocus(Qt::ActiveWindowFocusReason);
}

void MainWindow::attributeChanged( QSpliceGraphWrapper wrapper, std::string attributeName )
{
  SpliceGraphWrapper::Ptr wrapperPtr = wrapper.wrapper;
  if(!wrapperPtr)
    return;

  // perform an evaluation
  wrapperPtr->evaluate(true);
  redraw();
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

void MainWindow::glViewRedrawn()
{
  if(m_statusBarTimer.elapsed() < 1000)
    return;
  QString caption;
  caption.setNum(m_glWidget->fps(), 'f', 2);
  m_statusBar->showMessage(caption+" FPS");
  m_statusBarTimer.start();
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

void MainWindow::loadScript()
{ 
  std::string fileName = stdStringFromQString(QFileDialog::getOpenFileName(this,
       tr("Splice File"), "$HOME", tr("Splice Files (*.splice)")));

  if(fileName.length() == 0)
    return;

	clearAll();

	SpliceStandalone* app = SpliceStandalone::getInstance();
	SpliceGraphWrapper::Ptr wrapper = app->addWrapper(fileName);

  // for(size_t i=0;i<m_attributeEditors.size();i++)
  //   m_attributeEditors[i]->setWrapper(wrapper);
  for(size_t i=0;i<m_sourceEditors.size();i++)
    m_sourceEditors[i]->setWrapper(wrapper);

	redraw();
}

void MainWindow::clearAll()
{ 
	SpliceStandalone* app = SpliceStandalone::getInstance();

  for(size_t i=0;i<m_logWidgets.size();i++)
    m_logWidgets[i]->clear();
  // for(size_t i=0;i<m_attributeEditors.size();i++)
  // {
  //   m_attributeEditors[i]->setWrapper(SpliceGraphWrapper::Ptr());
  // }
  for(size_t i=0;i<m_sourceEditors.size();i++)
  {
    m_sourceEditors[i]->clear();
    m_sourceEditors[i]->setWrapper(SpliceGraphWrapper::Ptr());
  }

  m_glWidget->resetRTVals();

	app->clearAll();

  m_glWidget->updateGL();
}

void MainWindow::reloadAll()
{ 
	SpliceStandalone* app = SpliceStandalone::getInstance();
	app->reloadAll();
	m_glWidget->updateGL();
}

void MainWindow::activateManipulator()
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
