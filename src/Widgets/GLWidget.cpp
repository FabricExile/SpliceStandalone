#include "GLWidget.h"

#include <stdio.h>
#include <stdlib.h>
// #include <sys/time.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <ostream>
#include <fstream>
#include <streambuf>
#include <memory>

#include <QtGui/qapplication.h>
#include <QtGui/QDesktopWidget>
#include <QtGui/QLayout>
#include <QtGui/QVBoxLayout>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "macros.h"
#include "QtToKLEvent.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLThread
/////////////////////////////////////////////////////////////////////////////////////////////////////////

GLThread::GLThread( GLWidget *glWidget )
  : m_glWidget( glWidget )
{
}

void GLThread::resizeViewport( QSize const &size )
{
}

void GLThread::run()
{
  while ( true )
    m_glWidget->paintFromGLThread();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//  GLWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :
	QGLWidget(format, parent),
  m_glThread( this ),
  m_frame( 0 )
{	
  setAutoBufferSwap(false);

  m_fps = 0.0;

  for(int i=0;i<16;i++)
    m_fpsStack[i] = 0.0;

  m_fpsTimer.start();
  resetRTVals();
	setFocusPolicy(Qt::StrongFocus);

  m_redrawEnabled = false;
  m_painting = false;
  m_prevParent = NULL;
  m_fullScreenDialog = NULL;

  m_requiresInitialize = true;
  m_requiresResize = true;
  m_resizeEnabled = true;
  m_width = 0;
  m_height = 0;

  m_glThread.start();
}

GLWidget::~GLWidget()
{
  if(m_fullScreenDialog)
    delete(m_fullScreenDialog);
}

void GLWidget::paintEvent( QPaintEvent *e )
{
  // Handled by GLThread
}

void GLWidget::resetRTVals()
{
  FABRIC_TRY("GLWidget::resetRTVals",

    m_drawing = constructObjectRTVal("OGLInlineDrawing");
    if(!m_drawing.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
      return;
    }
    m_drawing = m_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);

    m_viewport = constructObjectRTVal("OGLStandaloneViewport");
    if(!m_viewport.isValid())
    {
      printf("[GLWidget] Error: Cannot construct OGLStandaloneViewport RTVal (extension loaded?)\n");
      return;
    }
    else
    {
      std::vector<FabricCore::RTVal> args(2);
      args[0] = constructStringRTVal("default");
      args[1] = m_viewport;
      m_drawing.callMethod("", "registerViewport", args.size(), &args[0]);
    }

    m_camera = m_viewport.maybeGetMember("camera");
    m_cameraManipulator = constructObjectRTVal("CameraManipulator", 1, &m_camera);

    m_viewport.setMember("windowId", constructUInt64RTVal((uint64_t)this->winId()));

    m_drawContext = constructObjectRTVal("DrawContext");
    if(!m_drawContext.isValid())
    {
      printf("[GLWidget] Error: Cannot construct DrawContext RTVal (extension loaded?)\n");
      return;
    }

    m_timeRTVal = m_drawContext.maybeGetMemberRef( "time" );
  );

  m_requiresInitialize = true;
  m_requiresResize = true;
  m_resizeEnabled = true;
}

FabricCore::RTVal GLWidget::getInlineViewport()
{
  return m_viewport;
}

void GLWidget::paintFromGLThread()
{
  ++m_frame;

  SpliceStandalone* app = SpliceStandalone::getInstance();
  const std::vector<SpliceGraphWrapper::Ptr> & wrappers = app->wrappers();
  for(size_t i=0;i<wrappers.size();i++)
    wrappers[i]->setFrame(m_frame);

  m_timeRTVal.setFloat32( m_frame / 24.0f );

  makeCurrent();

  // compute the fps
  double ms = m_fpsTimer.elapsed();
  if(ms == 0.0)
    m_fps = 0.0;
  else
    m_fps = 1000.0 / ms;

  double averageFps = 0.0;
  for(int i=0;i<15;i++) {
    m_fpsStack[i+1] = m_fpsStack[i];
    averageFps += m_fpsStack[i];
  }
  m_fpsStack[0] = m_fps;
  averageFps += m_fps;
  averageFps /= 16.0;
  m_fps = averageFps;

  m_fpsTimer.start();

	// perform drawing
  if(m_requiresInitialize)
  {
    FABRIC_TRY("GLWidget::initializeGL",

      m_viewport.callMethod("", "setup", 1, &m_drawContext);

    );
    m_requiresInitialize = false;
    m_requiresResize = true;
  }

  if(m_requiresResize)
  {
    FABRIC_TRY("GLWidget::resizeGL",

      std::vector<FabricCore::RTVal> args(3);
      args[0] = m_drawContext;
      args[1] = constructUInt32RTVal(m_width);
      args[2] = constructUInt32RTVal(m_height);
      m_viewport.callMethod("", "resize", args.size(), &args[0]);

    );
    m_requiresResize = false;
    m_resizeEnabled = true;
  }

  m_painting = true;

  bool responsibleForSwappingBuffers = false;

  FABRIC_TRY("GLWidget::paintGL",

    std::vector<FabricCore::RTVal> args(2);
    args[0] = constructStringRTVal("default");
    args[1] = m_drawContext;
    m_drawing.callMethod("", "drawViewport", args.size(), &args[0]);

    FabricCore::RTVal responsibleForSwappingBuffersVal = m_viewport.maybeGetMember("responsibleForSwappingBuffers");
    responsibleForSwappingBuffers = responsibleForSwappingBuffersVal.getBoolean();
  );

  // if(!responsibleForSwappingBuffers)
    swapBuffers();

  // emit redrawn();

  m_painting = false;
}

