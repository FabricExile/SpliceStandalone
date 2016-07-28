#include "AttributeEditorWidget.h"
#include "AE/AEWidgetFactory.h"
#include "AE/AEWidget.h"

#include <QPushButton>
#include <QHBoxLayout>

#include <QTimer>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QFrame>
#include <QDialog>		
#include <QPushButton>
#include <QLineEdit>

#include "macros.h"

using namespace FabricSplice;

AttributeEditorWidget::AttributeEditorWidget(QWidget* parent)
    : QWidget(parent)
{
	
	QVBoxLayout *mainLayout = new QVBoxLayout();
	mainLayout->setMargin(0);
	mainLayout->setSpacing(0);
	setLayout(mainLayout);	
}

AttributeEditorWidget::~AttributeEditorWidget()
{
}	

void AttributeEditorWidget::clearAllWidgets()
{
  // make sure we delete each item in the Map
  QLayoutItem* item;
  while ( ( item = layout()->takeAt( 0 ) ) != NULL )
  {
    delete item->widget();
    delete item;
  }
  m_widgetMap.clear();
}

void AttributeEditorWidget::setWrapper(SpliceGraphWrapper::Ptr wrapper)
{
  clearAllWidgets();

  m_wrapper = wrapper;
  if(!m_wrapper)
    return;

  const AEWidgetFactory & factory = AEWidgetFactory::widgetFactory();

  FabricSplice::DGGraph graph = m_wrapper->getGraph();
  for(unsigned int i=0;i<graph.getDGPortCount();i++)
  {
    FabricSplice::DGPort port = graph.getDGPort(i);
    if(port.isArray())
      continue;
    std::string portName = port.getName();
    if(portName == "time")
      continue;

    AEWidget * widget = factory.create(port);
    if(!widget)
    {
      continue;
    }

    m_widgetMap[portName] = widget;
    layout()->addWidget(widget);

    connect(widget, SIGNAL(AEWigetValueChanged(std::string)) ,
        this , SLOT(widgetValueChanged(std::string)) );

    // we silence the uiUpate signal as to double update
    widget->setSilentUpdate(false);
  }
}

void AttributeEditorWidget::widgetValueChanged(std::string attributeName)
{
	// for applications using QT
  QSpliceGraphWrapper wrapper;
  wrapper.wrapper = m_wrapper;
	emit attributeChanged(wrapper, attributeName);
}

AEWidget * AttributeEditorWidget::getWidget(std::string argName)
{
	return m_widgetMap[argName];
}

void AttributeEditorWidget::setWidgetPort( std::string name, FabricSplice::DGPort port , bool silent )
{
	// we want to make sure our args stay in sync
  FABRIC_TRY("AttributeEditorWidget::setWidgetRTVal", 

  	// in case we are displaying the UI we want to update it too 
  	AEWidget * widget = m_widgetMap[name];
  	if (widget)
  	{
  		// we set the internal value silently as no to triger more updates
  		widget->setSilentUpdate(silent);
  		widget->setPort( port );
  		widget->setSilentUpdate(false);

  		// the value changed this might be interesting to other part of the system		
  		emit valueForWidgetChanged( name );
  	}

  );
	
}

void AttributeEditorWidget::updateAllWidgets()
{
  // todo: update all widgets
 //  NodeData::DGPortList & args = m_nodeData->getArgs();
	// for (WidgetMap::iterator it = m_widgetMap.begin() ; it != m_widgetMap.end() ; it++ ) 
	// {
 //    FABRIC_TRY("AttributeEditorWidget::updateAllWidgets", 

 //      std::string name = it->first;
 //      for(size_t i=0;i<args.size();i++)
 //      {
 //        if(args[i].getName() == name)
 //        {
 //      		it->second->setPort( args[i] );
 //        }
 //      }
 //    );
	// }
}
