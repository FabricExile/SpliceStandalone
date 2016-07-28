#include "BooleanAEWidget.h"

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

AEWidget * BooleanAEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new BooleanAEWidget(port ,parent);
}

BooleanAEWidget::BooleanAEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QVBoxLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal BooleanAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("BooleanAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("Boolean");
    rtValSetArraySize(values, m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
      rtValSetArrayElement(values, i, constructBooleanRTVal(m_widgets[i]->checkState() == Qt::Checked));

  );
	return values;
}

void BooleanAEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("BooleanAEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void BooleanAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("BooleanAEWidget::setValueArray", 

    // clear the layout
    if(rtValGetArraySize(values) != m_widgets.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_widgets.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
    
        // connect the color box to uiChanged()
        connect( m_widgets[i], SIGNAL( stateChanged(int) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      if ( rtValGetArrayElement(values, i).getBoolean() )
        m_widgets[i]->setCheckState( Qt::Checked );
      else
        m_widgets[i]->setCheckState( Qt::Unchecked );
    }

  );
}

void BooleanAEWidget::uiChanged()
{
  FABRIC_TRY("BooleanAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );
  AEWidget::uiChanged();
}