float GLWidget::getTime() const
{
  FabricCore::RTVal timeVal = m_drawContext.maybeGetMember("time");
  return timeVal.getFloat32();
}

void GLWidget::setTime(float time)
{
  FABRIC_TRY("GLWidget::setTime",

    FabricCore::RTVal timeVal = constructFloat32RTVal(time);
    m_drawContext.setMember("time", timeVal);

  );
}


void GLWidget::setWireFrame(bool wireFrame)
{
  FABRIC_TRY("GLWidget::setWireFrame",

    FabricCore::RTVal wireFrameVal = constructBooleanRTVal(wireFrame);
    m_viewport.callMethod("", "setWireFrame", 1, &wireFrameVal);

  );
  updateGL();
}

void GLWidget::toggleGrid()
{
  FABRIC_TRY("GLWidget::toggleGrid",

    m_viewport.callMethod("", "toggleGrid", 0, 0);

  );
  updateGL();
}

/*************************************************************************/
/* QGLWidget calls                                                       */
/*************************************************************************/


void GLWidget::mousePressEvent(QMouseEvent *event)
{
  manipulateCamera(event);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
  manipulateCamera(event);
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
  manipulateCamera(event);
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
  manipulateCamera(event);
}

bool GLWidget::manipulateCamera(QEvent *event)
{
  bool result;
  FABRIC_TRY_RETURN("GLWidget::manipulateCamera", false,

    // Now we translate the Qt events to FabricEngine events..
    FabricCore::RTVal klevent = QtToKLEvent(event, m_viewport);

    // And then pass the event to the camera manipulator for handling.
    m_cameraManipulator.callMethod("", "onEvent", 1, &klevent);
    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
  );
  updateGL();
  return result;
}

void GLWidget::initializeGL()
{
  m_requiresInitialize = true;
}

void GLWidget::resizeGL(int width, int height)
{
  if(!m_resizeEnabled)
    return;
  if(m_painting)
    return;
  if(isGLFullScreen())
    return;

  m_width = width;
  m_height = height;

  m_painting = false;
  m_requiresResize = true;
}

void GLWidget::toggleGLFullScreen(int screenIndex)
{
  if(m_fullScreenDialog)
  {
    m_resizeEnabled = false;

    setParent(m_prevParent);
    m_prevParent->layout()->addWidget(this);
    m_fullScreenDialog->close();

    delete(m_fullScreenDialog);
    m_fullScreenDialog = NULL;

    m_requiresInitialize = true;

    m_width = m_prevParent->width();
    m_height = m_prevParent->height();

    m_prevParent = NULL;

    updateGL();
  }
  else
  {
    m_resizeEnabled = false;

    m_prevParent = parentWidget();
    m_fullScreenDialog = new QDialog();
    m_fullScreenDialog->setLayout(new QVBoxLayout());
    m_fullScreenDialog->layout()->setContentsMargins(0, 0, 0, 0);
    setParent(m_fullScreenDialog);
    m_fullScreenDialog->layout()->addWidget(this);
    m_fullScreenDialog->setModal(false);
    m_fullScreenDialog->setWindowFlags(Qt::SplashScreen);

    QRect geometry = QApplication::desktop()->screenGeometry(screenIndex);
    m_width = geometry.width();
    m_height = geometry.height();
    m_fullScreenDialog->setGeometry(geometry);

    m_requiresInitialize = true;
    m_fullScreenDialog->show();
    updateGL();
  }        
}

