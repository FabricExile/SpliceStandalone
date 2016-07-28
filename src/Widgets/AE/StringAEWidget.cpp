#include "StringAEWidget.h"

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
#include "utility.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * StringAEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new StringAEWidget(port ,parent);
}

StringAEWidget::StringAEWidget(FabricSplice::DGPort port ,QWidget* parent)
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

FabricCore::RTVal StringAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("StringAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("String");
    rtValSetArraySize(values, m_widgets.size());
  	for (unsigned int i = 0; i < m_widgets.size(); ++i)
    {
      std::string value = stdStringFromQString(m_widgets[i]->text());
      rtValSetArrayElement(values, i, constructStringRTVal(value.c_str()));
    }
  );
	return values;
}

void StringAEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("StringAEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void StringAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("StringAEWidget::setValueArray", 

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
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        m_listWidget->layout()->addWidget( m_widgets[i]);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      std::string value = rtValGetArrayElement(values, i).getStringCString();
      m_widgets[i]->setText(value.c_str());
    }

  );
}

void StringAEWidget::uiChanged()
{
  FABRIC_TRY("StringAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}
