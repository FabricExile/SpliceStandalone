#include "Float32SliderAEWidget.h"

#include <QtGui/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QScrollBar>
#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QSizePolicy>

#include "macros.h"
#include "SpliceStandalone.h"

using namespace FabricSplice;

AEWidget * Float32SliderAEWidget::create( FabricSplice::DGPort port , QWidget* parent)
{
	return new Float32SliderAEWidget(port ,parent);
}

Float32SliderAEWidget::Float32SliderAEWidget(FabricSplice::DGPort port ,QWidget* parent)
    : AEWidget(port ,parent)
{
	setLayout(createLabelControlLayout());

  m_notifying = false;

  m_listWidget = new QWidget();
  m_listWidget->setLayout(new QGridLayout(m_listWidget));
	layout()->addWidget(m_listWidget);

  m_validator = new DoubleValidator(this);

  setPort(port);
			
	// QSpacerItem * spacerItem = new QSpacerItem(20,1,QSizePolicy::Expanding , QSizePolicy::Minimum);
	// list->addItem(spacerItem);

}

FabricCore::RTVal Float32SliderAEWidget::getValueArray()
{
  FabricCore::RTVal values;

  FABRIC_TRY_RETURN("Float32SliderAEWidget::getValueArray", FabricCore::RTVal(), 

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

void Float32SliderAEWidget::setPort(FabricSplice::DGPort port)
{
  FABRIC_TRY("Float32SliderAEWidget::setPort",
  
    AEWidget::setPort(port);
    setValueArray(port.getRTVal());

  );
}

void Float32SliderAEWidget::setValueArray(FabricCore::RTVal values)
{
  FABRIC_TRY("Float32SliderAEWidget::setValueArray", 

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

      float bottom = 0.0; // todo
      float top = 1.0; // todo

      m_validator->setRange(bottom, top, precision);
      m_validator->setNotation(QDoubleValidator::StandardNotation);

      QGridLayout * layout = (QGridLayout*)m_listWidget->layout();

      m_sliders.resize(rtValGetArraySize(values));
      m_widgets.resize(rtValGetArraySize(values));
      for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
      {
        m_sliders[i] = new QSlider(m_listWidget);
        m_sliders[i]->setRange(bottom * 1000.0f, top * 1000.0f);
        m_sliders[i]->setOrientation(Qt::Horizontal);

        m_widgets[i] = new ItemWidget(m_listWidget);
        m_widgets[i]->setFont( getApplicationWidgetFont() );
        m_widgets[i]->setValidator(m_validator);
        m_widgets[i]->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    
        connect( m_sliders[i], SIGNAL( sliderMoved(int) ), this , SLOT( sliderMoved(int) ) );
        connect( m_widgets[i], SIGNAL( textChanged(const QString &) ), this , SLOT( textChanged(const QString &) ) );

        layout->addWidget( m_sliders[i], i, 0);
        layout->addWidget( m_widgets[i], i, 1);
      }
    }

    for (unsigned int i = 0; i < rtValGetArraySize(values); ++i)
    {
      float value = rtValGetArrayElement(values, i).getFloat32();
      m_widgets[i]->setText(QString::number(value, 'g', precision));
    }

  );
}

void Float32SliderAEWidget::uiChanged()
{
  FABRIC_TRY("Float32SliderAEWidget::uiChanged", 

    FabricCore::RTVal values = getValueArray();
    m_port.setRTVal(values);

  );

  AEWidget::uiChanged();
}

void Float32SliderAEWidget::sliderMoved(int pos)
{ 
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_sliders.size();i++)
  {
    if(m_sliders[i] != sender)
      continue;

    double value = double(pos) / 1000.0;
    m_widgets[i]->setText(QString::number(value, 'g', m_validator->decimals()));
    break;
  }

  m_notifying = false;
  uiChanged();
}

void Float32SliderAEWidget::textChanged(const QString & text)
{
  if(m_notifying)
    return;
  m_notifying = true;

  QObject * sender = QObject::sender();
  for(size_t i=0;i<m_widgets.size();i++)
  {
    if(m_widgets[i] != sender)
      continue;

    float value = m_widgets[i]->text().toFloat();
    m_sliders[i]->setValue(value * 1000.0);
    break;
  }

  m_notifying = false;
  uiChanged();
}

