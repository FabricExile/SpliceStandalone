#include "Float32AEWidget.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QMenu>
#include <QScrollBar>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QSizePolicy>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * Float32AEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new Float32AEWidget(port ,parent);
}

Float32AEWidget::Float32AEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new DoubleValidator(this);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Float32AEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Float32AEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("Float32");
    rtValSetArraySize(values, m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      QString value = m_widgets[i]->text();
      rtValSetArrayElement(values, i, constructFloat32RTVal(value.toFloat()));
    }
  );
	return values;
}

void Float32AEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("Float32AEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void Float32AEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Float32AEWidget::setValueArray", 

    unsigned int precision = 3;

    // clear the layout
    if(rtValGetArraySize(values) != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      float bottom = getPortOption("uiMin", 0.0);
      float top = getPortOption("uiMax", 1.0);

      m_validator->setRange(bottom, top, precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      m_widgets.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
        m_widgets[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      float value = rtValGetArrayElement(values, i).getFloat32();
      m_widgets[i]->setText(QString::number(value, 'g', precision));
    }

  );
}

void Float32AEWidget::uiChanged()
{
  FABRIC_TRY("Float32AEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}
