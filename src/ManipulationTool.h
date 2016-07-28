#ifndef __MANIPULATIONTOOL_H__
#define __MANIPULATIONTOOL_H__

#include <QObject>
#include <QEvent>

#include <FabricSplice.h>

#include "Widgets/GLWidget.h"

namespace FabricSplice
{
  class ManipulationCmd
  {

  private:
    FabricCore::RTVal m_rtval_commands;
    
  public:
    ManipulationCmd(); 
    virtual ~ManipulationCmd(); 
    
    bool redoIt();
    bool undoIt();
    bool isUndoable() const;

    // We set the static commands pointer, and then construct the command. 
    static FabricCore::RTVal s_rtval_commands;
  };


  class ManipulationTool 
  {
  public:
    ManipulationTool(GLWidget * glView);
    virtual ~ManipulationTool() {}

    virtual void toolOnSetup();
    virtual void toolOffCleanup();

    bool onEvent(QEvent *event);
    bool isActive() { return m_active;}

  private:
    bool  m_active;
    FabricCore::RTVal mEventDispatcher;

    GLWidget * mView;
  };
};

#endif
