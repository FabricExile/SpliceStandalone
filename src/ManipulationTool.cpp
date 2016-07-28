#include <QApplication>
#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "ManipulationTool.h"
#include "QtToKLEvent.h"

#include <FabricSplice.h>
#include "macros.h"

#include <map>
#include <iostream>

using namespace FabricSplice;

/////////////////////////////////////////////////////
// ManipulationCmd

FabricCore::RTVal ManipulationCmd::s_rtval_commands;

ManipulationCmd::ManipulationCmd()
{
  m_rtval_commands = s_rtval_commands;
}

ManipulationCmd::~ManipulationCmd()
{
}

bool ManipulationCmd::redoIt()
{
  if(m_rtval_commands.isValid())
  {
    for(uint32_t i=0; i<m_rtval_commands.getArraySize(); i++)
    {
      m_rtval_commands.getArrayElement(i).callMethod("", "doAction", 0, 0);
    }
  }
  return true;
}

bool ManipulationCmd::undoIt()
{
  if(m_rtval_commands.isValid())
  {
    for(uint32_t i=0; i<m_rtval_commands.getArraySize(); i++)
    {
      m_rtval_commands.getArrayElement(i).callMethod("", "undoAction", 0, 0);
    }
  }
  return true;
}

/////////////////////////////////////////////////////
// ManipulationTool

class EventFilterObject : public QObject
{
  public:
    ManipulationTool *tool;
    bool eventFilter(QObject *object, QEvent *event);
};

static EventFilterObject sEventFilterObject;

ManipulationTool::ManipulationTool(GLWidget * glView) 
  : m_active(false), mView(glView)
{

}

void ManipulationTool::toolOnSetup()
{
  FABRIC_TRY("ManipulationTool::toolOnSetup",
    
    FabricCore::RTVal eventDispatcherHandle = FabricSplice::constructObjectRTVal("EventDispatcherHandle");
    if(eventDispatcherHandle.isValid()){
      mEventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);

      if(mEventDispatcher.isValid()){
        mEventDispatcher.callMethod("", "activateManipulation", 0, 0);
      }
    }
  );
  m_active = true;

  sEventFilterObject.tool = this;
  mView->installEventFilter(&sEventFilterObject);
  mView->setFocus();
  mView->setMouseTracking(true);

  mView->updateGL();
}

void ManipulationTool::toolOffCleanup()
{

  mView->removeEventFilter(&sEventFilterObject);
  mView->clearFocus();

  FABRIC_TRY("ManipulationTool::toolOffCleanup",
    if(mEventDispatcher.isValid()){
      mEventDispatcher.callMethod("", "deactivateManipulation", 0, 0);
    }
  );
   
  m_active = false;
  mView->setMouseTracking(false);
  mView->updateGL();

}

bool EventFilterObject::eventFilter(QObject *object, QEvent *event)
{
  return tool->onEvent(event);
}

bool ManipulationTool::onEvent(QEvent *event)
{
  if(!mEventDispatcher.isValid())
  {
    return false;
  }

  // skip the alt key, so that we can continue to use the camera
  if(QApplication::keyboardModifiers().testFlag(Qt::AltModifier))
    return false;

  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent = QtToKLEvent(event, mView->getInlineViewport());

  if(klevent.isValid())
  {
    bool result = false;
    FabricCore::RTVal host = klevent.maybeGetMember("host");

    //////////////////////////
    // Invoke the event...
    mEventDispatcher.callMethod("Boolean", "onEvent", 1, &klevent);

    result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
    if(result)
      event->accept();

    if(host.maybeGetMember("redrawRequested").getBoolean())
      mView->updateGL();

    if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean())
    {
      // Cache the rtvals in a static variable that the command will then stor in the undo stack.
      ManipulationCmd::s_rtval_commands = host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0);
    }

    klevent.invalidate();
    return result;
  }
  // the event was not handled by FabricEngine manipulation system. 
  return false;
}
