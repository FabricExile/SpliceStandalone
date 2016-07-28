#ifndef __QT2OKLEVENT_H__
#define __QT2OKLEVENT_H__

#include <QObject>
#include <QEvent>

#include <FabricSplice.h>


FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::RTVal viewport);

#endif
