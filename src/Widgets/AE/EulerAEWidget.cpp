#include "EulerAEWidget.h"

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

AEWidget * EulerAEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new EulerAEWidget(port ,parent);
}

EulerAEWidget::EulerAEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new QDoubleValidator(this);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal EulerAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("EulerAEWidget::getValueArray", FabricCore::RTVal(), 

  	values = rtValConstruct("Euler");
    rtValSetArraySize(values, m_widgetsX.size());

    std::vector<FabricCore::RTVal> args(3);
    for (unsigned int i = 0; i < m_widgetsX.size(); ++i)
    {
      QString x = m_widgetsX[i]->text();
      QString y = m_widgetsY[i]->text();
      QString z = m_widgetsY[i]->text();
      args[0] = constructFloat32RTVal(x.toFloat());
      args[1] = constructFloat32RTVal(y.toFloat());
      args[2] = constructFloat32RTVal(z.toFloat());
      rtValSetArrayElement(values, i, constructRTVal("Euler", 3, &args[0]));
    }
  );
	return values;
}

void EulerAEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("EulerAEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void EulerAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("EulerAEWidget::setValueArray", 

    unsigned int precision = 3;

    // clear the layout
    if(rtValGetArraySize(values) != m_widgetsX.size())
    {
      QLayoutItem* item;
      while ( ( item = m_listWidget->layout()->takeAt( 0 ) ) != NULL )
      {
        delete item->widget();
        delete item;
      }

      m_validator->setDecimals(precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_widgetsX.resize(rtValGetArraySize(values));
      m_widgetsY.resize(rtValGetArraySize(values));
      m_widgetsZ.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_widgetsX[i] = new ItemWidget(m_listWidget);
        m_widgetsX[i]->setFont( getApplicationWidgetFont() );
        m_widgetsX[i]->setValidator(m_validator);
        m_widgetsY[i] = new ItemWidget(m_listWidget);
        m_widgetsY[i]->setFont( getApplicationWidgetFont() );
        m_widgetsY[i]->setValidator(m_validator);
        m_widgetsZ[i] = new ItemWidget(m_listWidget);
        m_widgetsZ[i]->setFont( getApplicationWidgetFont() );
        m_widgetsZ[i]->setValidator(m_validator);
    
        // connect the color box to uiChanged()
        connect( m_widgetsX[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsY[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        connect( m_widgetsZ[i], SIGNAL( textChanged(const QString &) ), this , SLOT( uiChanged() ) );
        layout->addWidget( m_widgetsX[i], i, 0);
        layout->addWidget( m_widgetsY[i], i, 1);
        layout->addWidget( m_widgetsZ[i], i, 2);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      FabricCore::RTVal value = rtValGetArrayElement(values, i);
      m_widgetsX[i]->setText(QString::number(value.maybeGetMember("x").getFloat32(), 'g', precision));
      m_widgetsY[i]->setText(QString::number(value.maybeGetMember("y").getFloat32(), 'g', precision));
      m_widgetsZ[i]->setText(QString::number(value.maybeGetMember("z").getFloat32(), 'g', precision));
    }

  );
}

void EulerAEWidget::uiChanged()
{
  FABRIC_TRY("EulerAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}
