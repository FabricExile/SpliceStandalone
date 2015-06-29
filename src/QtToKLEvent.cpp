
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>

#include <FabricSplice.h>

#include <map>
#include <iostream>

using namespace FabricSplice;

FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::RTVal viewport)
{

  // Now we translate the Qt events to FabricEngine events..
  FabricCore::RTVal klevent;

  if(event->type() == QEvent::Enter)
  {
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");
    // );
  }
  else if(event->type() == QEvent::Leave)
  {
    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");
    // );
  }
  else if (event->type() == QEvent::KeyPress || event->type() == QEvent::KeyRelease) 
  {
    QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("KeyEvent");
      klevent.setMember("key", constructUInt32RTVal(keyEvent->key()));
      klevent.setMember("count", constructUInt32RTVal(keyEvent->count()));
      klevent.setMember("isAutoRepeat", constructBooleanRTVal(keyEvent->isAutoRepeat()));
    // );
  } 
  else if ( event->type() == QEvent::MouseMove || 
            event->type() == QEvent::MouseButtonDblClick || 
            event->type() == QEvent::MouseButtonPress || 
            event->type() == QEvent::MouseButtonRelease
          ) 
  {
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseEvent");

      FabricCore::RTVal klpos = constructRTVal("Vec2");
      klpos.setMember("x", constructFloat32RTVal(mouseEvent->pos().x()));
      klpos.setMember("y", constructFloat32RTVal(mouseEvent->pos().y()));

      klevent.setMember("button", constructUInt32RTVal(mouseEvent->button()));
      klevent.setMember("buttons", constructUInt32RTVal(mouseEvent->buttons()));
      klevent.setMember("pos", klpos);
    // );
  } 
  else if (event->type() == QEvent::Wheel) 
  {
    QWheelEvent *mouseWheelEvent = static_cast<QWheelEvent *>(event);

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent = constructObjectRTVal("MouseWheelEvent");

      FabricCore::RTVal klpos = constructRTVal("Vec2");
      klpos.setMember("x", constructFloat32RTVal(mouseWheelEvent->pos().x()));
      klpos.setMember("y", constructFloat32RTVal(mouseWheelEvent->pos().y()));

      klevent.setMember("buttons", constructUInt32RTVal(mouseWheelEvent->buttons()));
      klevent.setMember("delta", constructSInt32RTVal(mouseWheelEvent->delta()));
      klevent.setMember("pos", klpos);
    // );
  }

  if(klevent.isValid())
  {
    int eventType = int(event->type());

    // FABRIC_TRY_RETURN("ManipulationTool::onEvent", false,
      klevent.setMember("eventType", constructUInt32RTVal(eventType));

      QInputEvent *inputEvent = static_cast<QInputEvent *>(event);
      klevent.setMember("modifiers", constructUInt32RTVal(inputEvent->modifiers()));

      //////////////////////////
      // Setup the viewport
      klevent.setMember("viewport", viewport);

      //////////////////////////
      // Setup the Host
      // We cannot set an interface value via RTVals.
      FabricCore::RTVal host = constructObjectRTVal("Host");
      host.setMember("hostName", constructStringRTVal("Splice Standalone"));
      klevent.setMember("host", host);
  }
  return klevent;
}
