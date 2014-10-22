#ifndef __QT2OKLEVENT_H__
#define __QT2OKLEVENT_H__

#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <FabricSplice.h>


FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::RTVal viewport);

#endif
