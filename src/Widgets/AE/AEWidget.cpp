#include "AEWidget.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QDialog>
#include <QPushButton>
#include <QLabel>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * AEWidget::create( FabricSplice::DGPort port ,QWidget* parent)
{
	return new AEWidget(port ,parent);
}

QString AEWidget::getLabel()
{
	std::string labelText;

  FABRIC_TRY_RETURN("AEWidget::getLabel", QString(),
  
    labelText = m_port.getName();

  );
	
  labelText +="   ";
	return QString(labelText.c_str() );
}


QHBoxLayout * AEWidget::createLabelControlLayout()
{
  QHBoxLayout * controlLayout = new QHBoxLayout;

	m_defaultLabelWidget = new QLabel( getLabel() );

  FABRIC_TRY_IGNORE("AEWidget::createLabelControlLayout",
	
    m_defaultLabelWidget->setToolTip( "" );

  );
	m_defaultLabelWidget->setMinimumWidth( 50 );
	m_defaultLabelWidget->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	m_defaultLabelWidget->setFixedHeight(18);
	m_defaultLabelWidget->setFont(getApplicationWidgetFont());
	controlLayout->setMargin( 2 );
	controlLayout->setSpacing( 1 );
	controlLayout->addWidget(m_defaultLabelWidget);

	return controlLayout;
}

void AEWidget::missingRegistration()
{
	QHBoxLayout * layout = createLabelControlLayout();

	QLabel * label = new QLabel( "Unknown Type" );
	label->setMinimumWidth( 50 );
	label->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
	layout->addWidget(label);
	layout->insertSpacing( -1 , 1000 );

	setLayout(layout);
}

AEWidget::AEWidget(FabricSplice::DGPort port , QWidget* parent)
    : QWidget(parent)
{
		
	m_port = port;

  FABRIC_TRY("AEWidget::AEWidget", 
    
    m_attributeName = m_port.getName();
  
  );
	
	// will be null in case no preset is setup .
	//m_presets		= m_description->getEntry<VariantMap>( "presets" );
	
  setMinimumWidth(40);
}

void AEWidget::setEnabledOnChildren(bool value)
{
	foreach (QObject * child , children())
	{
		if ( child->isWidgetType() )
		{
			const QMetaObject *	meta = child->metaObject();
			std::string className = meta->className();
			
			if ( className != "QLabel" && className != "QMenu" )
			{
				QWidget * widget = qobject_cast<QWidget*>(child);
				widget->setEnabled(value);
			}
		}
	}
}

void AEWidget::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu * menu = new QMenu(this);

	// add the action to the menu .
	// this is potentialy empty 
	menu->addActions(m_extraMenuActions);

	// show the menu 
	menu->exec(event->globalPos());
	
}

/*
void AEWidget::setPreset(VariantPtr value)
{
	setInternalValue(value);
}
*/

AEWidget::~AEWidget()
{
}	

void AEWidget::valueChanged()
{
	///emit AEWigetInternalChanged(m_attributeName);
}
	
void AEWidget::uiChanged()
{
	if (!m_silentUpdate)
	{
		emit AEWigetValueChanged(m_attributeName);
	}	
}

FabricCore::RTVal AEWidget::rtValConstruct(const char * dataType)
{
  if(m_port.isArray())
  {
    std::string arrayType = dataType;
    arrayType += "[]";
    return constructRTVal(arrayType.c_str());
  }
  return constructRTVal(dataType);
}

unsigned int AEWidget::rtValGetArraySize(const FabricCore::RTVal value)
{
  if(value.isArray())
    return value.getArraySize();
  return 1;
}

FabricCore::RTVal AEWidget::rtValGetArrayElement(const FabricCore::RTVal value, unsigned int index)
{
  if(value.isArray())
    return value.getArrayElement(index);
  return value;
}

void AEWidget::rtValSetArraySize(FabricCore::RTVal & value, unsigned int size)
{
  if(value.isArray())
    value.setArraySize(size);
}

void AEWidget::rtValSetArrayElement(FabricCore::RTVal & value, unsigned int index, const FabricCore::RTVal & element)
{
  if(value.isArray())
    value.setArrayElement(index, element);
  else
    value = element;
}

float AEWidget::getPortOption(const char * key, float defaultValue)
{
  if(!m_port.isValid())
    return defaultValue;

  FabricCore::Variant option = m_port.getOption(key);
  if(option.isNull())
    return defaultValue;

  if(option.isSInt8())
    return option.getSInt8();
  if(option.isSInt16())
    return option.getSInt16();
  if(option.isSInt32())
    return option.getSInt32();
  if(option.isSInt64())
    return option.getSInt64();
  if(option.isFloat32())
    return option.getFloat32();
  if(option.isFloat64())
    return option.getFloat64();

  return defaultValue;
}
